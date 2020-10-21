/*INTERFACE PROTOCOL-APPLICATION*/
#include "defines.h"
#include "receiverFuncs.h"

/**
 * 
 * @param porta - serial port
 * @param role -EMITTER / RECEIVER
 * @return identifier of data connection, -1 in case of error
 */
int llopen(int porta, char role);


int llwrite(int fd, char * buffer, int length);

int llread(int fd, char * buffer);  

int llclose(int fd);