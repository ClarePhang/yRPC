
INC += -I./depend/libevent/include

LIBS += -lpthread
LIBS += -levent_pthreads
LIBS += -L./depend/libevent/lib -levent

all:
	g++ socketbase.cpp -c -o socketbase.o -Wall
	g++ comm_driver.cpp -c -o comm_driver.o $(INC) -Wall
	g++ socketbase.cpp comm_driver.cpp server_test.cpp -o server $(INC) $(LIBS)
	g++ socketbase.cpp comm_driver.cpp client_test.cpp -o client $(INC) $(LIBS)
