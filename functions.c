#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>


ssize_t readln(int fd, char*line, size_t size){
	if (fd < 0){
		perror("open: ");
		return 1;
 	}
 	int next_pos=0;
 	int read_bytes = 0;

 	while(next_pos < size && (read(fd, line + next_pos, 1))>0){
 		read_bytes++;
 		if(line[next_pos] == '\n') break;
 		next_pos++;
	}
	
	close(fd);

	return read_bytes;
}


int terminarTarefa(int* tarefasExec, int* pidsExec, int used, int tarefasTerminadas, char*command){
    int k = -1;
    for(int i=0; i<used; i++){
        if(tarefasExec[i]==atoi(command)){
            if(pidsExec[i]!=-1){
                k=0;
                //matar tarefa
                k = kill(pidsExec[i],SIGINT);
                //copiar para ficheiro de terminadas
                write(tarefasTerminadas, command, 10);
				pidsExec[i] = -1;
                break;
            }
        }
 	}
    return k;
}

