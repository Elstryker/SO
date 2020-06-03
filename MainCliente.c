#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include "functions.h"


int wrtToFIFO(char const *nameFifo){
    int fd;
    if((fd = open(nameFifo,O_WRONLY)) < 0 ){
        perror("open");
        exit(1);
    }

    char *buffer = malloc(100 * sizeof(char));
    char *option = malloc(5 * sizeof(char));
    int bytesRead = 0;
    while((bytesRead = read(0,buffer,100)) >0){
        buffer = mySep(option,buffer,' ');
        write(1,option,strlen(option));
        write(1,buffer,strlen(buffer));
        if( strcmp(option,"-m") == 0 || strcmp(option,"-e") == 0 || strcmp(option,"-l") == 0 ||
            strcmp(option,"-t") == 0 || strcmp(option,"-r") == 0 || strcmp(option,"-h") == 0 || strcmp(option,"-o") == 0){
                perror("comando inválido\n");
            }
      /*  else if(write(fd,option,strlen(option)) < 0 ) {
            perror("write");
            exit(1);
        }*/
    }

    close(fd);
    return 0;
}


int main(int argc, char const *argv[]) {
    const char *myfifo = "../SO/fifo";

    wrtToFIFO(myfifo);

    return 0;
}




