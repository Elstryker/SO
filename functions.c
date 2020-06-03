#include "functions.h"


void histTerm(){
    int tarefasFile,tarefas;
    char buf[100];
    while((tarefas = open("../SO/TarefasTerminadas.txt",O_RDONLY)) > 0) {
        int readBytes = 0;
        while((readBytes = read(tarefas,buf,100)) > 0) {
            write(1,buf,readBytes);
        }
        close(readBytes);
        close(tarefas);
    }   
}


char* mySep(char* tok, char *buf, char delim) {
    int i;
    for(i = 0; buf[i]!=delim && buf[i] != '\n'; i++) {
        tok[i] = buf[i];
    }
    tok[i] = '\0';
    return buf+i+1;
}

int executar(char * buf) {
    if(fork() == 0) {
        char**ex, **line;
        line = malloc(10 * sizeof(char*));
        ex = malloc(10 * sizeof(char*));
        int i = 0, fd_pipe[2];
        ex[i++] = strtok(buf," ");
        while((ex[i++] = strtok(NULL," \n")) != NULL);
        ex[i++]=NULL;
        pipe(fd_pipe);
        execvp(ex[0],ex);
    }
    return 0;
}