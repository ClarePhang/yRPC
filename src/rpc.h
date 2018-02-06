/****************************************************************************************
 * @brief       ERPC application interface
 * @details     An embedded Remote Procedure Call system.
 * @author      Konishi
 * @version     V0.2
 * @email       konishi5202@126.com
 * @date        2018-01-30
****************************************************************************************/
#ifndef RPC_H__
#define RPC_H__
#include <string>

using namespace std;

typedef void (*ServiceHandler)(void *msg, void *data, size_t len);
typedef void (*ObserverHandler)(void *msg, void *data, size_t len);

/***************************************************************************************
 * @class ERPC
 * @brief ERPC interface class
***************************************************************************************/
class ERPC
{
public:
    ERPC();
    virtual ~ERPC();
    
public:
    /**
     * Get ERPC instance pointer.\n
     * This is the first function you called for using ERPC. Every process only have one ERPC instance.
     * @return ERPC instance pointer:
     * - NULL mean get failed;
     * - not NULL mean get ERPC instance OK.
     */
    static ERPC *getInstance(void);
    /**
     * Initialize ERPC system.\n
     * You must initialize ERPC system before using ERPC.
     * @param[in] process_name the name of a service you want to start.Match with the section name \n
     *                         of rpc.conf.\n
     *                         ERPC will use process_name section configuration to config this process.
     * @param[in] conf_path  The path of ERPC configuration file. You can not use the param or pass\n
     *                       "" to initRPC(), ERPC system will refer RPCCONFIG_PATH environment as conf.
     * @return 
     * -  0 :init ERPC system OK.
     * - -1 :init ERPC system failed!
     */
    virtual int initRPC(const string &process_name, const string &conf_path = "") = 0;
    /**
     * Register a service function into ERPC system. The service function type must be :\n
     * void (*ServiceHandler)(void *msg, void *data, size_t len);
     * @param[in] service Service name, matching with the remote call proxyCall() param func;\n
     *                    Ensure that one process can't have the same service name, otherwise\n
     *                    the registration will fail.
     * @param[in] func   The pointer to the function of specific service.
     * @return 
     * -  0 : register service at func ok.
     * - -1 : register service failed!
     */
    virtual int registerService(const string &service, ServiceHandler func) = 0;
    /**
     * Unregister a service from ERPC system.
     * @param[in] service The service name you have registered into ERPC system before.
     * @return 
     * -  0 : unregister service ok.
     * - -1 : unregister service failed!
     */
    virtual int unregisterService(const string &service) = 0;
    /**
     * Set the response data for remote calling.\n
     * This function will be called in service function.
     * @param[in] msg The first param of void (*ServiceHandler)(void *msg, void *data, size_t len);\n
     *                Just pass the msg to setResponse() first praram.
     * @param[in] response_data The data you want to return remote calling.\n
     *                          Be sure the data has been serialization.
     * @param[in] response_len The length of response_data.If no data return, just set response_data = NULL\n
     *                         and response_len = 0.
     * @return 
     * -  0 : return response data to remote call OK.
     * - -1 : return response data failed!
     */
    virtual int setResponse(void *msg, void *response_data, size_t response_len) = 0;
    /**
     * Remote calls func service in module module.
     * @param[in] module Where the func service be seated in.\n
     *                   The module name must match with ModuleConfiguration section key name.
     * @param[in] func  The service name you want to call. Math with the name on registerService().
     * @param[in] send  The data passing to remote service.Be sure the data has been serialization.
     * @param[in] slen  The length of send data.
     * @param[out] recv  The data return from remote service, return by service call setResponse().
     * @param rlen     [in]  Mean the length of recv buffer.\n
     *                 [out] Mean the length of recv data.
     * @param[in] tv  The time you want to wait until response data return.\n
     * @return 
     * -  0 : remote call service function ok, and response data will be put in recv and rlen.
     * - -1 : remote call failed!
     * @note  1.If you don't care about the data return from remote service, just set \n
     *          recv = NULL and rlen = NULL, or recv = NULL and *rlen = 0.\n
     *        2.If you pass tv = NULL, or *tv = {0, 0}, ERPC system will use ERPC default\n
     *          timeout configuration.
     */
    virtual int proxyCall(const string &module, const string &func, void *send, size_t slen, void *recv, size_t *rlen, struct timeval *tv = NULL) = 0;
    /**
     * Start run ERPC framework.\n
     * After you have registered all service, and created all observer, you should call start(),\n
     * then run your own business threads.At last, call runUntilAskedToQuit() switch into abnormal\n
     * monitoring.
     * @return 
     * -  0 : start ERPC framework OK.
     * - -1 : start ERPC framework failed!
     */
    virtual int start(void) = 0;
    /**
     * This is the last function you will call in the main() program.\n
     * This is a wile loop for monitoring program exception.
     * @param[in] state  If start() calling OK, and you business threads runing OK, pass true;\n
     *      Any other situation, please pass false to stop ERPC framework.
     * @return 
     * - -1 : Mean some exception has been occured.
     * -  0 : The ERPC framework has been normal exited.\n
     *        This will happened by you press "Ctrl + C" or kill -2 to the process.
     * @note  This function will not take the initiative to quit.
     */
    virtual int runUntilAskedToQuit(bool state) = 0;

public: // observer function
    /**
     * Apply ERPC system create a observer implement.
     * @param[in] observer  The observer implement of name you want to create.
     * @return
     * -  0 : create observer implement ok.
     * - -1 : create observer implement failed.
     */
    virtual int createObserver(const string &observer) = 0;
    /**
     * Apply ERPC system destroy a observer implement.
     * @param[in] observer  The observer implement of name you want to destroy.
     * @return
     * -  0 : destroy observer implement ok.
     * - -1 : destroy observer implement failed.
     */
    virtual int destroyObserver(const string &observer) = 0;
    /**
     * When observer implement status change, just call invokeObserver(), and all observers\n 
     * will be received the data.
     * @param[in] observer The observer implement name when you call createObserver().
     * @param[in] data   The data you want to send to observer if needed.
     * @param[in] len    The length of the data send to observer.
     * @return
     * -  0 : invoke observers OK.
     * - -1 : invoke observers failed!
     */
    virtual int invokeObserver(const string &observer, void *data, size_t len) = 0;
    /**
     * Register a observer to module observer implement.
     * @param[in] module Which module the observer implement in.
     * @param[in] observer  Which observer you want to register for.
     * @param[in] func   The call back function pointer on observer implement status changed.
     * @param[in] tv   The time you want to wait until register return.\n
     * @return
     * -  0 : register observer OK.
     * - -1 : register observer failed.
     */
    virtual int registerObserver(const string &module, const string &observer, ObserverHandler func, struct timeval *tv = NULL) = 0;
    /**
     * Unregister a observer to module observer implement.
     * @param[in] module Which module the observer implement in.
     * @param[in] observer  Which observer you want to unregister for.
     * @param[in] tv   The time you want to wait until register return.\n
     * @return
     * -  0 : unregister observer OK.
     * - -1 : unregister observer failed.
     */
    virtual int unregisterObserver(const string &module, const string &observer, struct timeval *tv = NULL) = 0;

private:
    /**
     * The static pointer of ERPC.\n
     * Used together with getInstance().\n
     * On the first in process, it will create a ERPC class;But the following calling getInstance(),\n
     * it just return the pointer of ERPC.
     */
    static ERPC *m_erpc;
};

#endif // RPC_H__

