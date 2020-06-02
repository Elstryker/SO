CC= gcc

CFLAGS= -Wall -O3 

allprog: MainCliente MainServer

MainCliente: MainCliente.o
		$(CC) MainCliente.o -o MainCliente

MainServer: MainServer.o
		$(CC) MainServer.o -o MainServer

MainCliente.o: MainCliente.c
				$(CC) -c MainCliente.c

MainServer.o: MainServer.c
				$(CC) -c MainServer.c
		
all:
	mkfifo fifo
	make allprog


clean:
	rm -f *.o MainCliente MainServer fifo
