CC= gcc

CFLAGS= -I -Wall 

all: MainCliente MainServer
		mkfifo fifo
		mkfifo wr
	 


MainCliente: 
		gcc MainCliente.c $(CFLAGS) -o MainCliente

MainServer: 
		gcc MainServer.c functions.c $(CFLAGS) -o MainServer
		


clean:
	rm -f *.o MainCliente MainServer fifo wr TarefasTerminadas.txt logs.txt log.idx fileTarefa.txt
