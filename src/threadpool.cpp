/* threadpool.cpp
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-21
 * Author: zhangqiyin/Konishi
 * Email : zhangqiyin@hangsheng.com.cn
 */
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>

#include "threadpool.h"

#define TDP_DEBUG   printf
#define TDP_INFO    printf
#define TDP_WARN    printf
#define TDP_ERROR   printf

#define tp_timeradd(tp, vp, ttp)					\
	do {								\
		(ttp)->tv_sec = (tp)->tv_sec + (vp)->tv_sec;		\
		(ttp)->tv_nsec = (tp)->tv_nsec+ (vp)->tv_nsec;       \
		if ((ttp)->tv_nsec >= 1000000000) {			\
			(ttp)->tv_sec++;				\
			(ttp)->tv_nsec -= 1000000000;			\
		}							\
	} while (0)

int ThreadPool::max_thread_size = 0;
pthread_attr_t ThreadPool::thread_attr;
volatile int ThreadPool::low_queue_num = 0;
volatile int ThreadPool::high_queue_num = 0;
volatile int ThreadPool::cur_thread_num = 0;
volatile bool ThreadPool::shut_down_flag = false;
ThreadPoolWorker *ThreadPool::low_queue_head = NULL;
ThreadPoolWorker *ThreadPool::high_queue_head = NULL;
pthread_cond_t ThreadPool::thread_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t ThreadPool::thread_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool::queue_mutex = PTHREAD_MUTEX_INITIALIZER;

ThreadPool::ThreadPool()
{
    max_queue_size = 0;
    fix_thread_size = 0;
    thread_id = NULL;
}

ThreadPool::~ThreadPool()
{
    max_queue_size = 0;
    fix_thread_size = 0;
    thread_id = NULL;
}

int ThreadPool::create(int fix_thread_num, int dyn_thread_num, int max_queue_size)
{
    int i = 0;
    size_t stack_size = 0;

    stack_size = THREAD_DEFAULT_STACK_SIZE*1024;
    if(pthread_attr_init(&thread_attr) != 0)
        return -1;
//    if(pthread_attr_setschedpolicy(&thread_attr, SCHED_RR) != 0)
//        goto CREATE_FAILED;
    if(pthread_attr_setstacksize(&thread_attr, stack_size) != 0)
        goto CREATE_FAILED;

    thread_id = (pthread_t *)malloc(fix_thread_num * sizeof(pthread_t));
    if(!thread_id)
    {
        TDP_ERROR("ThreadPool : %s: malloc thread-ids failed, errno:%d,error:%s.\n", __FUNCTION__, errno, strerror(errno));
        goto CREATE_FAILED;
    }

    if(0 == fix_thread_num)
    {
        TDP_WARN("ThreadPool : %s: At lease one fixed-thread!\n", __FUNCTION__);
        fix_thread_num = 1;
    }

    if(max_queue_size < (fix_thread_num + dyn_thread_num))
    {
        TDP_WARN("ThreadPool : %s: max_queue_size at lease >= max_thread_size!\n", __FUNCTION__);
        max_queue_size = fix_thread_num + dyn_thread_num + 1;
    }

    this->low_queue_num = 0;
    this->high_queue_num = 0;
    this->shut_down_flag = false;
    this->max_queue_size = max_queue_size;
    this->cur_thread_num = fix_thread_num;
    this->fix_thread_size = fix_thread_num;
    this->max_thread_size = fix_thread_num + dyn_thread_num;

    if(dyn_thread_num != 0)
    {
        if(pthread_create(&manager_id, NULL, threadpoolManager, NULL) != 0)
        {
            TDP_ERROR("ThreadPool : %s: pthread_create failed, errno:%d,error:%s.\n", __FUNCTION__, errno, strerror(errno));
            goto CREATE_FAILED;
        }
    }
    
    for(i = 0; i < fix_thread_size; i++)
    {
        if(pthread_create(&(thread_id[i]), &thread_attr, fixThreadRoutine, NULL) != 0)
        {
            TDP_ERROR("ThreadPool : %s: pthread_create failed, errno:%d,error:%s.\n", __FUNCTION__, errno, strerror(errno));
            goto CREATE_FAILED;
        }
    }

    return 0;
    
CREATE_FAILED:
    if(i > 0)
    {
        for(i--; ;i--)
        {
            pthread_cancel(thread_id[i]);
            pthread_join(thread_id[i], NULL);
            if(0 == i)
                break;
        }
        pthread_cancel(manager_id);
        pthread_join(manager_id, NULL);
    }
    if(thread_id)
        free(thread_id);
    pthread_attr_destroy(&thread_attr);
    
    return -1;
}

