
#include "functions.h"

extern int* pid;
extern int nPids;
extern int tempomaxexec;
extern int maxPipeTime;
extern int fd_pipePro[2];

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
    for(i = 0; buf[i]!=delim && buf[i] != '\n' && buf[i]; i++) {
        tok[i] = buf[i];
    }
    tok[i] = '\0';
    return buf+i+1;
}

int executar(char * buf) {
    if(fork() == 0) {
        nPids = 0;
        printf("Execute PID %d\n",getpid());
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
            if((pid[nPids++] = fork()) == 0) {
                printf("PID child: %d\n",getpid());
                execvp(ex[0],ex);
                _exit(1);
            }
            if(tempomaxexec > 0)
                alarm(tempomaxexec);
        }
        // Executar comando com apenas 1 fd_pipe[0]
        else if(nmrPipes == 0) {
            if((pid[nPids++] = fork()) == 0) {
                pid = malloc(10 * sizeof(int));
                nPids = 0;
                printf("PID child: %d\n",getpid());
                if((pid[nPids++] = fork()) == 0) {
                    dup2(fd_pipe[0][1],1);
                    close(fd_pipe[0][1]);
                    close(fd_pipe[0][0]);
                    execvp(ex[0],ex);
                    _exit(1);
                }
                if(maxPipeTime > 0) alarm(maxPipeTime);
                close(fd_pipe[0][1]);
                // Partir o segundo comando
                indexEx = 0;
                ex[indexEx++] = strtok(line[1]," \n");
                while((ex[indexEx++] = strtok(NULL," \n")) != NULL);
                indexEx--;
                ex[indexEx]=NULL;
                if((pid[nPids++] = fork()) == 0) {
                    dup2(fd_pipe[0][0],0);
                    close(fd_pipe[0][0]);
                    execvp(ex[0],ex);
                    _exit(1);
                }
                wait(NULL);
                close(fd_pipe[0][0]);
                _exit(0);
            }
            if(tempomaxexec > 0)
                alarm(tempomaxexec);
        }
        // Caso haja 2 ou mais pipes
        else if(nmrPipes > 0) {
            if((pid[nPids++] = fork()) == 0) {
                pid = malloc(10 * sizeof(int));
                nPids = 0;
                printf("PID child: %d\n",getpid());
                int pipenmr;
                if((pid[nPids++] = fork()) == 0) {
                    dup2(fd_pipe[0][1],1);
                    close(fd_pipe[0][1]);
                    execvp(ex[0],ex);
                    _exit(1);
                }
                if(maxPipeTime > 0) alarm(maxPipeTime);
                close(fd_pipe[0][1]);
                for(pipenmr = 0; pipenmr < nmrPipes; pipenmr++) {
                    pipe(fd_pipe[pipenmr+1]);
                    indexEx = 0;
                    ex[indexEx++] = strtok(line[pipenmr+1]," \n");
                    while((ex[indexEx++] = strtok(NULL," \n")) != NULL);
                    indexEx--;
                    ex[indexEx]=NULL;
                    if((pid[nPids++] = fork()) == 0) {
                        dup2(fd_pipe[pipenmr][0],0);
                        dup2(fd_pipe[pipenmr+1][1],1);
                        close(fd_pipe[pipenmr+1][1]);
                        close(fd_pipe[pipenmr][0]);
                        execvp(ex[0],ex);
                        _exit(1);
                    }
                    if(maxPipeTime > 0) alarm(maxPipeTime);
                    close(fd_pipe[pipenmr][0]);
                    close(fd_pipe[pipenmr+1][1]);
                }
                close(fd_pipe[pipenmr][1]);
                indexEx = 0;
                ex[indexEx++] = strtok(line[pipenmr+1]," \n");
                while((ex[indexEx++] = strtok(NULL," \n")) != NULL);
                indexEx--;
                ex[indexEx] = NULL;
                if((pid[nPids++] = fork()) == 0) {
                    dup2(fd_pipe[pipenmr][0],0);
                    close(fd_pipe[pipenmr][0]);
                    execvp(ex[0],ex);
                    _exit(1);
                }
                if(maxPipeTime > 0) alarm(maxPipeTime);
                wait(NULL);
                close(fd_pipe[pipenmr][0]);
                _exit(0);
            }
            if(tempomaxexec > 0)
                alarm(tempomaxexec);
        }
        int status;
        int pidusr;
        pidusr = wait(&status);
        write(fd_pipePro[1],&pidusr,sizeof(int));
        kill(getppid(),SIGUSR1);
        _exit(0);
    }
    return 0;
}

int terminarTarefa(int* tarefasExec, int* pidsExec, int used, int tarefasTerminadas, char*command){
    int k = 1;
    for(int i=0; i<used; i++){
        if(tarefasExec[i]==atoi(command)){
            if(pidsExec[i]!=-1){
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
