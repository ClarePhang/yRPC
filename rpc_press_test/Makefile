
INC += -I../src/

LIB += -L../src/ -lrpc
LIB += -lpthread

linux:
	g++ client.cpp module.cpp rpc_test.cpp -o test0 $(INC) $(LIBL) $(LIB) -Wall
	cp test0 test1
	cp test0 test2
	cp test0 test3
	cp test0 test4

clean:
	rm test*

