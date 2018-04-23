
INC += -I../src/

LIB += -L../src/ -lrpc
LIB += -lpthread -lrt

linux:
	g++ client.cpp module.cpp rpc_test.cpp -o test0 $(INC) $(LIBL) $(LIB) -Wall
	cp test0 test1
	cp test0 test2
	cp test0 test3
	cp test0 test4

HCC += arm-none-linux-gnueabi-g++
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9

imx6:
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	$(HCC) $(HCFLAGS) client.cpp module.cpp rpc_test.cpp -o test0 $(INC) $(LIBL) $(LIB) -Wall

ACC += arm-linux-g++

arm:
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	$(ACC) client.cpp module.cpp rpc_test.cpp -o test $(INC) $(LIBL) $(LIB) -Wall


clean:
	rm test*