void ThreadPool::destroy(void)
{
    int i = 0;
    ThreadPoolWorker *worker = NULL;

    if(shut_down_flag)
        return ;

    shut_down_flag = true;
    usleep(5*1000);

    pthread_attr_destroy(&thread_attr);
    pthread_mutex_lock(&thread_mutex);
    pthread_cond_broadcast(&thread_cond);
    pthread_mutex_unlock(&thread_mutex);

    pthread_join(manager_id, NULL);
    for(i = 0; i < fix_thread_size; i++)
    {
        pthread_join(thread_id[i], NULL);
    }
    if(thread_id)
        free(thread_id);

    while(high_queue_head)
    {
        worker = high_queue_head;
        high_queue_head = worker->next;
        if(worker->arg)
            free(worker->arg);
        free(worker);
    }

    while(low_queue_head)
    {
        worker = low_queue_head;
        low_queue_head = worker->next;
        if(worker->arg)
            free(worker->arg);
        free(worker);
    }
}

int ThreadPool::addWork(ThreadPoolPriority priority, void (*routine)(void *), void *arg)
{
    ThreadPoolWorker **queue_head;
    ThreadPoolWorker *member, *work;

    if(!routine)
    {
        TDP_ERROR("ThreadPool : %s:callback function Invalid argment.\n", __FUNCTION__);
        if(arg)
            free(arg);
        return -1;
    }

    if(LowPriority == priority)
    {
        if(low_queue_num > max_queue_size)
        {
            TDP_ERROR("ThreadPool : %s:low queue size is too large, %d:%d.\n", __FUNCTION__, low_queue_num, max_queue_size);
            if(arg)
                free(arg);
            return -1;
        }
    }
    else
    {
        if(high_queue_num > max_queue_size)
        {
            TDP_ERROR("ThreadPool : %s:high queue size is too large, %d:%d.\n", __FUNCTION__, low_queue_num, max_queue_size);
            if(arg)
                free(arg);
            return -1;
        }
    }
    
    work = (ThreadPoolWorker *)malloc(sizeof(ThreadPoolWorker));
	if(work == NULL)
	{
		TDP_ERROR("ThreadPool : %s:malloc worker failed.\n", __FUNCTION__);
		if(arg)
			free(arg);
		return -1;
	}

	work->routine = routine;
	work->arg = arg;
	work->next = NULL;

    pthread_mutex_lock(&thread_mutex);
    if(priority == HighPriority)
        queue_head = &high_queue_head;
    else
        queue_head = &low_queue_head;
    
    member = *queue_head;
    if(member == NULL)
    {
        *queue_head = work;
    }
    else
    {
        while(member->next != NULL)
            member = member->next;

        member->next = work;
    }

    assert(*queue_head != NULL);
    if(priority == LowPriority)
        low_queue_num++;
    else
        high_queue_num++;
    pthread_cond_signal(&thread_cond);
    pthread_mutex_unlock(&thread_mutex);
    
    return 0;
}

int ThreadPool::getCurQueueSize(void)
{
    return (high_queue_num + low_queue_num);
}

int ThreadPool::getCurThreadSize(void)
{
    return cur_thread_num;
}

void *ThreadPool::fixThreadRoutine(void *arg)
{
    ThreadPoolWorker *worker = NULL;

    TDP_INFO("ThreadPool : fix_thread_routine is running: %lu\n", pthread_self());

    while(true)
    {
        pthread_mutex_lock(&thread_mutex);
        while((high_queue_num == 0) && (low_queue_num == 0) && !shut_down_flag)
        {
            pthread_cond_wait(&thread_cond, &thread_mutex);
        }

        if(shut_down_flag)
        {
            pthread_mutex_unlock(&thread_mutex);
            break;
        }

        if(high_queue_num)
        {
            high_queue_num--;
            worker = high_queue_head;
            high_queue_head = worker->next;
        }
        else if(low_queue_num)
        {
            low_queue_num--;
            worker = low_queue_head;
            low_queue_head = worker->next;
        }
        assert(worker != NULL);
        pthread_mutex_unlock(&thread_mutex);

        // change thread high priority
        if(worker->routine)
            (*(worker->routine))(worker->arg);
        if(worker->arg)
            free(arg);
        free(worker);
        worker =NULL;
        // change thread normal priority
    }
    
    TDP_INFO("ThreadPool : fix_thread_routine will exit: %lu\n", pthread_self());
    pthread_exit(NULL);
}

