CC= gcc

CFLAGS= -I -Wall -O3 

all: MainCliente MainServer
		mkfifo fifo
		mkfifo wr
	 


MainCliente: 
		gcc MainCliente.c functions.c  $(CFLAGS) -o MainCliente

MainServer: 
		gcc MainServer.c functions.c $(CFLAGS) -o MainServer
		


clean:
	rm -f *.o MainCliente MainServer fifo wr
