
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



int output(int n){
    int wr = open("../SO/wr", O_WRONLY);
    int logs = open("logs.txt",O_RDONLY);
    int idx = open("log.idx",O_RDONLY);
    char* index = malloc(100*sizeof(char));
    char* buffer = malloc(1000*sizeof(char));
    int readIdx = read(idx,index,100);
    char* nTarefa = malloc(5*sizeof(char));
    char* indInicial = malloc(5*sizeof(char));
    char* indFinal = malloc(5*sizeof(char));
    while( readIdx > 0 ){
        index = mySep(indFinal,index,"\n");
        indFinal = mySep(nTarefa,indFinal,' ');
        if(atoi(nTarefa)==n){
            indFinal = mySep(indInicial,indFinal,' ');
            int dif = atoi(indFinal)-atoi(indInicial)+1;
            lseek(logs,atoi(indInicial),SEEK_SET);
            int readLogs =read(logs,buffer,dif);
            write(wr,buffer,dif);
            break;
        }
    }
    return 0;
}




void histTerm(int fd){
    int tarefas;
    char buf[100];
    int readBytes = 0;
    tarefas = open("../SO/TarefasTerminadas.txt",O_RDONLY);
    while((readBytes = read(tarefas,buf,100)) > 0) {
        write(fd,buf,readBytes);
    }
    write(fd,"\n",1);
    close(tarefas);
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
    int logs, idx;
    int indInicial;
    char* inicial;
    char* tar;
    if((filho=fork()) == 0) {
        logs = open("logs.txt",O_WRONLY | O_APPEND);
        idx = open("log.idx",O_WRONLY | O_APPEND);
        printf("TAREFA %i \n", nTarefa);
        int nTarefaN= count(nTarefa)+1;
        tar = malloc(nTarefaN*sizeof(char));
        sprintf(tar,"%d ",nTarefa);
        write(idx,tar,nTarefaN);
        indInicial = lseek(logs,0, SEEK_END);
        int indInicialN= count(indInicial)+1;
        printf("INDICE INICIAL %i  \n", indInicial);
        char* inicial = malloc(indInicialN*sizeof(char));
        sprintf(inicial,"%d ",indInicial);
        write(idx,inicial,indInicialN);
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
                dup2(logs,1);
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
                    dup2(logs,1);
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
                    dup2(logs,1);
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

        close(logs);
        logs = open("logs.txt",O_APPEND);

        int indFinal = lseek(logs,0,SEEK_END);
        int indFinalN= count(indFinal)+2;

        //printf("INDICE FINAL %d \n", indFinal);
        char* final = malloc(indFinalN*sizeof(char));  
        sprintf(final,"%d \n",indFinal);
        write(idx,final,indFinalN);


        close(idx);
        close(logs);
        status = WEXITSTATUS(status);
        if(status == 2) status = 2;
        if(status == 0 && actualStatus != 0) status = 1;
        write(fd_pipePro[1],&status,sizeof(int));
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
    if((tarefasTerminadas = open("../SO/TarefasTerminadas.txt",O_WRONLY | O_CREAT | O_APPEND,0666)) < 0) {
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



int count(int numero){
    int n = 0;
    if(numero==0) return 1;
    while(numero!=0){
        numero/=10;
        n++;
    }
    return n;
}