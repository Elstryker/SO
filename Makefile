CC= gcc

CFLAGS= -I include -Wall -O3 

all: MainCliente MainServer
	mkfifo fifo
	mkfifo wr


MainCliente:
		gcc MainCliente.c functions.c  -o MainCliente

MainServer: 
		gcc MainServer.c functions.c -o MainServer
		


clean:
	rm -f *.o MainCliente MainServer fifo wr
