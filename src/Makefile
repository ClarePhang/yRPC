INC += -I./message/
INC += -I./libevent/
INC += -I./libevent/include/
INC += -I./libinifile/
INC += -I./libinifile/include/
LIB += -L./libevent/lib -levent -levent_pthreads -lpthread

all:
	cd ./libevent/ && make
	cd ./libinifile/ && make
	cd ./libthreadpool && make
	cd ./data_structure/ && make
	#g++ ./message/message.cpp -c -o ./message/message.o -fPIC -Wall
	#g++ ./message/bodydata.cpp -c -o ./message/bodydata.o -fPIC -Wall
	#g++ rpc_proxy.cpp -c -o rpc_proxy.o -fPIC -Wall
	#g++ rpc.cpp -c -o rpc.o $(INC) -fPIC -Wall
	#ar cr librpc.a *.o ./libevent/*.o ./libinifile/*.o ./message/*.o
	#g++ -shared -o librpc.so *.o ./libevent/*.o ./libinifile/*.o ./message/*.o
	#strip librpc.a librpc.so
	#rm *.o ./libevent/*.o ./libinifile/*.o ./message/*.o

HCC += arm-none-linux-gnueabi-g++
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9

arm:
	cd ./libevent/ && make
	cd ./libinifile/ && make
	cd ./libthreadpool && make
	cd ./data_structure/ && make
	#$(HCC) $(HCFLAGS) -shared -fPIC rpc.cpp -c -o rpc.o $(INC) -Wall
	#$(HCC) $(HCFLAGS) -shared -fPIC rpc_proxy.cpp -c -o rpc_proxy.o -Wall
	#$(HCC) $(HCFLAGS) -shared -o librpc.so *.o ./message/*.o ./libevent/*.o ./libinifile/*.o ./rpc_hash/*.o ./rpc_list/*.o
	#$(HCC) $(HCFLAGS) client.cpp app.cpp -o client $(INC) $(LIB) $(LIBA) -Wall
	#$(HCC) $(HCFLAGS) server.cpp MediaModule.cpp BTModule.cpp -o server $(INC) $(LIB) $(LIBA) -Wall

clean:
	cd ./libevent/ && make clean
	cd ./libinifile/ && make clean
	cd ./libthreadpool && make clean
	cd ./data_structure/ && make clean
	rm *.o
