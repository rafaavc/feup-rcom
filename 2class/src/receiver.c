#include "protocol.h"

volatile int STOP=FALSE;

extern int logicConnectionFlag;

int main(int argc, char** argv){
    int fd, sum=0, speed=0;
    struct termios oldtio;

    logicConnectionFlag = FALSE;

    checkCmdArgs(argc, argv);


    fd = openConfigureSP(argv[1], &oldtio);
    

    unsigned valid = FALSE, counter = 0;
    char* ret;
    char* buf = NULL;
    ssize_t res, size;
    buf = constructSupervisionMessage(ADDR_SENT_RCV, CTRL_UA);

    //ligar alarme  
    //tries to read the message back from the serialPort
    ret = readFromSP(fd, &size);

    //verificar se recebeu e se é valida, caso nao reeenvia set enquanto nao for valida um maximo de 3 vezes
    if(size != (SUPERVISION_MESSAGE_SIZE + 1)){
        printf("Can't set connection1\n");
    }
     if(ret[2]!= CTRL_SET){
        printf("Can't set connection2\n");
    }
    // another condition for BCC
    //writes to the serial port, trying to connect
    res = writeToSP(fd,buf,SUPERVISION_MESSAGE_SIZE);
    
    //verifies if it was written correctly
    if(res != (SUPERVISION_MESSAGE_SIZE + 1)){
        return -1;
    }   

    printf("Connected successfully\n");
    
    sleep(1);

    closeSP(fd, &oldtio);

    return 0;
}
/*
Nota: falta tratamentos de erro

*/
