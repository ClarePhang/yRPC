
INC += -I../src/
INC += -I../src/message/
INC += -I../src/data_structure/
INC += -I../src/libthreadpool/include/
INC += -I../src/libevent/
INC += -I../src/libevent/include/
INC += -I../src/libinifile/
INC += -I../src/libinifile/include/

LIB += -lpthread
LIB += -L../src/ -lrpc

HCC += arm-none-linux-gnueabi-g++
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9

all:
	g++ client.cpp app.cpp -o client $(INC) $(LIB) -Wall
	g++ server.cpp MediaModule.cpp BTModule.cpp -o server $(INC) $(LIB) -Wall

arm:
	$(HCC) $(HCFLAGS) client.cpp app.cpp -o client $(INC) $(LIB) -Wall
	$(HCC) $(HCFLAGS) server.cpp MediaModule.cpp BTModule.cpp -o server $(INC) $(LIB) -Wall

clean:
	rm client server

