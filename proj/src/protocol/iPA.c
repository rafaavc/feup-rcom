#include "iPA.h"
#include "receiverFuncs.h"

char role;
struct termios oldtio;

int llopen(int porta, char * r){
    if (strcmp(r, "RECEIVER") == 0) role = RECEIVER;
    else if (strcmp(r, "TRANSMITTER") == 0) role = TRANSMITTER;
    else {
        printError("Invalid role given to llopen\n");
        return -1;
    }

    char portString[12];

    sprintf(portString, "/dev/ttyS%d", porta);
    
    int fd = openConfigureSP(portString, &oldtio); 

    if (fd == -1) return -1;
    
    if(role == RECEIVER){
        if (!receiverConnecting()) return -1; //establishing connection with TRANSMITTER
    }
    else if (role == TRANSMITTER){
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

    int res = 0;

    size_t lengthLeft = length;
    unsigned bufferPosition = 0;
    while (lengthLeft > 0) {
        size_t s;
        unsigned nextBufferPosition = 0;
        if (lengthLeft > MAX_DATA_LENGTH) {
            s = MAX_DATA_LENGTH;
            nextBufferPosition = MAX_DATA_LENGTH;
        } else {
            s = lengthLeft;
        }
        lengthLeft -= s;
        res += s; // adds the size to ret (unstuffed)
        constructInformationMessage(msg, &buffer[bufferPosition], &s);

        int sendMessageRet = sendMessage(msg,s);

        if (sendMessageRet < 0) return -1;// in case of error
        else if (s != sendMessageRet) {
            printError("Didn't write the intended data amount. Intended: %d, written: %d\n", (int) s, sendMessageRet);
            return -1;
        }

        bufferPosition = nextBufferPosition;
        bzero(msg, MAX_I_BUFFER_SIZE);
    }
    
    
    return res;
}

int llread(int fd, char * buffer){
    return receiverLoop(buffer);
} 

int llclose(int fd){
    if(role == TRANSMITTER){
        if(!establishDisconnection()) //disconnecting in the TRANSMITTER side 
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