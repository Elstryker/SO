CC= gcc

CFLAGS= -Wall -O3 


MainCliente: MainCliente.o
		$(CC) MainCliente.o -o MainCliente

MainServer: MainServer.o
		$(CC) MainServer.o -o MainServer
all:
	mkfifo fifo
		make allprog


clean:
	rm -f *.o MainCliente MainServer cliente
