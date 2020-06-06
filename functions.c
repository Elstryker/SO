
#include "functions.h"

extern int* pid; // Pids dos processos filho
extern int nPids; // Numero de pids em pid
extern int exec; // Flag de execução do servidor
extern int tempomaxexec; // Variável de controlo do tempo máximo de execução
extern int maxPipeTime; // Variável de controlo do tempo máximo de inatividade de pipes
extern char** tarefasExec; // Guarda comando da tarefa
extern int* pidsExec; // Array de pids em execução
extern int* nTarefasExec; // Array que guarda o numero das tarefas
extern int used; // Numero de pids no array
extern int tam; // Tamanho do array
extern int fd_pipePro[2]; // Pipe entre processo principal e cada processo filho
extern int nTarefa; // Número da próxima tarefa
extern int statusID; // Identificador de tipo erro
extern int actualStatus; // Estado atual do processo 


void histTerm(){
    int tarefas, tarefasTerminadas;
    if((tarefasTerminadas = open("../SO/TarefasTerminadas.txt",O_RDONLY)) < 0) {
        perror("File not found");
        exit(1);
    }
    char buf[100];
    int server = open("../SO/wr",O_WRONLY);
    while((tarefas = open("../SO/TarefasTerminadas.txt",O_RDONLY)) > 0){
       int readBytes = 0;
        while((readBytes = read(tarefas,buf,100)) > 0) {
            write(server,buf,readBytes);
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
    int filho = -1;
    if((filho=fork()) == 0) {
        statusID = 1;
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
                actualStatus = 0;
                statusID = 2;
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
                _exit(actualStatus);
            }
            if(tempomaxexec > 0)
                alarm(tempomaxexec);
        }
        // Caso haja 2 ou mais pipes
        else if(nmrPipes > 0) {
            if((pid[nPids++] = fork()) == 0) {
                actualStatus = 0;
                statusID = 2;
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
                _exit(actualStatus);
            }
            if(tempomaxexec > 0)
                alarm(tempomaxexec);
        }
        int status;
        wait(&status);
        write(fd_pipePro[1],&status,sizeof(int));
        status = WEXITSTATUS(status);
        actualStatus = status;
        kill(getppid(),SIGUSR1);
        _exit(actualStatus);
    }
    adicionarTarefa(filho, buf);    
    return 0;
}


void adicionarTarefa(int filho, char* buf){
    int fg = 0;
    for(int k = 0; k<used && fg==0; k++){
        if(pidsExec[k]==-1){
            nTarefasExec[k] = nTarefa;
            tarefasExec[k] = malloc(strlen(buf) * sizeof(char));
            strcpy (tarefasExec[k],buf);
            pidsExec[k] = filho;
            nTarefa++;
            fg = 1;
        }
    }
    if(fg==0){
        if(used==tam){
            nTarefasExec = realloc(nTarefasExec, 2*tam*sizeof(int));
            tarefasExec = realloc(tarefasExec, 2*tam*sizeof(char*));
            pidsExec = realloc(pidsExec, 2*tam*sizeof(char*));
            tam *= 2;
        }
        nTarefasExec[used] = nTarefa;
        tarefasExec[used] = malloc(strlen(buf) * sizeof(char));
        strcpy (tarefasExec[used],buf);
        pidsExec[used] = filho;
        nTarefa++;
        used++;
    }
}

int terminarTarefa(char*command){
    int k = 1;
    int tarefasTerminadas;
    if((tarefasTerminadas = open("../SO/TarefasTerminadas.txt",O_WRONLY | O_CREAT | O_APPEND)) < 0) {
        perror("File not found");
        exit(1);
    }
    int n = atoi(command);
    for(int i=0; i<used; i++){
        if(nTarefasExec[i]==n){
            if(pidsExec[i]!=-1){
                //matar tarefa
                k = kill(pidsExec[i],SIGKILL);
                //copiar para ficheiro de terminadas
                char* s =  malloc(100*sizeof(char*));
                sprintf(s, "#%i, Interrompida: %s \n", n, tarefasExec[i]); 
                write(tarefasTerminadas, s, strlen(s));
				pidsExec[i] = -1;
                break;
            }
        }
 	}
    return k;
}

