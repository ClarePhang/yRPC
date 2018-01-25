
HCC += arm-none-linux-gnueabi-g++
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9

all:
	g++ msg.cpp -c -o msg.o -fPIC -Wall
	g++ message_head.cpp -c -o message_head.o -fPIC -Wall
	g++ body_head.cpp -c -o body_head.o -fPIC -Wall

imx6:
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	$(HCC) $(HCFLAGS) msg.cpp -c -o msg.o -fPIC -Wall
	$(HCC) $(HCFLAGS) message_head.cpp -c -o message_head.o -fPIC -Wall
	$(HCC) $(HCFLAGS) body_head.cpp -c -o body_head.o -fPIC -Wall

ACC += arm-linux-g++

arm:
	$(ACC) msg.cpp -c -o msg.o -fPIC -Wall
	$(ACC) message_head.cpp -c -o message_head.o -fPIC -Wall
	$(ACC) body_head.cpp -c -o body_head.o -fPIC -Wall

clean:
	rm *.o
