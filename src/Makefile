INC += -I./message/
INC += -I./libevent/
INC += -I./libevent/include/
INC += -I./libinifile/
INC += -I./libinifile/include/
LIB += -L./libevent/lib -levent -levent_pthreads -lpthread

all:
	g++ int_hash.cpp -c -o int_hash.o -fPIC -Wall
	g++ rpc_hash.cpp -c -o rpc_hash.o -fPIC -Wall
	g++ rpc_list.cpp -c -o rpc_list.o -fPIC -Wall
	g++ threadpool.cpp -c -o threadpool.o -fPIC -Wall
	g++ ./message/message.cpp -c -o ./message/message.o -fPIC -Wall
	g++ ./message/bodydata.cpp -c -o ./message/bodydata.o -fPIC -Wall
	g++ ./libevent/socketbase.cpp -c -o ./libevent/socketbase.o -fPIC -Wall
	g++ ./libevent/rpc_comm.cpp -c -o ./libevent/rpc_comm.o $(INC) -fPIC -Wall
	g++ ./libinifile/module_config.cpp -c -o ./libinifile/module_config.o $(INC) -fPIC -Wall
	g++ ./libinifile/network_config.cpp -c -o ./libinifile/network_config.o $(INC) -fPIC -Wall
	g++ rpc_proxy.cpp -c -o rpc_proxy.o -fPIC -Wall
	g++ rpc.cpp -c -o rpc.o $(INC) -fPIC -Wall
	ar cr librpc.a *.o ./libevent/*.o ./libinifile/*.o ./message/*.o
	g++ -shared -o librpc.so *.o ./libevent/*.o ./libinifile/*.o ./message/*.o
	strip librpc.a librpc.so
	rm *.o ./libevent/*.o ./libinifile/*.o ./message/*.o
