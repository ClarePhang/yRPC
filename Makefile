
all:
	cd ./src/ && make
	cd ./example/ && make
	cd ./rpc_press_test && make

imx6:
	cd ./src/ && make imx6
	cd ./example/ && make imx6
	cd ./rpc_press_test && make imx6

arm:
	cd ./src/ && make arm
	cd ./example/ && make arm
	cd ./rpc_press_test && make arm

clean:
	cd ./src/ && make clean
	cd ./example/ && make clean
	cd ./rpc_press_test && make clean

distclean:
	cd ./src/ && make distclean
	cd ./example/ && make clean
	cd ./rpc_press_test && make clean

