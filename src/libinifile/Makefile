
INC += -I./include/

HCC += arm-none-linux-gnueabi-g++
HCFLAGS += -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9


all:
	g++ rpc_conf.cpp -c -o rpc_conf.o $(INC) -fPIC -Wall
	g++ rpc_conf_base.cpp -c -o rpc_conf_base.o $(INC) -fPIC -Wall
	g++ rpc_conf_check.cpp rpc_conf.cpp rpc_conf_base.cpp ./lib_linux/inifile.o -o rpc_conf_check $(INC) -Wall

imx6:
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	$(HCC) $(HCFLAGS) rpc_conf.cpp -c -o rpc_conf.o $(INC) -fPIC -Wall
	$(HCC) $(HCFLAGS) rpc_conf_base.cpp -c -o rpc_conf_base.o $(INC) -fPIC -Wall
	$(HCC) $(HCFLAGS) rpc_conf_check.cpp rpc_conf.cpp rpc_conf_base.cpp ./lib_imx6/inifile.o -o rpc_conf_check $(INC) -Wall

ACC += arm-linux-g++

arm:
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	export MGLS_LICENSE_FILE=/opt/study/MentorGraphics/license.txt
	$(ACC) rpc_conf.cpp -c -o rpc_conf.o $(INC) -fPIC -Wall
	$(ACC) rpc_conf_base.cpp -c -o rpc_conf_base.o $(INC) -fPIC -Wall
	$(ACC) rpc_conf_check.cpp rpc_conf.cpp rpc_conf_base.cpp ./lib_arm/inifile.o -o rpc_conf_check $(INC) -Wall

clean:
	rm *.o
