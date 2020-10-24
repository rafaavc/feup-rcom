#include "iPA.h"

char role;
struct termios oldtio;

int llopen(int porta, char r){
    char portString[12];

    sprintf(portString, "/dev/ttyS%d", porta);
    
    int fd = openConfigureSP(portString, &oldtio); 

    if (fd == -1) return -1;

    role = r;
    if(role == RECEIVER){
        if (!receiverConnecting()) return -1; //establishing connection with EMITTER
    }
    else if (role == EMITTER){
        if (signal(SIGALRM, alarmHandler) < 0) {  // Instals the handler for the alarm interruption
            perror("Alarm handler wasn't installed"); 
            exit(EXIT_FAILURE);
        }
        if (!establishLogicConnection()) return -1; //establishing connection with RECEIVER
    } 
    else return -1;

    return fd;
}

int llwrite(int fd, char * buffer, int length){
    char msg[MAX_I_BUFFER_SIZE] = {'\0'};
    size_t s = length;
    
    constructInformationMessage(msg, buffer, &s);

    int res = sendMessage(msg,s);

    if (res < 0) return -1;// in case of error
    
    return res;
}

int llread(int fd, char * buffer){
    return receiverLoop(buffer);
} 

int llclose(int fd){
    if(role == EMITTER){
        if(!establishDisconnection()) //disconnecting in the EMITTER side 
            return -1;
    }
    else if(role == RECEIVER){ //disconnecting in the RECEIVER side 
       if(receiverDisconnecting() == -1)//if there was an error sending/receiving the disconnection messages 
            return -1;;
    }
    else return -1;

    sleep(1);
    
    if(closeSP(&oldtio)!=0){
        return -1;
    }

    return 0;
}