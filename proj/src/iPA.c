#include "iPA.h"

char role;
struct termios oldtio;
extern int fd;


int llopen(int porta, char r){
    char portString[12];

    sprintf(portString, "/dev/ttyS%d", porta);
    openConfigureSP(portString, &oldtio);

    role = r;
   
    if(role == RECEIVER){
        //receiverLoop();//??
    }
    else if (role == EMITTER){
        if (!establishLogicConnection()) {
            return -1;
        }
    } 
    else return -1;

    return fd;
}

int llwrite(int fd, char * buffer, int length){
    int res = sendMessage(buffer,length);
    if (res < 0) {//retorna valor negativo em caso de erro
        return -1;
    }
    
    return res;//numero de carateres escritos em caso de sucesso
}

/*
comprimento do array(número de caracteres lidos)
valor negativo em caso de erro
*/

int llread(int fd, char * buffer){
        ssize_t size;
        enum stateMachine state;
        enum readFromSPRet res;
        /* 
        Tem de continuar a ler enquanto não ler 
        uma trama de informação (E acionar as ações necessárias com as tramas de controlo)
        Retorna os dados da trama de informação
        */
        int res = receiverLoop(buffer);
        if(res >0)// retorna o numero de careteres lido
            return res;
        return  -1;// -1 se erro
} 

int llclose(int fd){
    if(role == EMITTER){
        if(!establishDisconnection())
            return -1;
    }
    else if(role == RECEIVER){
        //maybe call receiver loop, but we have to make sure it only reads disc
        //condiçao de erro retorna -1
        receiverDisconnecting();//que se ocorrer erro retorna -1 caso contrario 0
    }
    else return -1;

    sleep(1);
    
    if(closeSP(&oldtio)!=0){
        return -1;
    }

    return 0;
}