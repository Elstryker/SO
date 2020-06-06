#include "functions.h"

int* pid; // Pids dos processos filho
int nPids; // Numero de pids em pid
int exec; // Flag de execução do servidor
int tempomaxexec; // Variável de controlo do tempo máximo de execução
int maxPipeTime; // Variável de controlo do tempo máximo de inatividade de pipes
char** tarefasExec; // Guarda comando da tarefa
int* pidsExec; // Array de pids em execução
int* nTarefasExec; // Array que guarda o numero das tarefas
int used; // Numero de pids no array
int tam; // Tamanho do array
int fd_pipePro[2]; // Pipe entre processo principal e cada processo filho
int nTarefa; // Número da próxima tarefa
int statusID; // Identificador de tipo estado
int actualStatus; // Estado atual do processo 

void alrm_hand(int signum) {
    actualStatus = statusID;
    for(int x = 0; x < nPids; x++) {
        kill(pid[x],SIGALRM);
        wait(NULL);
    }
    for(int x = 0; x < nPids; x++) {
        printf("Killing PID %d\n", pid[x]);
        if(pid[x] != -1)
            kill(pid[x],SIGKILL);
    }
}

void sigusr1_handler(int signum) {
    int pidusr, status, x, fd, numTarefa;
    char* buf, command;
    buf = malloc(200 * sizeof(char));
    read(fd_pipePro[0],&pidusr,sizeof(int));
    pidusr = wait(&status);
    status = WEXITSTATUS(status);
    for(x = 0; x < used; x++) {
        if(pidsExec[x] == pidusr) {
            pidsExec[x] = -1;
            command = strdup(tarefasExec[x]);
            free(tarefasExec[x]);
            numTarefa = nTarefasExec[x];
            nTarefasExec[x] = -1;
        }
    }
    if((fd = open("../SO/TarefasTerminadas.txt",O_WRONLY | O_CREAT | O_APPEND)) < 0) {
        perror("File not found");
    }
    else {
        if(status == 0) {
            sprintf(buf,"#%d, concluida: %s\n",numTarefa,command);
            write(fd,buf,strlen(buf));
        }
        else if(status == 1) {
            sprintf(buf,"#%d, max execucao: %s\n",numTarefa,command);
            write(fd,buf,strlen(buf));
        }
        else if(status == 2) {
            sprintf(buf,"#%d, max inatividade: %s\n",numTarefa,command);
            write(fd,buf,strlen(buf));
        }
    }
}

void int_handler(int signum) {
    exec = 0;
}

int main(int argc, char const *argv[]) {
    int fdfifo, fdfile, wrfifo;
    char * buf, *option;
    if((fdfile = open("../SO/logs.txt",O_WRONLY | O_APPEND | O_CREAT)) < 0) {
        perror("File not found");
        exit(1);
    }
    pipe(fd_pipePro);
    tempomaxexec = -1;
    maxPipeTime = -1;
    exec = 1;
    pid = malloc(10 * sizeof(int));
    nPids = 0;
    nTarefa = 0;
    tam = 1;
    used=0;
    tarefasExec = malloc(sizeof(char*));
    nTarefasExec = malloc(sizeof(int));
    for(int d = 0; d < tam; d++) {
        nTarefasExec[d] = -1;
        pidsExec[d] = -1;
    } 
    pidsExec = malloc(sizeof(int));
    option = malloc(5 * sizeof(char));
    buf = malloc(100 * sizeof(char));
    signal(SIGALRM,alrm_hand);
    signal(SIGINT,int_handler);
    signal(SIGUSR1,sigusr1_handler);
    fdfifo = open("../SO/fifo",O_RDONLY);
    wrfifo = open("../SO/wr",O_WRONLY);
    printf("Main PID %d\n",getpid());
    while(fdfifo > 0 && exec) {
        int readBytes = 0;
        while((readBytes = read(fdfifo,buf,100)) > 0) {
            buf = mySep(option,buf,' ');
            if(strcmp(option,"-i") == 0 || strcmp(option,"tempo-inactividade") == 0) {
                maxPipeTime = atoi(buf);
            }
            else if(strcmp(option,"-m") == 0 || strcmp(option,"tempo-execucao") == 0) {
                tempomaxexec = atoi(buf);
            }
            else if(strcmp(option,"-e") == 0 || strcmp(option,"executar") == 0) {
                
                executar(buf);
            }
            else if(strcmp(option,"-l") == 0 || strcmp(option,"listar") == 0) {
                for(int n = 0; n<used; n++)
                    if(pidsExec[n]!=-1)
                        write(wrfifo, tarefasExec[n], strlen(tarefasExec[n]));
                printf("l option with: %s",buf);
            }
            else if(strcmp(option,"-t") == 0 || strcmp(option,"terminar") == 0) {
                int r = terminarTarefa(tarefasTerminadas,buf);
                if(r==0)  write(wrfifo, "Tarefa terminada", 17);
                else if (r==-1) write(wrfifo, "Não é possível terminar a tarefa", 36);
                else write(wrfifo, "Tarefa não está em execução", 32);
                printf("t option with: %s",buf);
            }
            else if(strcmp(option,"-r") == 0 || strcmp(option,"historico") == 0) {
                printf("r option with: %s",buf);
                histTerm();
            }
            else if(strcmp(option,"-h") == 0 || strcmp(option,"ajuda") == 0) {
                write (wrfifo,"tempo-inatividade",18);
                write (wrfifo,"tempo-execucao",15);
                write (wrfifo,"executar",9);
                write (wrfifo,"listar",17);
                write (wrfifo,"terminar",9);
                write (wrfifo,"historico",10);
                write (wrfifo,"ajuda",6);
                write (wrfifo,"output",7);
                printf("h option with: %s",buf);
            }
            else if(strcmp(option,"-o") == 0 || strcmp(option,"output") == 0) {
                printf("o option with: %s",buf);
            }
            for(int j = 0; j < 100; j++) buf[j] = '\0';
        }       
    }
    if(fdfifo < 0) {
        perror("Negative fd");
    }
    close(fdfifo);
    close(fdfile);
    return 0;
}
