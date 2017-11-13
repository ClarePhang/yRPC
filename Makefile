
INC += -I./depend/libevent/include

LIBS += -lpthread
LIBS += -levent_pthreads
LIBS += -L./depend/libevent/lib -levent

all:
	g++ socketbase.cpp -c -o socketbase.o -Wall
	g++ socketserver.cpp -c -o socketserver.o $(INC) -Wall
	#g++ socketclient.cpp -c -o socketclient.o $(INC) -Wall
	g++ socketbase.cpp socketserver.cpp server_test.cpp -o server $(INC) $(LIBS)
	g++ socketbase.cpp socketserver.cpp client_test.cpp -o client $(INC) $(LIBS)
