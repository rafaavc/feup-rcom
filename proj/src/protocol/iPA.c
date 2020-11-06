#include "iPA.h"
#include "receiverFuncs.h"

char role;

int llopen(int porta, char * r){
    if (strcmp(r, "RECEIVER") == 0) role = RECEIVER;
    else if (strcmp(r, "TRANSMITTER") == 0) role = TRANSMITTER;
    else {
        printError("Invalid role given to llopen\n");
        return -1;
    }

    char portString[12];

    sprintf(portString, "/dev/ttyS%d", porta);
    
    int fd = openConfigureSP(portString); 

    if (fd == -1) return -1;
    
    if(role == RECEIVER){
        if (!receiverConnect()) return -1; //establishing connection with TRANSMITTER
    }
    else if (role == TRANSMITTER){
        if (signal(SIGALRM, alarmHandler) < 0) {  // Instals the handler for the alarm interruption
            perror("Alarm handler wasn't installed"); 
            exit(EXIT_FAILURE);
        }
        if (!transmitterConnect()) return -1; //establishing connection with RECEIVER
    } 
    else return -1;

    return fd;
}

int llwrite(int fd, char * buffer, int length){
    char * msg = myMalloc(MAX_I_BUFFER_SIZE);
    bzero(msg, MAX_I_BUFFER_SIZE);
    size_t s = length;
    
    constructInformationMessage(msg, buffer, &s);

    int res = transmitterWrite(msg,s);
    free(msg);
    if (res < 0) return -1;// in case of error
    
    return res;
}

int llread(int fd, char * buffer){
    return receiverRead(buffer);
} 

int llclose(int fd){
    if(role == TRANSMITTER){
        if(!transmitterDisconnect()) //disconnecting in the TRANSMITTER side 
            return -1;
    }
    else if(role == RECEIVER){ //disconnecting in the RECEIVER side 
       if(receiverDisconnect() == -1)//if there was an error sending/receiving the disconnection messages 
            return -1;;
    }
    else return -1;

    sleep(1);
    
    if(closeSP() != 0){ 
        return -1;
    }

    return 0;
}