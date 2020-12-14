#include "connection.h"

/**
 *   Function that takes care of the download of the file
*/
int receiveFile(int dataSocketFD);

/**
* Function that calls all functions where the commands are executed
*/
int fileTransfer(int socketFD, char * user, char * password, char * host, char * urlPath);


