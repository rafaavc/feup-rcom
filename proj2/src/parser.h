#include "commandManager.h"

#define URL_PROTOCOL "ftp://"

enum urlParsingState {
    PROTOCOL_OK,
    USERNAME_OK,
    PASSWORD_OK,
    HOST_OK,
    PATH_OK
};


int strneedle(char * haystack, char needle);

int parseURL(char *url, char **user, char **password, char **host, char **urlPath);

int parsePsv(char *psvReply, char *ip, unsigned int *porta);

