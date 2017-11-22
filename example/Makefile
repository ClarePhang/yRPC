
INC += -I../depend/
INC += -I../depend/libevent/include/

LIB += -lrpc
LIB += -lpthread -levent_pthreads
LIB += -L../depend/
LIB += -L../depend/libevent/lib -levent

all:
	g++ hash_test.cpp -o hash_test $(INC) $(LIB) -Wall
	g++ list_test.cpp -o list_test $(INC) $(LIB) -Wall
	g++ server_test.cpp -o server $(INC) $(LIB) -Wall
	g++ client_test.cpp -o client $(INC) $(LIB) -Wall
	g++ thread_test.cpp -o thread $(INC) $(LIB) -Wall
