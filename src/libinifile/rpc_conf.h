/* rpc_conf.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2018-01-16
 * Author: Konishi
 * Email : konishi5202@163.com
 */

#ifndef RPC_CONF_H__
#define RPC_CONF_H__
#include <string>
#include <vector>

using namespace std;
#define Interface class

typedef struct {
    int default_fix_threads;
    int default_dyn_threads;
    int default_max_workqueue;
    int default_cycle_timeout;
    int default_connect_timeout;
    int default_interactive_timeout;
    bool default_timeout_en;
    bool default_cycle_check_en;
}DefaultGlobalConfig;

typedef struct {
    int fix_threads;
    int dyn_threads;
    int max_workqueue;
    int cycle_timeout;
    int connect_timeout;
    int interactive_timeout;
    bool timeout_en;
    bool cycle_check_en;
    string ip_address;
    unsigned int listen_port;
}ProcessConfig;

Interface RPCConfig
{
public:
    static RPCConfig *create(void);
    static RPCConfig *getInstance(void);
    
public:
    virtual int setConfigProfile(const string &path) = 0;
    virtual int selfCheckValidity(void) = 0;
    virtual int getProcessFromModule(string &process, const string &module) = 0;
    virtual int getProcessConfig(const string &process, ProcessConfig &process_config) = 0;
    virtual int getProcessNetConfig(const string &process, ProcessConfig &process_config) = 0;
    virtual int getDefaultConfiguration(DefaultGlobalConfig &default_config) = 0;
    virtual const string getRPCVersion(void) = 0;
    virtual int setRPCVersion(const string &version) = 0;

public:
    RPCConfig();
    virtual ~RPCConfig();

private:
    static RPCConfig *m_rpc_config_ptr;
};

#endif // RPC_CONF_H__

