#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>




int wrtToFIFO(char const *nameFifo){
    int fd;

    if((fd = open(nameFifo,O_WRONLY)) < 0 ){
        perror("open");
        exit(1);
    }


    char buffer[100];
    int bytesRead = 0;

    while((bytesRead = read(0,buffer,100)) >0){
        if(write(fd,buffer,bytesRead) < 0 ) {
            perror("write");
            exit(1);
        }
    }

    close(fd);
    return 0;
}


int main(int argc, char const *argv[]) {

    wrtToFIFO(*argv);
    return 0;
}