#include "receiverFuncs.h"

extern int fd;

int main(int argc, char** argv){
    // sum=0, speed=0;
    struct termios oldtio;

    checkCmdArgs(argc, argv);


    openConfigureSP(argv[1], &oldtio);


    //char* ret;
    //char * buf = NULL;
    //ssize_t res, size;
    //buf = constructSupervisionMessage(ADDR_SENT_RCV, CTRL_UA);

    /*state = Start;
    //tries to read the message back from the serialPort
    ret = readFromSP(fd, &size,0);
    printf("Read from sp\n");
    if(state == DONE) valid = TRUE;
    //writes to the serial port, trying to connect
    res = writeToSP(fd,buf,SUPERVISION_MESSAGE_SIZE);
    
    if(valid && res>0){*/
    if (!establishLogicConnection()) {
        printf("Wasn't able to establish logic connection!\n");
    }
    /*
    Reads the data, until it receives a disconnect, when this happens sends back a disconect and the program hands
    
    
    */
    
    sleep(1);

    closeSP(&oldtio);

    return 0;
}
/*
Nota: falta tratamentos de erro

*/
