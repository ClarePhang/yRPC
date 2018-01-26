
INC += -I../src/

LIB += -lpthread -lrt
LIB += -L../src -lrpc

HCC += arm-none-linux-gnueabi-g++
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9

all:
	g++ app.cpp MediaModuleInterface.cpp -o app $(INC) $(LIB) -Wall
	g++ service.cpp MediaModuleImplement.cpp -o service $(INC) $(LIB) -Wall

imx6:
	$(HCC) $(HCFLAGS) app.cpp MediaModuleInterface.cpp -o app $(INC) $(LIB) -Wall
	$(HCC) $(HCFLAGS) service.cpp MediaModuleImplement.cpp -o service $(INC) $(LIB) -Wall

ACC += arm-linux-g++

arm:
	$(ACC) app.cpp MediaModuleInterface.cpp -o app $(INC) $(LIB) -Wall
	$(ACC) service.cpp MediaModuleImplement.cpp -o service $(INC) $(LIB) -Wall

clean:
	rm app service

