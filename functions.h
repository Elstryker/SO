#ifndef functions_h
#define functions_h

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

char* mySep(char* tok, char* buf, char delim);
int executar(char* buf);
void histTerm();
int terminarTarefa(int tarefasTerminadas, char*command);

#endif

