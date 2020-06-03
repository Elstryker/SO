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
    }
    close(tarefas);
    
}

int tempo_inactividade = -1;

void sigint_handler(int signum) {
    
}

int main(int argc, char const *argv[]) {
    if(signal(SIGINT,sigint_handler)==SIG_ERR) {
        perror("Error");
        exit(1);
    }
    int fdfifo, fdfile;
    char * buf, *option;
    if((fdfile = open("../SO/logs.txt",O_WRONLY | O_TRUNC | O_CREAT)) < 0) {
        perror("File not found");
        exit(1);
    }
    option = malloc(5 * sizeof(char));
    buf = malloc(100 * sizeof(char));
    while((fdfifo = open("../SO/fifo",O_RDONLY)) > 0) {
        int readBytes = 0;
        while((readBytes = read(fdfifo,buf,100)) > 0) {
            buf = mySep(option,buf,' ');
            if(strcmp(option,"-i") == 0 || strcmp(option,"tempo-inactividade") == 0) {
                write(1,"-i done!",8);
                write(1,buf,readBytes-2);
            }
            else if(strcmp(option,"-m") == 0 || strcmp(option,"tempo-execucao") == 0) {
                printf("m option with: %s",buf);
            }
            else if(strcmp(option,"-e") == 0 || strcmp(option,"executar") == 0) {
                executar(buf);
            }
            else if(strcmp(option,"-l") == 0 || strcmp(option,"listar") == 0) {
                printf("l option with: %s",buf);
            }
            else if(strcmp(option,"-t") == 0 || strcmp(option,"terminar") == 0) {
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
        perror("Not found");
        exit(1);
    }
    else close(fdfifo);
    close(fdfile);
    return 0;
}
