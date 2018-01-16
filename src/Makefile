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
	cd ./libthreadpool && make
	cd ./data_structure/ && make
	g++ proxy.cpp -c -o proxy.o -fPIC -Wall
	g++ rpc.cpp -c -o rpc.o $(INC) -fPIC -Wall
	g++ -shared -o librpc.so *.o ./message/*.o ./libevent/*.o ./libinifile/*.o ./libinifile/lib_linux/*.o ./libthreadpool/lib_linux/*.o ./data_structure/*.o
	strip librpc.so

HCC += arm-none-linux-gnueabi-g++
STRIP += arm-none-linux-gnueabi-strip
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9

arm:
	cd ./message/ && make arm
	cd ./libevent/ && make arm
	cd ./libinifile/ && make arm
	cd ./libthreadpool && make arm
	cd ./data_structure/ && make arm
	$(HCC) $(HCFLAGS) -fPIC proxy.cpp -c -o proxy.o -Wall
	$(HCC) $(HCFLAGS) -fPIC rpc.cpp -c -o rpc.o $(INC) -Wall
	$(HCC) $(HCFLAGS) -shared -o librpc.so *.o ./message/*.o ./libevent/*.o ./libinifile/*.o ./libinifile/lib_arm/*.o ./libthreadpool/lib_arm/*.o ./data_structure/*.o
	$(STRIP) librpc.so

clean:
	cd ./message && make clean
	cd ./libevent/ && make clean
	cd ./libinifile/ && make clean
	cd ./libthreadpool && make clean
	cd ./data_structure/ && make clean
	rm *.o
