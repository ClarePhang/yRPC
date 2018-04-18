/*!
 * @brief       ERPC application interface
 * @details     An embedded Remote Procedure Call Framework.
 * @author      Konishi
 * @version     V2.3
 * @email       konishi5202@163.com
 * @date        2018-01-30
*/
#ifndef RPC_H__
#define RPC_H__
#include <string>

using namespace std;

typedef void (*ServiceHandler)(void *msg, void *data, size_t len);
typedef void (*ObserverHandler)(void *msg, void *data, size_t len);

/*!
 * @class ERPC
 * @brief ERPC interface class
 * @note The using step of ERPC is: \n
 *  - 1.getInstance() : Get ERPC instance(MUST);\n
 *  - 2.initRPC()     : Initialize ERPC system(MUST);\n
 *  - 3.registerService() : Register services to ERPC(MUST);\n
 *  - 4.createObserver()  : Create observer object ro ERPC if needed;\n
 *  - 5.registerObserver(): Register observer to ERPC if needed;\n
 *  - 6.start()       : Start ERPC framework(MUST);\n
 *  - 7.Start you own business threads;\n
 *  - 8.runUntilAskedToQuit() : Step into anomaly detection cycle(MUST);\n
 *  - 9.In your business/service function, call proxyCall()/invokeObserver()/setResponse() to deal with business.
*/
class ERPC
{
public:
    ERPC();
    virtual ~ERPC();
    
public:
    /*!
     * Get ERPC instance pointer. Every process only have one ERPC instance.\n
     * @return ERPC instance pointer:
     * - NULL mean get ERPC instance failed;
     * - not NULL mean get ERPC instance OK.
     * @note This is the first function you called before any others.\n
     * After you get the instance of ERPC, you can call initRPC() to init ERPC system.
     */
    static ERPC *getInstance(void);
    /*!
     * Start run ERPC framework.\n
     * After you have registered all service, and created all observer, you should call start(),\n
     * then run your own business threads.At last, call runUntilAskedToQuit() switch into abnormal\n
     * monitoring.
     * @return 
     * -  0 : start ERPC framework OK.
     * - -1 : start ERPC framework failed!
     * - -2 : ERPC configuration file has not been init before you start ERPC framework.
     */
    virtual int start(void) = 0;
    /*!
     * This is the last function you will call in the main() program.\n
     * This is a while loop for monitoring program exception.
     * @param[in] state  If start() calling OK, and you business threads runing OK, pass true;\n
     *      Any other situation, please pass false to stop ERPC framework.
     * @return 
     * -  0 : The ERPC framework has been normal exited.\n
     *        This will happened by you press "Ctrl + C" or "kill -2" to the process.
     * - -1 : Mean some exception has been occured.
     * - -2 : ERPC framework has not been running.
     * @note  This function will not take the initiative to quit.
     */
    virtual int runUntilAskedToQuit(bool state) = 0;
    /*!
     * Register a service function into ERPC system. The service function type must be :\n
     * void (*ServiceHandler)(void *msg, void *data, size_t len);
     * @param[in] service Service name, matching with the remote call proxyCall() param func;\n
     *                    Ensure that one process can't have the same service name, otherwise\n
     *                    the registration will fail.
     * @param[in] func   The pointer to the function of the 'service'.
     * @return 
     * -  1 : the service has been registered, func won't replace the old.
     * -  0 : register service at func ok.
     * - -1 : register service failed!
     */
    virtual int registerService(const string &service, ServiceHandler func) = 0;
    /*!
     * Unregister a service from ERPC system.
     * @param[in] service The service name you have registered into ERPC system before.
     * @return 
     * -  0 : unregister service ok.
     * - -1 : unregister service failed!
     */
    virtual int unregisterService(const string &service) = 0;
    /*!
     * Set the response data for remote calling.
     * This function will be called in service function.
     * @param[in] msg The first param of void (*ServiceHandler)(void *msg, void *data, size_t len);\n
     *                Just pass the 'msg' to setResponse() first praram directly.
     * @param[in] response_data The data you want to response remote calling.
     *                          Be sure the data has been serialization.
     * @param[in] response_len The length of response_data.
     * @note  If no data response, just set response_data = NULL and response_len = 0.
     * @return 
     * -  0 : return response data to remote call OK.
     * - -1 : return response data failed!
     * - -2 : ERPC framework has not been running.
     */
    virtual int setResponse(void *msg, void *response_data, size_t response_len) = 0;
    /*!
     * Remote calls 'func' service in 'module' module.
     * @param[in] module The module of the 'func' service be seated in.\n
     *                   The module name must match with the keywords of 
     *                   ModuleConfiguration section in rpc.conf.
     * @param[in] func  The service name you want to call. Math with the name on
     *                  calling registerService().
     * @param[in] send  The data passing to remote service.Be sure the data has been serialization.
     * @param[in] slen  The length of 'send' data.
     * @param[out] recv  The data returned from remote service by calling setResponse().
     * @param rlen     [in]  Mean the length of 'recv' buffer.\n
     *                 [out] Mean the length of receive data from service.
     * @param[in] tv  The time you want to wait for response data.\n
     * @return 
     * -  0 : remote call service function ok, and response data will be put in recv and rlen.
     * - -1 : remote call failed!
     * - -2 : ERPC framework has not been running.
     * @note  1.If you don't care about the data return from remote service, just set 
     *          recv = NULL and rlen = NULL, or recv = NULL and *rlen = 0.\n
     *        2.If you pass tv = NULL, or *tv = {0, 0}, ERPC system will use ERPC default
     *          timeout configuration.
     */
    virtual int proxyCall(const string &module, const string &func, void *send, size_t slen, void *recv, size_t *rlen, struct timeval *tv = NULL) = 0;

public: // observer function
    /*!
     * Create an observer object to ERPC system.
     * @param[in] observer  The name of observer object you want to create.
     * @return
     * -  1 : the observer has created, just use it.
     * -  0 : create observer object ok.
     * - -1 : create observer object failed.
     */
    virtual int createObserver(const string &observer) = 0;
    /*!
     * Destroy an observer object from ERPC system.
     * @param[in] observer  The name of observer object you want to destroy.
     * @return
     * -  0 : destroy observer object ok.
     * - -1 : destroy observer object failed.
     */
    virtual int destroyObserver(const string &observer) = 0;
    /*!
     * When observer object state changed, just call invokeObserver(), and all observers
     * will be received the new state.
     * @param[in] observer The observer object name when you call createObserver().
     * @param[in] data   The data you want to send to observers if needed.
     * @param[in] len    The length of the data send to observers.
     * @return
     * -  0 : invoke observers OK.
     * - -1 : invoke observers failed!
     * - -2 : ERPC framework has not been running.
     */
    virtual int invokeObserver(const string &observer, void *data, size_t len) = 0;
    /*!
     * Register a observer to 'module' observer object.
     * @param[in] module The module of the observer object be seated in.
     * @param[in] observer  The observer you want to register to.
     * @param[in] func   The call back function pointer on observer object state changed.
     * @param[in] tv   The time you want to wait until register return.\n
     * @return
     * -  0 : register observer OK.
     * - -1 : register observer failed.
     * - -2 : ERPC framework has not been running.
     */
    virtual int registerObserver(const string &module, const string &observer, ObserverHandler func, struct timeval *tv = NULL) = 0;
    /*!
     * Unregister a observer from 'module' observer object.
     * @param[in] module  The module of the observer object be seated in.
     * @param[in] observer  The observer you want to unregister from.
     * @param[in] tv   The time you want to wait until unregister return.\n
     * @return
     * -  0 : unregister observer OK.
     * - -1 : unregister observer failed.
     * - -2 : ERPC framework has not been running.
     */
    virtual int unregisterObserver(const string &module, const string &observer, struct timeval *tv = NULL) = 0;

private:
    /*!
     * The static pointer of ERPC. Used together with getInstance().\n
     * On the first in process, it will create a ERPC class;But the following calling getInstance(),\n
     * it just return the pointer of ERPC has been created.
     */
    static ERPC *m_erpc;
};

#endif // RPC_H__

