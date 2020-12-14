#include "parser.h"


char *getIP(char * hostName);

int connectToIP(char * ip, uint16_t port);

int loginHost(int socketFD, char *user, char *password);

int passiveMode(int socketFD, char *ip, unsigned int *porta);

int retrCommand(int socketFD, char*urlPath);

int disconnect(int socketFD);

int binaryMode(int socketFD, char *ip, unsigned int *porta);