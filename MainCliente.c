#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* separateString(char * tok, char* buf) {
    int i, j = 0;
    char * new;
    new = malloc(100 * sizeof(char));
    for(i = 0; buf[i]!=' ' && buf[i] != '\n' && buf[i]; i++) {
        tok[i] = buf[i];
    }
    tok[i] = '\0';
    for(i = 0; buf[i]; i++) {
        if(buf[i] != '\'' && buf[i] != '\"' && buf[i] != '\n')
            new[j++] = buf[i];
    }
    new[j] = '\0';
    return new;
}

void shellInterpreter(int fdToServer, int fdFromServer) {
    char* option, *buf;
    int bytesRead;
    buf = malloc(150 * sizeof(char));
    option = malloc(25 * sizeof(char));
    while((bytesRead = read(0,buf,100)) > 0) {
        buf[strlen(buf)-1] = '\0';
        if(strcmp(buf,"sair") == 0) break; 
        if(fork() == 0) {
            buf = separateString(option,buf);
            if( strcmp(option,"tempo-inactividade") != 0 && 
                strcmp(option,"tempo-execucao") != 0 && 
                strcmp(option,"executar") != 0 && 
                strcmp(option,"listar") != 0 &&
                strcmp(option,"terminar") != 0 && 
                strcmp(option,"historico") != 0 && 
                strcmp(option,"ajuda") != 0 && 
                strcmp(option,"output") != 0) {
                    write(1,"Comando Inválido\n",19);
                }
            else if(write(fdToServer,buf,strlen(buf)) < 0) {
                perror("Fifo");
                exit(1);
            }
            while((bytesRead = read(fdFromServer,buf,150)) > 0) {
                if(write(1,buf,bytesRead) < 0) {
                    perror("Write"),
                    exit(1);
                }
            }
        }
    }
}

void commandInterpreter(int fdToServer, int fdFromServer, char argv1[], char argv2[]) {
    char *args=malloc(150*sizeof(char));
    int bytesRead;
    sprintf(args,"%s %s",argv1,argv2);
    if( strcmp(argv1,"-i") != 0 && 
        strcmp(argv1,"-m") != 0 && 
        strcmp(argv1,"-e") != 0 && 
        strcmp(argv1,"-l") != 0 && 
        strcmp(argv1,"-e") != 0 &&
        strcmp(argv1,"-l") != 0 && 
        strcmp(argv1,"-r") != 0 && 
        strcmp(argv1,"-t") != 0 && 
        strcmp(argv1,"-h") != 0 &&
        strcmp(argv1,"-o") != 0) {
            write(1,"Comando Inválido\n",19);
    }
    else if(write(fdToServer,args,strlen(args)) < 0) {
        perror("Write");
        exit(1);
    }
    while((bytesRead = read(fdFromServer,args,150)) > 0) {
        if(write(1,args,bytesRead) < 0) {
            perror("Write"),
            exit(1);
        }
    }
}

int main(int argc,char*argv[]) {
    int fdToServer, fdFromServer;
    if((fdToServer = open("../SO/fifo", O_WRONLY)) < 0) {
        perror("Fifo error");
        exit(1);
    }
    if((fdFromServer = open("../SO/wr", O_RDONLY)) < 0) {
        perror("Fifo error");
        exit(1);
    }
    if(argc == 1) {
        shellInterpreter(fdToServer,fdFromServer);
    }
    else if(argc > 1) {
        commandInterpreter(fdToServer, fdFromServer, argv[1], argv[2]);
    }
    close(fdFromServer);
    close(fdToServer);
    return 0;
}