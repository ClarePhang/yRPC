
INC += -I./include

HCC += arm-none-linux-gnueabi-g++
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9

all:
	g++ socketbase.c -c -o socketbase.o -fPIC -Wall
	g++ comm_driver.c -c -o comm_driver.o $(INC) -fPIC -Wall

arm:
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	$(HCC) $(HCFLAGS) socketbase.c -shared -fPIC -c -o socketbase.o -Wall
	$(HCC) $(HCFLAGS) rpc_comm.c -shared -fPIC -c -o rpc_comm.o $(INC) -Wall
	

clean:
	rm *.o
