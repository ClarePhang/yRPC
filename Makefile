
all:
	cd ./src/ && make
	cd ./example/ && make

arm:
	cd ./src/ && make arm
	cd ./example/ && make arm

clean:
	cd ./src/ && make clean
	cd ./example/ && make clean
