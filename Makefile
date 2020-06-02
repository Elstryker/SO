CC= gcc

CFLAGS= -I include -Wall -O3 

allprog: MainCliente MainServer


MainCliente.o: MainCliente.c
				$(CC) -c MainCliente.c

MainServer.o: MainServer.c  functions.h 
				$(CC) -c MainServer.c

MainCliente: MainCliente.o
		$(CC) $(CFLAGS) MainCliente.o -o MainCliente

MainServer: MainServer.o functions.o
		$(CC) $(CFLAGS) -o MainServer.o -o MainServer
		
all:
	mkfifo fifo
	make allprog


clean:
	rm -f *.o MainCliente MainServer fifo
