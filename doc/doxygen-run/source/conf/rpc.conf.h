/**
 * Global Default configurations.\n
 * The section must contain all values, Because it contains all default config.
 */
[GlobalConfiguration]
DefaultFixThreads=5  /**< RPC process default fixed threads */
DefaultDynThreads=8  /**< RPC process default dynamic threads. It will be automation \n
                          created when the business is busy; and will be automation \n
                          destroy when the business is idle.*/
DefaultMaxWorkQueue=300  /**< RPC process default worker queue max length. */
DefaultCycleTimeout=15S  /**< RPC process cycle event time. The unit can be S or ms, case-insensitive. Not used now */
DefaultConnectTimeout=5S  /**< RPC process connect timeout time. The unit can be S or ms, case-insensitive. */
DefaultInteractiveTimeout=3S  /**< RPC processes communication timeout time. The unit can be S or ms, case-insensitive. */
DefaultTimeoutEn=false  /**< RPC process communicate timeout control flag.true means open, and false means close. Not used now. */
DefaultCycleCheckEn=false  /**< RPC cycle event control flag. true means open, and false means close. Not used now. */
RPCVersion=Release-V2.0  /**< Indicate RPC Release version, this configuration will be update by program running. */

/**
 * Modules configurations.\n
 * This section config indicate which process of the module loaded in.\n
 * On process can hold one or more modules.
 */
[ModuleConfiguration]
BTModule=service
MediaModule=service
APPView=app

/**
 * Process configurations.\n
 * This section config indicate how process run. Include at least network configuration.\n
 * You also have you own configuration cover global default configuration. Just remove 'Default' world exept \n
 * RPCVersion, like this: FixThreads = 5. Then service will run 5 fixed threads.
 */
[service]
IPAddress=127.0.0.1
#IPAddress=124.23.13.13
ListenPort=8000

/**
 * One product may have more than one process.
 */
[app]
ConnectTimeout=8S
IPAddress=127.0.0.1
ListenPort=8989