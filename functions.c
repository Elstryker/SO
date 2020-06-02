#include "functions.h"

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