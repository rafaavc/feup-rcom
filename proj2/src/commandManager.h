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

/**
 * Sending and reading commands
*/


/**
 * Sends the command
*/
int sendCommand(int socket, char* cmd);
/**
 * Reads the server reply
*/
int readReply(int socketFD, unsigned * replyCode, char * reply);



