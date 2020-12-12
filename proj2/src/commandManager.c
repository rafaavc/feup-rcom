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


int readReply(int socketFD, unsigned * replyCode, char * reply) {
    FILE* socket = fdopen(socketFD, "r");
    
    if (socket == NULL) {
        return -1;
    }


    //*reply = malloc(REPLY_SIZE*sizeof(char));
    size_t replySize = 0;
    char* buf = NULL;
    printf("# Reading reply...\n");

    while(getline(&buf, &replySize, socket) > 0){
        printf("%s", buf);

        if(buf[3] == ' ') { // reply code has been read
            if (reply != NULL) strcpy(reply, buf);
            buf[3] = '\0';
            sscanf(buf,"%u", replyCode);
            break;
        }
    }
    return 0;
}