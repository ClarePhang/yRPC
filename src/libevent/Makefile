
INC += -I./include

HCC += arm-none-linux-gnueabi-g++
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9

all:
	g++ socket_base.cpp -c -o socket_base.o -fPIC -Wall
	g++ comm_driver.cpp -c -o comm_driver.o $(INC) -fPIC -Wall

arm:
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	$(HCC) $(HCFLAGS) socket_base.cpp -shared -fPIC -c -o socket_base.o -Wall
	$(HCC) $(HCFLAGS) comm_driver.cpp -shared -fPIC -c -o comm_driver.o $(INC) -Wall
	

clean:
	rm *.o
