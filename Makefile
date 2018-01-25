
all:
	cd ./src/ && make
	cd ./example/ && make

imx6:
	cd ./src/ && make imx6
	cd ./example/ && make imx6

arm:
	cd ./src/ && make arm
	cd ./example/ && make arm

clean:
	cd ./src/ && make clean
	cd ./example/ && make clean
