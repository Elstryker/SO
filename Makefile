CC= gcc

CFLAGS= -Wall -O3 

MainCliente: MainCliente.o
		$(CC) MainCliente.o

MainServer: MainServer.o
		$(CC) MainServer.o
all:
	make allprog



clean:
	rm -f *.o MainCliente MainServer
