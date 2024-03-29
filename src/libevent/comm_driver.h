/* comm_driver.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-20
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#ifndef COMM_DRIVER_H__
#define COMM_DRIVER_H__

//#define USING_COMM_TIMEOUT
//#define USING_WRITE_HANDLER

#define COMM_VERSION    "V1.1"

#define COMM_DEBUE  "COMMUNICATION_DEBUG"

#define COMMEventRecv        0x01
#define COMMEventSend        0x02
#define COMMEventConnect     0x04
#define COMMEventDisconnect  0x08
#define COMMEventCheck       0x10
#define COMMEventRTimeout    0x20
#define COMMEventSTimeout    0x40
#define COMMEventReserved    0x80

typedef int (*CommEventHandler)(unsigned int type, void *fd_ptr, void *data, size_t data_len);

class COMMDriver
{
public:
    COMMDriver()
    {
        comm_main_thread_id = 0;
        comm_accept_thread_id = 0;
    }
    ~COMMDriver()
    {
        comm_main_thread_id = 0;
        comm_accept_thread_id = 0;
    }

public:
    void version(void);
    void methods(void);
    void setTimeout(struct timeval &tv);
    void setCyclecheck(struct timeval &tv);
    void cyclecheckEn(bool enable);

    int create(CommEventHandler handler, struct sockaddr *s_addr, size_t s_len);
    void destroy(void);

    int connect(struct sockaddr *s_addr, size_t s_len, struct timeval &tv, void **fdptr);
    void disconnect(void *fdptr);

    int send(const void *fdptr, const void *data, size_t size);
    
private:
    pthread_t comm_main_thread_id;
    pthread_t comm_accept_thread_id;
};

#endif // COMM_DRIVER_H__

