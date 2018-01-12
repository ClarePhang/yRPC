
INC += -I./depend/
INC += -I./depend/message/
INC += -I./depend/libevent/
INC += -I./depend/libevent/include/
INC += -I./depend/libinifile/
INC += -I./depend/libinifile/include/

LIB += -lrpc
LIB += -lpthread -levent_pthreads
LIB += -L./depend/
LIB += -L./depend/libevent/lib -levent
LIB += -L./depend/libinifile/lib -linifile

all:
#	g++ client.cpp -o client $(INC) $(LIB) -Wall
	g++ server.cpp -o server $(INC) $(LIB) -Wall
