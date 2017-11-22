
INC += -I./libevent/include/
LIB += -L./libevent/lib -levent -levent_pthreads -lpthread

all:
	g++ rpc_hash.cpp -c -o rpc_hash.o -fPIC -Wall
	g++ rpc_list.cpp -c -o rpc_list.o -fPIC -Wall
	g++ threadpool.cpp -c -o threadpool.o -fPIC -Wall
	g++ socketbase.cpp -c -o socketbase.o -fPIC -Wall
	g++ rpc_comm.cpp -c -o rpc_comm.o $(INC) -fPIC -Wall
	ar cr librpc.a *.o
	g++ -shared -o librpc.so *.o
	strip librpc.a librpc.so
	rm *.o
