/** 
 * @defgroup group1 GlobalConfiguration
 * @{
 *  Global default configurations.\n
 *  The section must contain all values, Because it contains all default config.
 * @param DefaultFixThreads  RPC process default fixed threads.
 * @param DefaultDynThreads  RPC process default dynamic threads. It will be automation created when the business
 *                           is busy; and will be automation destroy when the business is idle.
 * @param DefaultMaxWorkQueue  RPC process default worker queue max length.
 * @param DefaultCycleTimeout  RPC process cycle event time. The unit can be S or ms, case-insensitive. Not used now.
 * @param DefaultConnectTimeout  RPC process connect timeout time. The unit can be S or ms, case-insensitive.
 * @param DefaultInteractiveTimeout RPC processes communication timeout time. The unit can be S or ms, case-insensitive.
 * @param DefaultTimeoutEn  RPC process communicate timeout control flag.true means open, and false means close. Not used now.
 * @param DefaultCycleCheckEn  RPC cycle event control flag. true means open, and false means close. Not used now.
 * @param RPCVersion  Indicate RPC Release version, this configuration will be update by program running.
 * @par For example:
 * @code
 * [GlobalConfiguration]
 *  DefaultFixThreads=5
 *  DefaultDynThreads=8
 *  DefaultMaxWorkQueue=300
 *  DefaultCycleTimeout=15S
 *  DefaultConnectTimeout=5S
 *  DefaultInteractiveTimeout=3S
 *  DefaultTimeoutEn=false
 *  DefaultCycleCheckEn=false
 *  RPCVersion=Release-V2.0
 * @endcode
 * @}
 *
 * @defgroup grpup2 ModuleConfiguration
 * @{
 *  Modules configurations.This section config indicate which process of the 
 *  module loaded in. One process can hold one or more modules.\n
 * @par For example:
 * @code
 *  [ModuleConfiguration]
 *   BT=service
 *   Media=service
 *   APP=app
 * @endcode
 *  This means 'BT' module and 'Media' module are builded in service process, 'APP' module is builded in app process.
 * @}
 *
 * @defgroup group3 ProcessConfig
 * This section config indicate how process run. Include at least network configuration.
 * @{
 * @par The simplest example:
 * @code
 * [service]
 *  IPAddress=127.0.0.1
 *  ListenPort=8000
 * @endcode
 * This mean 'service' process will run at loop network binding 8000 port.
 *
 * You also can have you own configuration cover global default configuration.
 * Just remove 'Default' world except RPCVersion from 'GlobalConfiguration', \n
 * like this: FixThreads = 3. Then the process will run 3 fixed threads, not the
 * DefaultFixThreads which equal 5.
 * @par Cover global configuration example:
 * @code
 * [app]
 *  ConnectTimeout=10MS
 *  IPAddress=/tmp/s_app
 *  ListenPort=0
 * @endcode
 * This mean 'app' process will run at local network at /tmp/s_app file, and its 
 * time-out period of connection is 10 ms.
 *
 * Every process has 8 hidden configurations. All the configurations you can cover are :
 * FixThreads, DynThreads, MaxWorkQueue, CycleTimeout, ConnectTimeout, InteractiveTimeout,
 * TimeoutEn, CycleCheckEn.\n
 * @par The most comprehensive example:
 * @code
 * [service]
 *  FixThreads=5
 *  DynThreads=8
 *  MaxWorkQueue=300
 *  CycleTimeout=15S
 *  ConnectTimeout=5S
 *  InteractiveTimeout=3S
 *  TimeoutEn=false
 *  CycleCheckEn=false
 *  IPAddress=192.168.1.23
 *  ListenPort=8080
 * @endcode
 *
 * From the above examples, you can see : The ERPC can use loop network, local network, and TCP network.\n
 * @par Using loop network:
 * Just make IPAddress equal 127.0.0.1.
 * @code
 *  IPAddress=127.0.0.1
 *  ListenPort=8000
 * @endcode
 * @par Using local network
 * Just make ListenPort equal 0, and IPAddress equal a local file.
 * @code
 *  IPAddress=/tmp/s_app
 *  ListenPort=0
 * @endcode
 * @par Using TCP network:
 * Just make IPAddress equal INADDR_ANY, or a real IP address.
 * @code
 *  IPAddress=INADDR_ANY
 *  ListenPort=8080
 * @endcode
 * @note
 *  1.Don't let the IPAddress of different processes is equal on using local network;
 *  2.Don't let the ListenPort of different processes is equal on using loop network and TCP network.
 * @}
 */
[GlobalConfiguration]
DefaultFixThreads=5
DefaultDynThreads=8
DefaultMaxWorkQueue=300
DefaultCycleTimeout=15S
DefaultConnectTimeout=5S
DefaultInteractiveTimeout=3S
DefaultTimeoutEn=false
DefaultCycleCheckEn=false
RPCVersion=Release-V2.0

[ModuleConfiguration]
BT=service
Media=service
APP=app

[service]
IPAddress=127.0.0.1
ListenPort=8000

[app]
ConnectTimeout=8S
IPAddress=127.0.0.1
ListenPort=8989

/** @} */

