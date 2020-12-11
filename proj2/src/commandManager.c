#include "commandManager.h"

int sendCommand(int socket, char* cmd){
    size_t cmdLength = strlen(cmd)+2;
    char * aux = calloc(cmdLength, sizeof(char));
    
    strcat(aux, cmd);
    strcat(aux, "\r\n"); // to add the endline in the end of the command
    if(write(socket,aux,cmdLength) != cmdLength){
        fprintf(stderr, "Error sending command %s\n", cmd);
        free(aux);
        return 1;
    }

    free(aux);

    return 0;
}


int readReply(int socketFD, char ** reply) {
    FILE* socket = fdopen(socketFD, "r");
    
    if (socket == NULL) {
        return -1;
    }


    *reply = malloc(REPLY_SIZE*sizeof(char));
    size_t reply_size = 0;
    char* buf = NULL;
    printf("read reply 1\n");

    while(getline(&buf, &reply_size , socket) > 0){
        strncat(*reply, buf, reply_size-1);

        if(buf[3] == ' '){ // reply code has been read
            sscanf(buf,"%c", *reply);
            break;
        }
    }

    printf("%s", *reply);
    return 0;
}