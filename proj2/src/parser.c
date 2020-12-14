#include "parser.h"

void copyBufferToVar(char * buffer, size_t bufferSize, char ** out) {
    *out = malloc((strlen(buffer)+1)*sizeof(char));
    memcpy(*out, buffer, (strlen(buffer)+1)*sizeof(char));
    memset(buffer, 0, bufferSize);
}

int parseURL(char *url, char **user, char **password, char **host, char **urlPath, char **filename) {
    if (strncmp(URL_PROTOCOL, url, 6) != 0) {
        fprintf(stderr, "The protocol isn't valid. Should be '%s'.\n", URL_PROTOCOL);
        return 1;
    }
    enum urlParsingState state = PROTOCOL_OK;

    size_t bufferSize = (strlen(url))*sizeof(char);
    char * buffer = malloc(bufferSize);

    for (unsigned i = 6; i < strlen(url)+1; i++) {
        char currentChar = url[i];
        enum urlParsingState prevState = state;
        switch (state) {
            case PROTOCOL_OK:
                switch (currentChar) {
                    case ':':
                        copyBufferToVar(buffer, bufferSize, user);
                        state = USERNAME_OK;
                        break;
                    case '/':
                        copyBufferToVar(buffer, bufferSize, host);
                        state = HOST_OK;
                        break;
                    case '@':
                        fprintf(stderr, "Invalid ftp url: Expecting username or host, '@' in the wrong place\n");
                        return -1;
                    default: break;
                }
                break;
            case USERNAME_OK:
                switch(currentChar) {
                    case ':':
                        fprintf(stderr, "Invalid ftp url: Expecting password, ':' in the wrong place\n");
                        return -1;
                    case '/':
                        fprintf(stderr, "Invalid ftp url: Expecting password, '/' in the wrong place\n");
                        return -1;
                    case '@':
                        copyBufferToVar(buffer, bufferSize, password);
                        state = PASSWORD_OK;
                        break;
                    default: break;
                }
                break;
            case PASSWORD_OK:
                switch(currentChar) {
                    case '/':
                        copyBufferToVar(buffer, bufferSize, host);
                        state = HOST_OK;
                        break;
                    case '@':
                        fprintf(stderr, "Invalid ftp url: Expecting host, '@' in the wrong place\n");
                        return -1;
                    default: break;
                }
                break;
            case HOST_OK:
                if (i == (unsigned) strlen(url)) {
                    copyBufferToVar(buffer, bufferSize, urlPath);
                    char * tempUrlPath = malloc(strlen(*urlPath)*sizeof(char));
                    strcpy(tempUrlPath, *urlPath);
                    char * tok = strtok(tempUrlPath, "/");
                    while (tok != NULL) {
                        tok = strtok(NULL, "/");
                        if (tok != NULL) {
                            *filename = malloc(strlen(tok)*sizeof(char));
                            strcpy(*filename, tok);
                        }
                    }
                    free(buffer);
                    return 0;
                }
                break;
            default:
                fprintf(stderr, "Invalid urlParsingState.\n");
                break;
        }
        if (prevState == state) {
            buffer[strlen(buffer)] = currentChar;
            buffer[strlen(buffer)+1] = '\0';
        }
    }
    return -1;
}



int parsePsv(char *psvReply, char *ip, unsigned int *porta){ // 227 Entering Passive Mode (193,136,28,12,19,91)
    unsigned int ip1, ip2, ip3, ip4, p1, p2;

    if(sscanf(psvReply, "227 Entering Passive Mode (%u, %u, %u, %u, %u, %u)",&ip1, &ip2, &ip3, &ip4, &p1, &p2) != 6){
        fprintf(stderr, "Error in parsing pasv\n");
        return -1;
    }
    
    sprintf(ip, "%u.%u.%u.%u", ip1, ip2, ip3, ip4);
    *porta = p1*256 + p2;
    
    return 0;
}

