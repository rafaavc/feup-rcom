#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include "defines.h"


int sendCommand(int socket, char* cmd);

int readReply(int socketFD, char ** reply);

/*Envio e leitura de comandos*/



