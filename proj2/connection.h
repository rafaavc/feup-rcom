#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

int parseURL(char *url, char **user, char **password, char **host, char **urlPath);

int strneedle(char * haystack, char needle);

char *getIP(char * hostName);

int loginHost(int fdSocket, char *user, char *password);


/* Todas as funçoes de construçao de comandos e parser do input*/