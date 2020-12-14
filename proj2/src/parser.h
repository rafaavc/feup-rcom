#include "commandManager.h"

#define URL_PROTOCOL "ftp://"

enum urlParsingState {
    PROTOCOL_OK,
    USERNAME_OK,
    PASSWORD_OK,
    HOST_OK,
    PATH_OK
};


/**
 * Parses the received url into the necessary variables
*/
int parseURL(char *url, char **user, char **password, char **host, char **urlPath, char **filename);
/**
 * Parses the response from the server and calculates the port in which the server is waiting for connection
*/
int parsePsv(char *psvReply, char *ip, unsigned int *porta);


void copyBufferToVar(char * buffer, size_t bufferSize, char ** out);

