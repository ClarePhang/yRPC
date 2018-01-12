/* comm_driver.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-20
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#ifndef COMM_DRIVER_H__
#define COMM_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

//#define USING_COMM_TIMEOUT
//#define USING_WRITE_HANDLER

#define COMM_VERSION    "V1.1"

#define EXTERN  extern
#define STATIC  static
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

EXTERN void comm_version(void);
EXTERN void comm_methods(void);
EXTERN void comm_set_timeout(struct timeval &tv);
EXTERN void comm_set_cyclecheck(struct timeval &tv);

EXTERN int comm_create(CommEventHandler handler, struct sockaddr *s_addr, size_t s_len);
EXTERN void comm_destroy(void);
EXTERN void cycle_check_en(bool enable);

EXTERN int comm_connect(struct sockaddr *s_addr, size_t s_len, struct timeval &tv, void **fdptr);
EXTERN void comm_disconnect(void *fdptr);

EXTERN int comm_send(const void *fdptr, const void *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif // COMM_DRIVER_H__

