CC= gcc

CFLAGS= -I -Wall 

all: MainCliente MainServer
		mkfifo fifo
		mkfifo wr
		touch fileTarefa.txt
		echo "0" > fileTarefa.txt
	 


MainCliente: 
		gcc MainCliente.c $(CFLAGS) -o Argus

MainServer: 
		gcc MainServer.c functions.c $(CFLAGS) -o Server
		

clean:
	rm -f *.o Server Argus fifo wr TarefasTerminadas.txt logs.txt log.idx fileTarefa.txt
