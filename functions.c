#include "functions.h"

extern int pid;

void histTerm(){
    int tarefas;
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
    char**ex, **line;
    line = malloc(10 * sizeof(char*));
    ex = malloc(10 * sizeof(char*));
    int indexEx = 0, indexLine = 0, nmrPipes = 0;
    // Partir comando por pipes
    line[indexLine++] = strtok(buf,"|");
    while((line[indexLine++] = strtok(NULL,"|\n")) != NULL);
    indexLine--;
    // Criação do numero de pipes necessários
    nmrPipes = indexLine - 2;
    int fd_pipe[nmrPipes+1][2];
    pipe(fd_pipe[0]);
    // Partir primeiro comando em espaços 
    ex[indexEx++] = strtok(line[0]," \n");
    while((ex[indexEx++] = strtok(NULL," \n")) != NULL);
    indexEx--;
    ex[indexEx]=NULL;
    // Executar comando sem pipes
    if(nmrPipes < 0) {
        if((pid = fork()) == 0) {
            execvp(ex[0],ex);
            _exit(1);
        }
        alarm(4);
    }
    // Executar comando com apenas 1 fd_pipe[0]
    else if(nmrPipes == 0) {
        if(fork() == 0) {
            dup2(fd_pipe[0][1],1);
            close(fd_pipe[0][1]);
            close(fd_pipe[0][0]);
            execvp(ex[0],ex);
            _exit(1);
        }
        close(fd_pipe[0][1]);
        // Partir o segundo comando
        indexEx = 0;
        ex[indexEx++] = strtok(line[1]," \n");
        while((ex[indexEx++] = strtok(NULL," \n")) != NULL);
        indexEx--;
        ex[indexEx]=NULL;
        if(fork() == 0) {
            dup2(fd_pipe[0][0],0);
            close(fd_pipe[0][0]);
            execvp(ex[0],ex);
            _exit(1);
        }
        close(fd_pipe[0][0]);
    }
    // Caso haja 2 ou mais pipes
    else if(nmrPipes > 0) {
        int pipenmr;
        if(fork() == 0) {
            dup2(fd_pipe[0][1],1);
            close(fd_pipe[0][1]);
            execvp(ex[0],ex);
            _exit(1);
        }
        close(fd_pipe[0][1]);
        for(pipenmr = 0; pipenmr < nmrPipes; pipenmr++) {
            pipe(fd_pipe[pipenmr+1]);
            indexEx = 0;
            ex[indexEx++] = strtok(line[pipenmr+1]," \n");
            while((ex[indexEx++] = strtok(NULL," \n")) != NULL);
            indexEx--;
            ex[indexEx]=NULL;
            if(fork() == 0) {
                dup2(fd_pipe[pipenmr][0],0);
                dup2(fd_pipe[pipenmr+1][1],1);
                close(fd_pipe[pipenmr+1][1]);
                close(fd_pipe[pipenmr][0]);
                execvp(ex[0],ex);
                _exit(1);
            }
            close(fd_pipe[pipenmr][0]);
            close(fd_pipe[pipenmr+1][1]);
        }
        close(fd_pipe[pipenmr][1]);
        indexEx = 0;
        ex[indexEx++] = strtok(line[pipenmr+1]," \n");
        while((ex[indexEx++] = strtok(NULL," \n")) != NULL);
        indexEx--;
        ex[indexEx] = NULL;
        if(fork() == 0) {
            dup2(fd_pipe[pipenmr][0],0);
            close(fd_pipe[pipenmr][0]);
            execvp(ex[0],ex);
            _exit(1);
        }
        close(fd_pipe[pipenmr][0]);
    }
    return 0;
}