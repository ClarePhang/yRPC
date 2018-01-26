/* rpc_conf.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2018-01-16
 * Author: Konishi
 * Email : konishi5202@163.com
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cctype>

#include "rpc_conf_base.h"

#define K_DEBUG   printf
#define K_INFO    printf
#define K_WARN    printf
#define K_ERROR   printf

#define USING_DEBUG
//#undef USING_DEBUG

#define RPCCONFIG_PATH_ENV  "RPCCONFIG_PATH"
#define RPC_SELF_CHECK_ENV  "RPCCONFIG_CHECK"

#define GLOBALSECTION           "Global"
#define DEFAULTFIXTHREADSKEY    "DefaultFixThreads"
#define DEFAULTDYNTHREADSKEY    "DefaultDynThreads"
#define DEFAULTMAXWORKQUEUEKEY  "DefaultMaxWorkQueue"
#define DEFAULTCYCLETIMEOUTKEY  "DefaultCycleTimeout"
#define DEFAULTCONNECTTIMEOUTKEY        "DefaultConnectTimeout"
#define DEFAULTINTERACTIVETIMEOUTKEY    "DefaultInteractiveTimeout"
#define DEFAULTTIMEOUTENKEY     "DefaultTimeoutEn"
#define DEFAULTCYCLECHECKENKEY  "DefaultCycleCheckEn"
#define RPCVERSIONKEY           "RPCVersion"

#define FIXTHREADSKEY    "FixThreads"
#define DYNTHREADSKEY    "DynThreads"
#define MAXWORKQUEUEKEY  "MaxWorkQueue"
#define CYCLETIMEOUTKEY  "CycleTimeout"
#define CONNECTTIMEOUTKEY        "ConnectTimeout"
#define INTERACTIVETIMEOUTKEY    "InteractiveTimeout"
#define TIMEOUTENKEY     "TimeoutEn"
#define CYCLECHECKENKEY  "CycleCheckEn"

#define IPADDRESSKEY    "IPAddress"
#define LISTENPORTKEY   "ListenPort"
#define MODULESKEY      "Modules"

RPCConfigBase::RPCConfigBase()
{
    m_env_file.clear();
    m_conf_file.clear();
    m_rpc_version.clear();
    m_process_last.clear();
    m_get_default_conf_flag = false;
    m_load_state = CONFIG_LOAD_INIT;
    m_check_state = CONFIG_CHECK_INIT;
}

RPCConfigBase::~RPCConfigBase()
{
    m_env_file.clear();
    m_conf_file.clear();
    m_rpc_version.clear();
    m_process_last.clear();
    m_get_default_conf_flag = false;
    m_load_state = CONFIG_LOAD_INIT;
    m_check_state = CONFIG_CHECK_INIT;
}

void RPCConfigBase::printSection(void)
{
    IniFile::iterator it;

    if(CONFIG_LOAD_SUCCESS != m_load_state)
    {
        K_ERROR("RPCConfig : you have does't load a config file!\n");
        return ;
    }
    #ifdef USING_DEBUG
    K_INFO("RPCConfig : Section contains\n");
    for(it = m_inifile.begin(), it++; it != m_inifile.end(); ++it)
        K_INFO("          %s\n", it->first.c_str());
    #else
    for(it = m_inifile.begin(); it != m_inifile.end(); ++it)
    {
        if(0 != it->first.length())
            K_INFO("           %s\n", it->first.c_str());
    }
    #endif
    K_INFO("\n");
}

void RPCConfigBase::printConfig(const string &section)
{
    IniSection::iterator it;
    IniSection *section_ptr = NULL;

    if(CONFIG_LOAD_SUCCESS != m_load_state)
    {
        K_ERROR("RPCConfig : you have does't load a config file!\n");
        return ;
    }
    
    section_ptr = m_inifile.getSection(section);
    if(NULL == section_ptr)
    {
        K_ERROR("RPCConfig : does't not have %s section config, please check!\n", section.c_str());
        return ;
    }

    K_INFO("RPCConfig : %s configs\n", section.c_str());
    for(it = section_ptr->begin(); it != section_ptr->end();++it)
        K_INFO("          %s=%s\n", it->key.c_str(), it->value.c_str());
    K_INFO("\n");
}

void RPCConfigBase::printAllConfig(void)
{
    IniFile::iterator it_s;  // it section
    IniSection::iterator it_c;  // it config of one

    if(CONFIG_LOAD_SUCCESS != m_load_state)
    {
        K_ERROR("RPCConfig : you have does't load a config file!\n");
        return ;
    }
    
    K_INFO("RPCConfig : Configurations\n");
    for(it_s = m_inifile.begin(), it_s++;it_s != m_inifile.end(); ++it_s)
    {
        //K_INFO("%s\n", it_s->second->name.c_str());
        K_INFO("          [%s]\n",it_s->first.c_str());
        for(it_c = it_s->second->items.begin();it_c != it_s->second->items.end(); ++it_c)
            K_INFO("            %s=%s\n", it_c->key.c_str(), it_c->value.c_str());
        K_INFO("\n");
    }
}

void RPCConfigBase::printAllConfigWithComment(void)
{
    IniFile::iterator it_s;  // it section
    IniSection::iterator it_c;  // it config of one

    if(CONFIG_LOAD_SUCCESS != m_load_state)
    {
        K_ERROR("RPCConfig : you have does't load a config file!\n");
        return ;
    }
    
    K_INFO("RPCConfig : Configurations with comments\n");
    for(it_s = m_inifile.begin(), it_s++;it_s != m_inifile.end(); ++it_s)
    {
        if(0 != it_s->second->comment.length())
            K_INFO("%s\n", it_s->second->comment.c_str());
        //K_INFO("%s\n", it_s->second->name.c_str());
        K_INFO("          [%s]\n", it_s->first.c_str());
        for(it_c = it_s->second->items.begin(); it_c != it_s->second->items.end(); ++it_c)
        {
            if(0 != it_c->comment.length())
                K_INFO("%s\n", it_c->comment.c_str());
            K_INFO("            %s=%s\n", it_c->key.c_str(), it_c->value.c_str());
        }
        K_INFO("\n");
    }
}

void RPCConfigBase::printDefaultConfig(void)
{
    if(false == m_get_default_conf_flag)
    {
        if(getDefaultConfiguration() < 0)
            return ;
    }
    
    K_DEBUG("          [Default configurations]:\n");
    K_DEBUG("          %s = %d\n", DEFAULTFIXTHREADSKEY, m_default_config.default_fix_threads);
    K_DEBUG("          %s = %d\n", DEFAULTDYNTHREADSKEY, m_default_config.default_dyn_threads);
    K_DEBUG("          %s = %d\n", DEFAULTMAXWORKQUEUEKEY, m_default_config.default_max_workqueue);
    K_DEBUG("          %s = %d ms\n", DEFAULTCYCLETIMEOUTKEY, m_default_config.default_cycle_timeout);
    K_DEBUG("          %s = %d ms\n", DEFAULTCONNECTTIMEOUTKEY, m_default_config.default_connect_timeout);
    K_DEBUG("          %s = %d ms\n", DEFAULTINTERACTIVETIMEOUTKEY, m_default_config.default_interactive_timeout);
    if(m_default_config.default_timeout_en)
        K_DEBUG("          %s = true\n", DEFAULTTIMEOUTENKEY);
    else
        K_DEBUG("          %s = false\n", DEFAULTTIMEOUTENKEY);
    if(m_default_config.default_cycle_check_en)
        K_DEBUG("          %s = true\n", DEFAULTCYCLECHECKENKEY);
    else
        K_DEBUG("          %s = false\n", DEFAULTCYCLECHECKENKEY);
    K_DEBUG("          %s = %s\n\n", RPCVERSIONKEY, m_rpc_version.c_str());
}

void RPCConfigBase::printProcessConfig(const string &process)
{
    ProcessConfig process_config;

    if(CONFIG_LOAD_SUCCESS != m_load_state)
    {
        K_ERROR("RPCConfig : you have does't load a config file!\n");
        return ;
    }
    
    if(m_process_last == process)
    {
        process_config = m_process_config_last;
    }
    else
    {
        if(getProcessConfig(process, process_config) < 0)
            return ;
    }
    
    K_DEBUG("          [%s configurations]:\n", process.c_str());
    K_DEBUG("          %s = %d\n", FIXTHREADSKEY, process_config.fix_threads);
    K_DEBUG("          %s = %d\n", DYNTHREADSKEY, process_config.dyn_threads);
    K_DEBUG("          %s = %d\n", MAXWORKQUEUEKEY, process_config.max_workqueue);
    K_DEBUG("          %s = %d ms\n", CYCLETIMEOUTKEY, process_config.cycle_timeout);
    K_DEBUG("          %s = %d ms\n", CONNECTTIMEOUTKEY, process_config.connect_timeout);
    K_DEBUG("          %s = %d ms\n", INTERACTIVETIMEOUTKEY, process_config.interactive_timeout);
    if(process_config.timeout_en)
        K_DEBUG("          %s = true\n", TIMEOUTENKEY);
    else
        K_DEBUG("          %s = false\n", TIMEOUTENKEY);
    if(process_config.cycle_check_en)
        K_DEBUG("          %s = true\n", CYCLECHECKENKEY);
    else
        K_DEBUG("          %s = false\n", CYCLECHECKENKEY);
    K_DEBUG("          %s = %s\n", IPADDRESSKEY, process_config.ip_address.c_str());
    K_DEBUG("          %s = %d\n", LISTENPORTKEY, process_config.listen_port);

    K_DEBUG("          Modules:\n");
    for(unsigned int i = 0; i < process_config.modules.size(); i++)
        K_DEBUG("           [%02d], %s\n", i, process_config.modules[i].c_str());
    K_DEBUG("\n");
}

int RPCConfigBase::setConfigProfile(const string &path)
{
    int ret = -1;
    string tmp_path;
    char *self_check_env = NULL;

    tmp_path.clear();
    if(path.empty())
    {
        ret = getConfigProfileFromEnv();
        if(ret < 0)
        {
            m_load_state = CONFIG_LOAD_FAILED;
            K_ERROR("RPCConfig : config-file path can not be NULL!\n");
            return -1;
        }
        tmp_path = m_env_file;
        K_WARN("RPCConfig : config-file path is NULL, use environment config.\n");
    }
    else
    {
        tmp_path = path;
    }
    
    if(CONFIG_LOAD_SUCCESS == m_load_state)
    {
        K_WARN("RPCConfig : you have loaded %s configuration file before.\n", m_conf_file.c_str());
        if(tmp_path == m_conf_file)
            return 0;
        K_WARN("RPCConfig : now will load %s file.\n", tmp_path.c_str());
    }
    else if((CONFIG_LOAD_FAILED == m_load_state) && (tmp_path == m_conf_file))
    {
        K_WARN("RPCConfig : loaded failed, you have loaded %s file before.\n", tmp_path.c_str());
        return -1;
    }
    
    m_conf_file.clear();
    m_conf_file = tmp_path;
    
    m_check_state = CONFIG_CHECK_INIT;
    ret = m_inifile.load(m_conf_file);
    if(0 != ret)
    {
        m_load_state = CONFIG_LOAD_FAILED;
        K_ERROR("RPCConfig : load %s config-file failed!\n", m_conf_file.c_str());
        return -1;
    }

    m_load_state = CONFIG_LOAD_SUCCESS;
    K_INFO("RPCConfig : load %s config-file OK.\n", m_conf_file.c_str());

    self_check_env = getenv(RPC_SELF_CHECK_ENV);
    if((self_check_env != NULL) && ((0 == strncmp(self_check_env, "true", 4)) || (0 == strncmp(self_check_env, "TRUE", 4))))
    {
        ret = this->selfCheckValidity();
        if(ret < 0)
        {
            return -1;
        }
    }
    
    return 0;
}

int RPCConfigBase::selfCheckValidity(void)
{
    IniFile::iterator it;

    if(CONFIG_LOAD_SUCCESS != m_load_state)
    {
        K_ERROR("RPCConfig : you have does't load a config file!\n");
        return -1;
    }
    
    if(CONFIG_CHECK_SUCCESS == m_check_state)
    {
        K_INFO("RPCConfig : you have already self-checked, it's OK.\n");
        return 0;
    }

    K_INFO("RPCConfig : start self-check ...\n");
    
    // check Global section
    if(false == m_inifile.hasSection(GLOBALSECTION))
    {
        K_ERROR("RPCConfig : does't not have %s section config, please check!\n", GLOBALSECTION);
        goto CHECK_FAILED;
    }
    if(false == m_inifile.hasKey(GLOBALSECTION, DEFAULTFIXTHREADSKEY))
    {
        K_ERROR("RPCConfig : does't not have %s key config, please check!\n", DEFAULTFIXTHREADSKEY);
        goto CHECK_FAILED;
    }
    if(false == m_inifile.hasKey(GLOBALSECTION, DEFAULTDYNTHREADSKEY))
    {
        K_ERROR("RPCConfig : does't not have %s key config, please check!\n", DEFAULTDYNTHREADSKEY);
        goto CHECK_FAILED;
    }
    if(false == m_inifile.hasKey(GLOBALSECTION, DEFAULTMAXWORKQUEUEKEY))
    {
        K_ERROR("RPCConfig : does't not have %s key config, please check!\n", DEFAULTMAXWORKQUEUEKEY);
        goto CHECK_FAILED;
    }
    if(false == m_inifile.hasKey(GLOBALSECTION, DEFAULTCYCLETIMEOUTKEY))
    {
        K_ERROR("RPCConfig : does't not have %s key config, please check!\n", DEFAULTCYCLETIMEOUTKEY);
        goto CHECK_FAILED;
    }
    if(false == m_inifile.hasKey(GLOBALSECTION, DEFAULTCONNECTTIMEOUTKEY))
    {
        K_ERROR("RPCConfig : does't not have %s key config, please check!\n", DEFAULTCONNECTTIMEOUTKEY);
        goto CHECK_FAILED;
    }
    if(false == m_inifile.hasKey(GLOBALSECTION, DEFAULTINTERACTIVETIMEOUTKEY))
    {
        K_ERROR("RPCConfig : does't not have %s key config, please check!\n", DEFAULTINTERACTIVETIMEOUTKEY);
        goto CHECK_FAILED;
    }
    if(false == m_inifile.hasKey(GLOBALSECTION, DEFAULTTIMEOUTENKEY))
    {
        K_ERROR("RPCConfig : does't not have %s key config, please check!\n", DEFAULTTIMEOUTENKEY);
        goto CHECK_FAILED;
    }
    if(false == m_inifile.hasKey(GLOBALSECTION, DEFAULTCYCLECHECKENKEY))
    {
        K_ERROR("RPCConfig : does't not have %s key config, please check!\n", DEFAULTCYCLECHECKENKEY);
        goto CHECK_FAILED;
    }

    // check process section
    for(it = m_inifile.begin(), it++; it != m_inifile.end(); ++it)
    {
        if(it->first == GLOBALSECTION)
            continue;
        
        if(false == m_inifile.hasKey(it->first, IPADDRESSKEY))
        {
            K_ERROR("RPCConfig : %s does't not have %s key config, please check!\n", it->first.c_str(), IPADDRESSKEY);
            goto CHECK_FAILED;
        }
        if(false == m_inifile.hasKey(it->first, LISTENPORTKEY))
        {
            K_ERROR("RPCConfig : %s does't not have %s key config, please check!\n", it->first.c_str(), LISTENPORTKEY);
            goto CHECK_FAILED;
        }
        if(false == m_inifile.hasKey(it->first, MODULESKEY))
        {
            K_ERROR("RPCConfig : %s does't not have %s key config, please check!\n", it->first.c_str(), MODULESKEY);
            goto CHECK_FAILED;
        }
    }
    
    m_check_state = CONFIG_CHECK_SUCCESS;
    K_INFO("RPCConfig : self-check PASS ^_^\n");
    return 0;

CHECK_FAILED:
    m_check_state = CONFIG_CHECK_FAILED;
    K_ERROR("RPCConfig : self-check FAIL T_T!\n");
    return -1;
}

int RPCConfigBase::getProcessFromModule(string &process, const string &module)
{
    int result = -1;
    IniFile::iterator it;
    vector<string> modules;
    vector<string>::iterator m_it;

    if(CONFIG_LOAD_SUCCESS != m_load_state)
    {
        K_ERROR("RPCConfig : you have does't load a config file!\n");
        return -1;
    }

    for(it = m_inifile.begin(), it++; it != m_inifile.end(); ++it)
    {
        if(string(GLOBALSECTION) ==  it->first)
            continue;
        result = m_inifile.getValues(it->first, MODULESKEY, modules);
        if(result < 0)
            return -1;
        for(m_it = modules.begin(); m_it != modules.end(); ++m_it)
        {
            if(module == *m_it)
            {
                process = it->first;
                return 0;
            }
        }
    }
    if(it == m_inifile.end())
        return -1;  // not exsit
    
    return 0;
}

int RPCConfigBase::getProcessConfig(const string &process, ProcessConfig &process_config)
{
    int ret = -1;
    string cycle_timeout_str;
    string connect_timeout_str;
    string interactive_timeout_str;
    
    if(CONFIG_LOAD_SUCCESS != m_load_state)
    {
        K_ERROR("RPCConfig : you have does't load a config file!\n");
        return -1;
    }

    if(m_process_last == process)
    {
        //process_config = m_process_config_last;
        process_config.fix_threads = m_process_config_last.fix_threads;
        process_config.dyn_threads = m_process_config_last.dyn_threads;
        process_config.max_workqueue = m_process_config_last.max_workqueue;
        process_config.cycle_timeout = m_process_config_last.cycle_timeout;
        process_config.connect_timeout = m_process_config_last.connect_timeout;
        process_config.interactive_timeout = m_process_config_last.interactive_timeout;
        process_config.timeout_en = m_process_config_last.timeout_en;
        process_config.cycle_check_en = m_process_config_last.cycle_check_en;
        process_config.ip_address = m_process_config_last.ip_address;
        process_config.listen_port = m_process_config_last.listen_port;
        process_config.modules = m_process_config_last.modules;
        return 0;
    }
    
    cycle_timeout_str.clear();
    connect_timeout_str.clear();
    interactive_timeout_str.clear();

    if((false == m_get_default_conf_flag) && (getDefaultConfiguration() < 0))
        return -1;
    
    // get general config
    process_config.fix_threads = m_inifile.getIntValue(process, FIXTHREADSKEY, ret);
    if(ret < 0)
        process_config.fix_threads = m_default_config.default_fix_threads;
    process_config.dyn_threads = m_inifile.getIntValue(process, DYNTHREADSKEY, ret);
    if(ret < 0)
        process_config.dyn_threads = m_default_config.default_dyn_threads;
    process_config.max_workqueue = m_inifile.getIntValue(process, MAXWORKQUEUEKEY, ret);
    if(ret < 0)
        process_config.max_workqueue = m_default_config.default_max_workqueue;
    cycle_timeout_str = m_inifile.getStringValue(process, CYCLETIMEOUTKEY, ret);
    if(ret < 0)
        process_config.cycle_timeout = m_default_config.default_cycle_timeout;
    else
        process_config.cycle_timeout = atoi(cycle_timeout_str.c_str()) * getTimeUnitConversion(cycle_timeout_str);
    connect_timeout_str = m_inifile.getStringValue(process, CONNECTTIMEOUTKEY, ret);
    if(ret < 0)
        process_config.connect_timeout = m_default_config.default_connect_timeout;
    else
        process_config.connect_timeout = atoi(connect_timeout_str.c_str()) * getTimeUnitConversion(connect_timeout_str);
    interactive_timeout_str = m_inifile.getStringValue(process, INTERACTIVETIMEOUTKEY, ret);
    if(ret < 0)
        process_config.interactive_timeout = m_default_config.default_interactive_timeout;
    else
        process_config.interactive_timeout = atoi(interactive_timeout_str.c_str()) * getTimeUnitConversion(interactive_timeout_str);
    process_config.timeout_en = m_inifile.getBoolValue(process, TIMEOUTENKEY, ret);
    if(ret < 0)
        process_config.timeout_en = m_default_config.default_timeout_en;
    process_config.cycle_check_en = m_inifile.getBoolValue(process, CYCLECHECKENKEY, ret);
    if(ret < 0)
        process_config.cycle_check_en = m_default_config.default_cycle_check_en;

    ret = getProcessNetConfig(process, process_config);  // get net config
    if(ret == 0)
    {
        ret = getProcessModulesConfig(process, process_config);  // get modules config
    }

    if(ret == 0)
    {
        m_process_last = process;
        //m_process_config_last = process_config;
        m_process_config_last.fix_threads = process_config.fix_threads;
        m_process_config_last.dyn_threads = process_config.dyn_threads;
        m_process_config_last.max_workqueue = process_config.max_workqueue;
        m_process_config_last.cycle_timeout = process_config.cycle_timeout;
        m_process_config_last.connect_timeout = process_config.connect_timeout;
        m_process_config_last.interactive_timeout = process_config.interactive_timeout;
        m_process_config_last.timeout_en = process_config.timeout_en;
        m_process_config_last.cycle_check_en = process_config.cycle_check_en;
        m_process_config_last.ip_address = process_config.ip_address;
        m_process_config_last.listen_port = process_config.listen_port;
        m_process_config_last.modules = process_config.modules;
    }
    
    #ifdef USING_DEBUG // just for Debug
    if(0 == ret)
        printProcessConfig(process);
    #endif
    
    return ret;
}

int RPCConfigBase::getProcessNetConfig(const string &process, ProcessConfig &process_config)
{
    int ret = -1;
    
    if(CONFIG_LOAD_SUCCESS != m_load_state)
    {
        K_ERROR("RPCConfig : you have does't load a config file!\n");
        return -1;
    }

    process_config.ip_address = m_inifile.getStringValue(process, IPADDRESSKEY, ret);
    if(ret < 0)
    {
        K_ERROR("RPCConfig : get %s:%s config failed, please check!\n", process.c_str(), IPADDRESSKEY);
        return -1;
    }
    process_config.listen_port = m_inifile.getIntValue(process, LISTENPORTKEY, ret);
    if(ret < 0)
    {
        K_ERROR("RPCConfig : get %s:%s config failed, please check!\n", process.c_str(), LISTENPORTKEY);
        return -1;
    }
    
    return 0;
}

int RPCConfigBase::getProcessModulesConfig(const string &process, ProcessConfig &process_config)
{
    int ret = -1;
    
    if(CONFIG_LOAD_SUCCESS != m_load_state)
    {
        K_ERROR("RPCConfig : you have does't load a config file!\n");
        return -1;
    }

    process_config.modules.clear();
    ret = m_inifile.getValues(process, MODULESKEY, process_config.modules);
    if(ret < 0)
        K_ERROR("RPCConfig : get %s:%s config failed, please check!\n", process.c_str(), MODULESKEY);
    
    return ret;
}

int RPCConfigBase::getDefaultConfiguration(void)
{
    int ret = -1;
    string cycle_timeout_str;
    string connect_timeout_str;
    string interactive_timeout_str;
    
    if(CONFIG_LOAD_SUCCESS != m_load_state)
    {
        K_ERROR("RPCConfig : you have does't load a config file!\n");
        return -1;
    }

    cycle_timeout_str.clear();
    connect_timeout_str.clear();
    interactive_timeout_str.clear();
    
    m_default_config.default_fix_threads = m_inifile.getIntValue(GLOBALSECTION, DEFAULTFIXTHREADSKEY, ret);
    if(ret < 0)
    {
        K_ERROR("RPCConfig : get %s config failed, please check!\n", DEFAULTFIXTHREADSKEY);
        return -1;
    }
    m_default_config.default_dyn_threads = m_inifile.getIntValue(GLOBALSECTION, DEFAULTDYNTHREADSKEY, ret);
    if(ret < 0)
    {
        K_ERROR("RPCConfig : get %s config failed, please check!\n", DEFAULTDYNTHREADSKEY);
        return -1;
    }
    m_default_config.default_max_workqueue = m_inifile.getIntValue(GLOBALSECTION, DEFAULTMAXWORKQUEUEKEY, ret);
    if(ret < 0)
    {
        K_ERROR("RPCConfig : get %s config failed, please check!\n", DEFAULTMAXWORKQUEUEKEY);
        return -1;
    }
    cycle_timeout_str = m_inifile.getStringValue(GLOBALSECTION, DEFAULTCYCLETIMEOUTKEY, ret);
    if(ret < 0)
    {
        K_ERROR("RPCConfig : get %s config failed, please check!\n", DEFAULTCYCLETIMEOUTKEY);
        return -1;
    }
    m_default_config.default_cycle_timeout = atoi(cycle_timeout_str.c_str()) * getTimeUnitConversion(cycle_timeout_str);
    connect_timeout_str = m_inifile.getStringValue(GLOBALSECTION, DEFAULTCONNECTTIMEOUTKEY, ret);
    if(ret < 0)
    {
        K_ERROR("RPCConfig : get %s config failed, please check!\n", DEFAULTCONNECTTIMEOUTKEY);
        return -1;
    }
    m_default_config.default_connect_timeout = atoi(connect_timeout_str.c_str()) * getTimeUnitConversion(connect_timeout_str);
    interactive_timeout_str = m_inifile.getStringValue(GLOBALSECTION, DEFAULTINTERACTIVETIMEOUTKEY, ret);
    if(ret < 0)
    {
        K_ERROR("RPCConfig : get %s config failed, please check!\n", DEFAULTINTERACTIVETIMEOUTKEY);
        return -1;
    }
    m_default_config.default_interactive_timeout = atoi(interactive_timeout_str.c_str()) * getTimeUnitConversion(interactive_timeout_str);
    m_default_config.default_timeout_en = m_inifile.getBoolValue(GLOBALSECTION, DEFAULTTIMEOUTENKEY, ret);
    if(ret < 0)
    {
        K_ERROR("RPCConfig : get %s config failed, please check!\n", DEFAULTTIMEOUTENKEY);
        return -1;
    }
    m_default_config.default_cycle_check_en = m_inifile.getBoolValue(GLOBALSECTION, DEFAULTCYCLECHECKENKEY, ret);
    if(ret < 0)
    {
        K_ERROR("RPCConfig : get %s config failed, please check!\n", DEFAULTCYCLECHECKENKEY);
        return -1;
    }
    m_rpc_version = m_inifile.getStringValue(GLOBALSECTION, RPCVERSIONKEY, ret);
    if(ret < 0)
        m_rpc_version = "NULL";

    m_get_default_conf_flag = true;
    
    #ifdef USING_DEBUG
    printDefaultConfig();
    #endif

    return 0;
}

int RPCConfigBase::getDefaultConfiguration(DefaultGlobalConfig &default_config)
{
    if(false == m_get_default_conf_flag)
    {
        if(getDefaultConfiguration() < 0)
            return -1;
    }

    //default_config = m_default_config;
    default_config.default_fix_threads = m_default_config.default_fix_threads;
    default_config.default_dyn_threads = m_default_config.default_dyn_threads;
    default_config.default_max_workqueue = m_default_config.default_max_workqueue;
    default_config.default_cycle_timeout = m_default_config.default_cycle_timeout;
    default_config.default_connect_timeout = m_default_config.default_connect_timeout;
    default_config.default_interactive_timeout = m_default_config.default_interactive_timeout;
    default_config.default_timeout_en = m_default_config.default_timeout_en;
    default_config.default_cycle_check_en = m_default_config.default_cycle_check_en;
    
    return 0;
}

int RPCConfigBase::getConfigProfileFromEnv(void)
{
    char *config_profile_env = NULL;

    config_profile_env = getenv(RPCCONFIG_PATH_ENV);
    if(NULL == config_profile_env)
    {
        K_ERROR("RPCConfig : Environment %s has not set in the system!\n", RPCCONFIG_PATH_ENV);
        return -1;
    }

    m_env_file.clear();
    m_env_file.append(config_profile_env);
    K_INFO("RPCConfig : RPC environment config file is %s.\n", m_conf_file.c_str());
    
    return 0;
}

int RPCConfigBase::getTimeUnitConversion(string &unit)
{
    IniFile::toLowerCase(unit);

    if(string::npos != unit.find("ms"))
        return 1;
    else if(string::npos != unit.find("s"))
        return 1000;
    else if(string::npos != unit.find("min"))
        return 60*1000;
    return 1;
}

CONFIG_LOAD_STATE RPCConfigBase::getLoadState(void)
{
    return m_load_state;
}

CONFIG_CHECK_STATE RPCConfigBase::getCheckState(void)
{
    return m_check_state;
}

const string RPCConfigBase::getRPCVersion(void)
{
    if(false == m_get_default_conf_flag)
    {
        if(getDefaultConfiguration() < 0)
            return "NULL";
    }
    
    return m_rpc_version;
}

int RPCConfigBase::setRPCVersion(const string &version)
{
    int ret = -1;
    
    if(false == m_get_default_conf_flag)
    {
        if(getDefaultConfiguration() < 0)
            return -1;
    }

    if(version != m_rpc_version)
    {
        m_rpc_version = version;
        m_inifile.setValue(GLOBALSECTION, RPCVERSIONKEY, version, "");
//            "# Do not change RPCVersion, because it will be changed by program on running.");
        ret = m_inifile.save();
    }
    
    return ret;
}

