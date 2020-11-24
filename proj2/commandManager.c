#include "commandManager.h"

int sendCommand(int socket, char* cmd){
    size_t cmdLength = strlen(cmd);

    if(write(socket,cmd,cmdLength)!= cmdLength){
        fprintf(stderr, "Error sending command %s\n", cmd);
        return 1;
    }

    //nao sei se temos que mandar o \n

    return 0;
}


int readReply(){


    return 0;
}