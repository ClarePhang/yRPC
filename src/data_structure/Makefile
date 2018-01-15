
HCC += arm-none-linux-gnueabi-g++
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9

all:
	g++ uint_hash.cpp -c -o uint_hash.o -fPIC -Wall
	g++ string_hash.cpp -c -o string_hash.o -fPIC -Wall

arm:
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	$(HCC) $(HCFLAGS) uint_hash.cpp -c -o uint_hash.o -fPIC -Wall
	$(HCC) $(HCFLAGS) string_hash.cpp -c -o string_hash.o -fPIC -Wall

clean:
	rm *.o
