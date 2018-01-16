
INC += -I./include/

HCC += arm-none-linux-gnueabi-g++
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9


all:
	g++ module_config.cpp -c -o module_config.o $(INC) -fPIC -Wall
	g++ network_config.cpp -c -o network_config.o $(INC) -fPIC -Wall

arm:
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	$(HCC) $(HCFLAGS) module_config.cpp -shared -fPIC -c -o module_config.o $(INC) -Wall
	$(HCC) $(HCFLAGS) network_config.cpp -shared -fPIC -c -o network_config.o $(INC) -Wall

clean:
	rm *.o
