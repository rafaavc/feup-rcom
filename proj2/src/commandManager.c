#include "commandManager.h"

int sendCommand(int socket, char* cmd){
    size_t cmdLength = strlen(cmd)+1;
    char * aux = calloc(cmdLength, sizeof(char));
    
    strcat(aux, cmd);
    strcat(aux, "\n"); // to add the endline in the end of the command
    if(write(socket,aux,cmdLength) != cmdLength){
        fprintf(stderr, "Error sending command %s\n", cmd);
        free(aux);
        return 1;
    }

    free(aux);

    return 0;
}


int readReply(int socketFD, char ** reply) {
    *reply = malloc(REPLY_SIZE*sizeof(char));
    printf("read reply 1\n");
    if (read(socketFD, *reply, REPLY_SIZE) <= 0) {
        perror("readReply > read()");
        return 1;
    }
    printf("%s", *reply);
    return 0;
}