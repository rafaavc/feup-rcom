#include "iPA.h"

char role;
struct termios oldtio;

int llopen(int porta, char r){
    char portString[12];
    sprintf(portString, "/dev/ttyS%d", porta);
    int fd = openConfigureSP(portString, &oldtio);
    role = r;
    if(role == RECEIVER){
        //receiverLoop();//??
    }
    else if (role == EMITTER){
        if (!establishLogicConnection()) {
            return -1;
        }
    } else return -1;
    return fd;
}

int llwrite(int fd, char * buffer, int length){
    if (role == EMITTER) {
        if (!sendMessage(buffer, length)) {
            return 1;
        }
    } else return 1;
    return 0;
}

int llread(int fd, char * buffer){
    if (role == RECEIVER) {

        ssize_t size;
        enum stateMachine state;
        enum readFromSPRet res;
        /* 
        Tem de continuar a ler enquanto não ler 
        uma trama de informação (E acionar as ações necessárias com as tramas de controlo)
        Retorna os dados da trama de informação
        */

        return receiverLoop(buffer); // -1 se erro, > 0 quando lê
    }

    return 0;
} 

int llclose(int fd){
    if(role == EMITTER){
        establishDisconnection();
    }
    else if(role == RECEIVER){
        //maybe call receiver loop, but we have to make sure it only reads disc
    }
    else return -1;

    sleep(1);
    
    if(closeSP(&oldtio)!=0){
        return -1;
    }

    return 0;
}