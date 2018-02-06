
INC += -I../src/

LIB += -lpthread -lrt
LIB += -L../src -lrpc

HCC += arm-none-linux-gnueabi-g++
HAR += arm-none-linux-gnueabi-ar
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9

all:
	g++ MediaModule.cpp -c -o MediaModule.o -shared -fPIC $(INC) -Wall
	g++ MediaModuleBase.cpp -c -o MediaModuleBase.o -shared -fPIC $(INC) -Wall
	ar -crv libMediaModule.a *.o
	#g++ -shared -fPIC -o libMediaModule.so *.o
	g++ app.cpp -o app $(INC) $(LIB) -L./ -lMediaModule -Wall
	g++ service.cpp  -o service $(INC) $(LIB) -L./ -lMediaModule -Wall

imx6:
	$(HCC) $(HCFLAGS) MediaModule.cpp -c -o MediaModule.o -shared -fPIC $(INC) -Wall
	$(HCC) $(HCFLAGS) MediaModuleBase.cpp -c -o MediaModuleBase.o -shared -fPIC $(INC) -Wall
	$(HAR) -crv libMediaModule.a *.o
	#$(HCC) $(HCFLAGS) -shared -fPIC -o libMediaModule.so *.o
	$(HCC) $(HCFLAGS) app.cpp -o app $(INC) $(LIB) -L./ -lMediaModule -Wall
	$(HCC) $(HCFLAGS) service.cpp -o service $(INC) $(LIB) -L./ -lMediaModule -Wall

ACC += arm-linux-g++
AAR += arm-linux-ar
arm:
	$(ACC) MediaModule.cpp -c -o MediaModule.o -shared -fPIC $(INC) -Wall
	$(ACC) MediaModuleBase.cpp -c -o MediaModuleBase.o -shared -fPIC $(INC) -Wall
	$(AAR) -crv libMediaModule.a *.o
	#$(ACC) -shared -fPIC -o libMediaModule.so *.o
	$(ACC) app.cpp -o app $(INC) $(LIB) -L./ -lMediaModule -Wall
	$(ACC) service.cpp  -o service $(INC) $(LIB) -L./ -lMediaModule -Wall

clean:
	rm app service *.o *.a

