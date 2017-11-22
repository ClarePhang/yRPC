#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
     
#include "threadpool.h"

void callback(void *arg)
{
    static unsigned int call_back = 0;
    usleep(1000);
    call_back++;
    if(call_back == 10000)
    {
        printf("worker running....\n");
        call_back = 0;
    }
}

void highcallback(void *arg)
{
    static unsigned int high_callback = 0;
    usleep(1000);
    high_callback++;
    if(high_callback == 5000)
    {
        printf("high task runing.\n");
        high_callback = 0;
    }
}


int main(void)
{
    unsigned int count = 0;
    ThreadPool thread_pool;

    thread_pool.create(30, 50, 300);

    printf("current thread num :%d\n", thread_pool.getCurThreadSize());
    while(true)
    {
        usleep(3);
        thread_pool.addWork(LowPriority , callback , NULL);
        if(count % 9 == 0)
        {
            thread_pool.addWork(HighPriority , highcallback , NULL);
        }
    }
    
    thread_pool.destroy();
    
    return 0;
}

