#include "connection.h"


char *getIP(char* hostname){
    struct hostent *h; 

    if ((h = gethostbyname(hostname)) == NULL) {  
        herror("Error getting host by name");
        return NULL;
    }

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

    return inet_ntoa(*((struct in_addr *)h->h_addr));
}


int strneedle(char * haystack, char needle) {
    for (int i = 0; i < strlen(haystack); i++) {
        if (haystack[i] == needle) return i;
    }
    return -1;
}

int parseURL(char *url, char **user, char **password, char **host, char **urlPath) {
    char * urlBegin = "ftp://";
    if (strncmp(urlBegin, url, 6) != 0) {
        fprintf(stderr, "The first six characters aren't valid. Should be '%s'.\n", urlBegin);
        return 1;
    }

    int atPosition = strneedle(url, '@');
    if (atPosition != -1) {
        char * aux = malloc((atPosition - strlen(urlBegin)) * sizeof(char));
        strncpy(aux, url + strlen(urlBegin), atPosition - strlen(urlBegin));

        int colonPosition = strneedle(aux, ':');

        *user = malloc(colonPosition * sizeof(char));


    printf("%s\n", *urlPath);
        strncpy(*user, aux, colonPosition);

        *password = malloc((strlen(aux) - colonPosition -1)*sizeof(char));
        strncpy(*password, aux + colonPosition+1, strlen(aux) - colonPosition -1);

        free(aux);
        
    }

    int hostStart = atPosition != -1 ? atPosition +1 : strlen(urlBegin);
    int aux, forwardSlashPosition = (aux = strneedle(url + hostStart, '/'), aux != -1 ? aux+hostStart : -1);

    *host = malloc((forwardSlashPosition - hostStart)*sizeof(char));
    strncpy(*host, url + hostStart, forwardSlashPosition - hostStart);

    *urlPath = malloc((strlen(url) - forwardSlashPosition-1)*sizeof(char));
    strncpy(*urlPath, url + forwardSlashPosition + 1, strlen(url) - forwardSlashPosition-1);

    return 0;
}


/*
int loginHost(int fdSocket, char *user, char *password){
    //check if there is a user


    //user command
    char *userCommand = malloc(sizeof(char)* (strlen(user)+5));
    userCommand[0] = '\0';
    strcat(userCommand, "user ");
    strcat(userCommand,user);

    //password command
    char *passwordCommand = malloc(sizeof(char)*(strlen(password)+9));
    passwordCommand[0] = '\0';
    strcat(passwordCommand, "password ");
    strcat(passwordCommand, password);
    

    //send user command -- check if sent correctly
    printf("User command: %s\n", userCommand);
    free(userCommand);
    //read command answer -- check if read correctly

    //send password command -- check if sent correctly
    printf("Password command: %s \n", passwordCommand);
    free(passwordCommand);
    //read command answer -- check if read correctly


    return 0;
}



int passiveMode(int socket){
    char *pasvCommand = malloc(sizeof(char)* (strlen("pasv")+2));
    sprintf(pasvCommand, "pasv \n");
    

    //send pasv command -- check if sent correctly

    //read response < 227 Entering Passive Mode (193,136,28,12,19,91)



    return 0;
}*/