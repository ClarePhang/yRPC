
HCC += arm-none-linux-gnueabi-g++
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9

all:
	g++ message.cpp -c -o message.o -fPIC -Wall
	g++ bodydata.cpp -c -o bodydata.o -fPIC -Wall

arm:
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	$(HCC) $(HCFLAGS) message.cpp -shared -fPIC -c -o message.o -Wall
	$(HCC) $(HCFLAGS) bodydata.cpp -shared -fPIC -c -o bodydata.o -Wall

clean:
	rm *.o
