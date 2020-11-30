#include "commandManager.h"

/* Todas as funçoes de construçao de comandos e parser do input*/

int parseURL(char *url, char **user, char **password, char **host, char **urlPath);

int parsePsv(char *psvReply, char *ip, unsigned int *porta);

int strneedle(char * haystack, char needle);

char *getIP(char * hostName);

int connectToIP(char * ip, uint16_t port);

int loginHost(int socketFD, char *user, char *password);

int passiveMode(int socketFD, char *ip, unsigned int *porta);

int retrCommand(int socketFD, char*urlPath);

