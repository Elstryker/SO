CC= gcc

CFLAGS= -I -Wall 

all: 	argus argusd
		mkfifo fifo
		mkfifo wr
		touch fileTarefa.txt
		echo "0" > fileTarefa.txt
	 


Argus: 
		($CC) $(CFLAGS) argus.c -o argus

Argusd: 
		($CC) $(CFLAGS) argusd.c functions.c -o argusd
		

clean:
	rm -f *.o argus argusd fifo wr TarefasTerminadas.txt logs.txt log.idx fileTarefa.txt