void *ThreadPool::dynThreadRoutine(void *arg)
{
    int result = -1;
    struct timespec now;
    ThreadPoolWorker *worker = NULL;
    struct timespec tp = {DYNA_THREAD_WAIT_TIME, 0};
    
    // make dynamic thread detached
    pthread_detach(pthread_self());
    
    TDP_INFO("ThreadPool : dyn_thread_routine is running: %lu\n", pthread_self());

    while(true)
    {
        pthread_mutex_lock(&thread_mutex);
        if((high_queue_num == 0) && (low_queue_num == 0) && !shut_down_flag)
        {
            clock_gettime(CLOCK_REALTIME,&now);
            tp_timeradd(&tp, &now, &now);
            result = pthread_cond_timedwait(&thread_cond, &thread_mutex, &now);
            if(ETIMEDOUT == result)
            {
                pthread_mutex_unlock(&thread_mutex);
                break;
            }
            else if(result < 0)
            {
                pthread_mutex_unlock(&thread_mutex);
                TDP_WARN("ThreadPool : Dynamic thread wait failed, errno:%d,error:%s.\n", errno, strerror(errno));
                continue;
            }
        }

        if(shut_down_flag)
        {
            pthread_mutex_unlock(&thread_mutex);
            break;
        }

        if(high_queue_num)
        {
            high_queue_num--;
            worker = high_queue_head;
            high_queue_head = worker->next;
        }
        else if(low_queue_num)
        {
            low_queue_num--;
            worker = low_queue_head;
            low_queue_head = worker->next;
        }
        else
        {
            pthread_mutex_unlock(&thread_mutex);
            continue;
        }
        
        assert(worker != NULL);
        pthread_mutex_unlock(&thread_mutex);

        // change thread high priority
        if(worker->routine)
            (*(worker->routine))(worker->arg);
        if(worker->arg)
            free(arg);
        free(worker);
        worker =NULL;
        // change thread normal priority
    }
    
    TDP_INFO("ThreadPool : dyn_thread_routine will exit: %lu\n", pthread_self());
    subThreadNum();
    pthread_exit(NULL);
}

void *ThreadPool::threadpoolManager(void *arg)
{
    pthread_t dynamic_thread_id;
    
    TDP_INFO("ThreadPool : threadpool manager is running: %lu\n", pthread_self());

    while(true)
    {
        if(shut_down_flag)
            break;

        if((high_queue_num + low_queue_num) > cur_thread_num)
        {
            if(cur_thread_num == max_thread_size)
            {
                // all threads is running, wait a while for check
                TDP_WARN("ThreadPool : all threads is runing, full loading.\n");
                TDP_INFO("ThreadPool : high queue = %d, low queue %d, current thread = %d\n", high_queue_num, low_queue_num, cur_thread_num);
                sleep(DYNA_THREAD_WAIT_TIME);
                continue;
            }

            // ceate a dynamic thread
            if(pthread_create(&dynamic_thread_id, &thread_attr, dynThreadRoutine, NULL) != 0)
            {
                TDP_WARN("ThreadPool : create dynamic thread failed, errno:%d,error:%s..\n", errno, strerror(errno));
                continue;
            }
            TDP_INFO("ThreadPool : reate dynamic thread %lu\n", dynamic_thread_id);
            addThreadNum();
        }
        else
        {
            sleep(MANAGER_THREAD_CHECK_TIME);
            TDP_INFO("ThreadPool : high queue = %d, low queue %d, current thread = %d\n", high_queue_num, low_queue_num, cur_thread_num);
        }
    }
    
    TDP_INFO("ThreadPool : threadpool manager will exit: %lu\n", pthread_self());
    pthread_exit(NULL);
}

void ThreadPool::addThreadNum(void)
{
    pthread_mutex_lock(&queue_mutex);
    cur_thread_num++;
    pthread_mutex_unlock(&queue_mutex);
}

void ThreadPool::subThreadNum(void)
{
    pthread_mutex_lock(&queue_mutex);
    cur_thread_num--;
    pthread_mutex_unlock(&queue_mutex);
}

/* the following function is not useful
 * set websit: http://www.cnblogs.com/imapla/p/4234258.html
 */
void ThreadPool::setThreadPriority(int priority)
{
    int policy = 0;
    struct sched_param param;
    pthread_t self_id= pthread_self();

    if(pthread_getschedparam(self_id, &policy, &param) < 0)
        return ;
    param.sched_priority = priority;
    param.__sched_priority = priority;
    if(pthread_setschedparam(self_id, policy, &param) < 0)
        return ;
}
