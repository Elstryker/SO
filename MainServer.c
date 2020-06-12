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
    char* buf, *command;
    buf = malloc(200 * sizeof(char));
    read(fd_pipePro[0],&status,sizeof(int));
    pidusr = wait(NULL);
    for(x = 0; x < used; x++) {
        if(pidsExec[x] == pidusr) {
            pidsExec[x] = -1;
            command = strdup(tarefasExec[x]);
            free(tarefasExec[x]);
            numTarefa = nTarefasExec[x];
            nTarefasExec[x] = -1;
        }
    }
    if((fd = open("../SO/TarefasTerminadas.txt",O_WRONLY | O_CREAT | O_APPEND, 0666)) < 0) {
        perror("File not found");
    }
    else {
        if(status == 0) {
            sprintf(buf,"#%d, concluida: %s",numTarefa,command);
            write(fd,buf,strlen(buf));
        }
        else if(status == 1) {
            sprintf(buf,"#%d, max execucao: %s",numTarefa,command);
            write(fd,buf,strlen(buf));
        }
        else if(status == 2) {
            sprintf(buf,"#%d, max inatividade: %s",numTarefa,command);
            write(fd,buf,strlen(buf));
        }
    }
}

int main(int argc, char const *argv[]) {
    int fdfifo, fdfile, wrfifo, fileTarefa;
    char * buf, *option;

    if((fileTarefa = open("../SO/fileTarefa.txt",O_RDWR | O_CREAT, 0666)) < 0) {
        perror("File not found");
        exit(1);
    }
    int currentTarefa = 0;
    char* linhaTarefa = malloc(10*sizeof(char));
    read(fileTarefa,linhaTarefa,10);
    currentTarefa = atoi(linhaTarefa);
    nTarefa=currentTarefa;
    free(linhaTarefa);
    if((fdfile = open("../SO/logs.txt",O_WRONLY | O_APPEND | O_CREAT, 0666)) < 0) {
        perror("File not found");
        exit(1);
    }
    pipe(fd_pipePro);
    tempomaxexec = -1;
    maxPipeTime = -1;
    exec = 1;
    pid = malloc(10 * sizeof(int));
    nPids = 0;
    tam = 1;
    used=0;
    tarefasExec = malloc(sizeof(char*));
    nTarefasExec = malloc(sizeof(int));
    pidsExec = malloc(sizeof(int));
    for(int d = 0; d < tam; d++) {
        nTarefasExec[d] = -1;
        pidsExec[d] = -1;
    }
    option = malloc(25 * sizeof(char));
    buf = malloc(100 * sizeof(char));
    if(signal(SIGALRM,alrm_hand) == SIG_ERR) {
        perror("Signal");
        exit(1);
    }
    if(signal(SIGUSR1,sigusr1_handler) == SIG_ERR) {
        perror("Signal");
        exit(1);
    }
    fdfifo = open("../SO/fifo",O_RDONLY);
    while(fdfifo > 0 && exec) {
        int readBytes = 0;
        while((readBytes = read(fdfifo,buf,100)) > 0) {
            wrfifo = open("../SO/wr",O_WRONLY);
            buf = mySep(option,buf,' ');
            write(1,option,strlen(option));
            if(strcmp(option,"-i") == 0 || strcmp(option,"tempo-inactividade") == 0) {
                maxPipeTime = atoi(buf);
                write(wrfifo,"Novo tempo máximo de inatividade\n",35);
            }
            else if(strcmp(option,"-m") == 0 || strcmp(option,"tempo-execucao") == 0) {
                tempomaxexec = atoi(buf);
                write(wrfifo,"Novo tempo máximo de execucao\n",32);
            }
            else if(strcmp(option,"-e") == 0 || strcmp(option,"executar") == 0) {
                executar(buf);
                char * temp = malloc(25 * sizeof(char));
                sprintf(temp,"Nova tarefa: %d\n",nTarefa);
                write(wrfifo,temp,strlen(temp));
                free(temp);
            }
            else if(strcmp(option,"-l") == 0 || strcmp(option,"listar") == 0) {
                for(int n = 0; n<used; n++) {
                    if(pidsExec[n]!=-1) {
                        write(wrfifo, tarefasExec[n], strlen(tarefasExec[n]));
                    }
                }
            }
            else if(strcmp(option,"-t") == 0 || strcmp(option,"terminar") == 0) {
                int r = terminarTarefa(buf);
                if(r==0)  write(wrfifo, "Tarefa terminada", 17);
                else if (r==-1) write(wrfifo, "Não é possível terminar a tarefa", 36);
                else write(wrfifo, "Tarefa não está em execução", 32);
            }
            else if(strcmp(option,"-r") == 0 || strcmp(option,"historico") == 0) {
                histTerm(wrfifo);
            }
            else if(strcmp(option,"-h") == 0 || strcmp(option,"ajuda") == 0) {
                write(wrfifo,"tempo-inatividade segs \n tempo-execucao segs \n executar p1 | p2 ... | pn \n listar \n terminar n \n historico \n ajuda \n output n \n",128);
            }
            else if(strcmp(option,"-o") == 0 || strcmp(option,"output") == 0) {
                output(atoi(buf));
            }
            
            close(wrfifo);
        }
    int countTarefa=count(nTarefa);
    char* tarefaNumero = malloc(countTarefa*sizeof(char));
    sprintf(tarefaNumero,"%d",nTarefa);
    lseek(fileTarefa, 0, SEEK_SET);
    write(fileTarefa,tarefaNumero,countTarefa);
    close(fileTarefa);       
    free(tarefaNumero);
    }
    if(fdfifo < 0) {
        perror("Negative fd");
    }

    close(fdfifo);
    close(fdfile);
    return 0;
}