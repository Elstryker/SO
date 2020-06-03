#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(int argc, char const *argv[]) {
    int fdfifo, fdfile, tarefasTerminadas;
    int tam = 1, used=1;
    int* tarefasExec = malloc(sizeof(int));
    int* pidsExec = malloc(sizeof(int));
    //array numeros tarefas em execuçao
    //array pid associados as tarefas
    char * command, *option;
    char buf[100];
    if((fdfile = open("../SO/logs.txt",O_RDONLY | O_WRONLY | O_TRUNC | O_CREAT)) < 0) {
        perror("File not found");
        exit(1);
    }
    if((tarefasTerminadas = open("../SO/tarefasTerminadas.txt",O_WRONLY | O_TRUNC | O_CREAT)) < 0) {
        perror("File not found");
        exit(1);
    }
    while((fdfifo = open("../SO/fifo",O_RDONLY)) > 0) {
        int readBytes = 0;
        while((readBytes = read(fdfifo,buf,100)) > 0) {
            write(1,buf,readBytes);
            option = strtok(buf," \n");
            command = strtok(NULL," \n");
            if(strcmp(option,"-i") == 0 || strcmp(option,"tempo-inactividade") == 0) {
                write(1,"-i done!",8);
                write(1,command,sizeof(command));
            }
            else if(strcmp(option,"-m") == 0 || strcmp(option,"tempo-execucao") == 0) {
                printf("m option with: %s",command);
            }
            else if(strcmp(option,"-e") == 0 || strcmp(option,"executar") == 0) {
                printf("e option with: %s",command);
            }
            else if(strcmp(option,"-l") == 0 || strcmp(option,"listar") == 0) {
                printf("l option with: %s",command);
            }
            else if(strcmp(option,"-t") == 0 || strcmp(option,"terminar") == 0) {
                int r = terminarTarefa(tarefasExec,pidsExec,used,tarefasTerminadas,command);
                if(r==0) printf("Tarefa não está em execução");
                else printf("Tarefa terminada");
                printf("t option with: %s",command);
            }
            else if(strcmp(option,"-r") == 0 || strcmp(option,"historico") == 0) {
                printf("r option with: %s",command);
            }
            else if(strcmp(option,"-h") == 0 || strcmp(option,"ajuda") == 0) {
                printf("h option with: %s",command);
            }
            else if(strcmp(option,"-o") == 0 || strcmp(option,"output") == 0) {
                printf("o option with: %s",command);
            }
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