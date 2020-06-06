
#include <sys/stat.h>
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
int fd_pipePro[2];
int nTarefa;


int wrtToFIFO(char const *nameFifo,char *argv[],int argc){
    int fd;
    if((fd = open(nameFifo,O_WRONLY)) < 0 ){
        perror("open");
        exit(1);
    }
    char *buffer = malloc(100 * sizeof(char));
    char *option = malloc(5 * sizeof(char));
    int bytesRead = 0;
    int pid;
        if(argc<=1){
            if(fork()==0){
            while((bytesRead = read(0,buffer,100)) >0){
                char* aux = buffer;
                buffer = mySep(option,buffer,' ');
                buffer = aux;
                if( strcmp(option,"tempo-inactividade") != 0 && strcmp(option,"tempo-execucao") != 0 && strcmp(option,"executar") != 0 && strcmp(option,"listar") != 0 &&
                    strcmp(option,"terminar") != 0 && strcmp(option,"historico") != 0 && strcmp(option,"ajuda") != 0 && strcmp(option,"output") != 0){
                    perror("comando inválido\n");
                    }
                else if(write(fd,buffer,bytesRead) < 0 ) {
                    perror("write");
                    exit(1);
                }
            }
        }
    }

    else{
        if(argc>1){
            if((pid=fork())==0){
                if(strcmp(argv[1],"-m") != 0 && strcmp(argv[1],"-e") != 0  && strcmp(argv[1],"-l") != 0 && strcmp(argv[1],"-e") != 0 &&
                    strcmp(argv[1],"-l") != 0 && strcmp(argv[1],"-r") != 0 && strcmp(argv[1],"-t") != 0 && strcmp(argv[1],"-h") != 0){
                    write(0,"comando inválido",18);
                    }
                else {
                    char *args=malloc(100*sizeof(char));
                    sprintf(args,"%s %s",argv[1],argv[2]);
                    int v = write(fd,args,strlen(args));
                    if( v < 0 ) {
                        perror("write");
                        exit(1);
                    }
                }
            }
            
        }
    }
    close(fd);
    return pid;
}

int rdFromFIFO(const char *myserver,int pid,int argc){
    int fd;
    if((fd = open(myserver,O_RDONLY)) < 0 ){
        perror("open");
        exit(1);
    }
    int bytesRead=0;
    char *buffer = malloc(100 * sizeof(char));
    while((bytesRead = read(fd,buffer,100)) > 0){
        int v = write(0,buffer,strlen(buffer));
        if(v < 0){
            perror("write");
            exit(1);
        }
        close(fd);
        if(argc>1) kill(pid,SIGINT);
    }
   
    
return 0;
}

int main(int argc,char*argv[]) {
    const char *myfifo = "../SO/fifo";
    const char *myserver = "../SO/wr";
    int pid = wrtToFIFO(myfifo,argv,argc);
    rdFromFIFO(myserver,pid,argc);
    return 0;
}




