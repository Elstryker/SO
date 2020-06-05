#include "functions.h"

int* pid;
int nPids;
int exec;
int tempomaxexec;
int maxPipeTime;
char** nTarefasExec;
int* pidsExec;
int* tarefasExec;
int used;
int tam;

void alrm_hand(int signum) {
    for(int x = 0; x < nPids; x++) {
        kill(pid[x],SIGALRM);
        wait(NULL);
        printf("Killing PID %d\n", pid[x]);
        if(pid[x] != -1)
            kill(pid[x],SIGKILL);
    }
}

void int_handler(int signum) {
    exec = 0;
}

int main(int argc, char const *argv[]) {
    int fdfifo, fdfile,tarefasTerminadas, wrfifo;
    char * buf, *option;
    if((fdfile = open("../SO/logs.txt",O_WRONLY | O_TRUNC | O_CREAT)) < 0) {
        perror("File not found");
        exit(1);
    }
    tempomaxexec = -1;
    exec = 1;
    if((tarefasTerminadas = open("../SO/tarefasTerminadas.txt",O_WRONLY | O_TRUNC | O_CREAT)) < 0) {
        perror("File not found");
        exit(1);
    }
    pid = malloc(10 * sizeof(int));
    nPids = 0;
    tam = 1;
    used=1;
    tarefasExec = malloc(sizeof(char*));
    nTarefasExec = malloc(sizeof(int));
    pidsExec = malloc(sizeof(int));
    option = malloc(5 * sizeof(char));
    buf = malloc(100 * sizeof(char));
    signal(SIGALRM,alrm_hand);
    signal(SIGINT,int_handler);
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
                if(r==0)  write(wrfifo, "Tarefa terminada", strlen("Tarefa terminada"));
                else if (r==-1) write(wrfifo, "Não é possível terminar a tarefa", strlen("Não é possível terminar a tarefa"));
                else write(wrfifo, "Tarefa não está em execução", strlen("Tarefa não está em execução"));
                printf("t option with: %s",buf);
            }
            else if(strcmp(option,"-r") == 0 || strcmp(option,"historico") == 0) {
                printf("r option with: %s",buf);
                histTerm();
            }
            else if(strcmp(option,"-h") == 0 || strcmp(option,"ajuda") == 0) {
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
