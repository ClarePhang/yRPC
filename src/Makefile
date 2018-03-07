INC += -I./message/
INC += -I./libevent/
INC += -I./libevent/include/
INC += -I./libinifile/
INC += -I./libinifile/include/
INC += -I./data_structure/
INC += -I./libthreadpool/include/

all:
	cd ./message/ && make
	cd ./libevent/ && make
	cd ./libinifile/ && make
	cd ./libthreadpool/ && make
	cd ./data_structure/ && make
	g++ rpc.cpp -c -o rpc.o $(INC) -fPIC -Wall
	g++ rpc_core.cpp -c -o rpc_core.o $(INC) -fPIC -Wall
	g++ rpc_proxy.cpp -c -o rpc_proxy.o $(INC) -fPIC -Wall
	g++ rpc_observer.cpp -c -o rpc_observer.o $(INC) -fPIC -Wall
	g++ -shared -fPIC -o librpc.so *.o ./message/*.o ./libevent/*.o ./libevent/lib_linux/*.a ./libinifile/*.o ./libinifile/lib_linux/*.a ./libthreadpool/lib_linux/*.a ./data_structure/*.o
	strip librpc.so

HCC += arm-none-linux-gnueabi-g++
STRIP += arm-none-linux-gnueabi-strip
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9

imx6:
	cd ./message/ && make imx6
	cd ./libevent/ && make imx6 
	cd ./libinifile/ && make imx6
	cd ./libthreadpool/ && make imx6
	cd ./data_structure/ && make imx6
	$(HCC) $(HCFLAGS) -fPIC rpc.cpp -c -o rpc.o $(INC) -Wall
	$(HCC) $(HCFLAGS) -fPIC rpc_core.cpp -c -o rpc_core.o $(INC) -Wall
	$(HCC) $(HCFLAGS) -fPIC rpc_proxy.cpp -c -o rpc_proxy.o $(INC) -Wall
	$(HCC) $(HCFLAGS) -fPIC rpc_observer.cpp -c -o rpc_observer.o $(INC) -Wall
	$(HCC) $(HCFLAGS) -shared -o librpc.so *.o ./message/*.o ./libevent/*.o ./libevent/lib_imx6/*.a ./libinifile/*.o ./libinifile/lib_imx6/*.a ./libthreadpool/lib_imx6/*.a ./data_structure/*.o
	$(STRIP) librpc.so

ACC += arm-linux-g++
ASTRIP += arm-linux-strip

arm:
	export PATH=/usr/arm-linux-gcc-v4.4.3/bin/:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin
	cd ./message/ && make arm
	cd ./libevent/ && make arm
	cd ./libinifile/ && make arm
	cd ./libthreadpool/ && make arm
	cd ./data_structure/ && make arm
	$(ACC) -fPIC rpc.cpp -c -o rpc.o $(INC) -Wall
	$(ACC) -fPIC rpc_core.cpp -c -o rpc_core.o $(INC) -Wall
	$(ACC) -fPIC rpc_proxy.cpp -c -o rpc_proxy.o $(INC) -Wall
	$(ACC) -fPIC rpc_observer.cpp -c -o rpc_observer.o $(INC) -Wall
	$(ACC) -shared -fPIC -o librpc.so *.o ./message/*.o ./libevent/*.o ./libevent/lib_arm/*.a ./libinifile/*.o ./libinifile/lib_arm/*.a ./libthreadpool/lib_arm/*.a ./data_structure/*.o
	$(STRIP) librpc.so

clean:
	cd ./message && make clean
	cd ./libevent/ && make clean
	cd ./libinifile/ && make clean
	cd ./libthreadpool/ && make clean
	cd ./data_structure/ && make clean
	rm *.o

distclean:
	cd ./message && make clean
	cd ./libevent/ && make clean
	cd ./libinifile/ && make clean
	cd ./libthreadpool/ && make clean
	cd ./data_structure/ && make clean
	rm *.o *.so
	
