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


    //tries to read the message back from the serialPort

    ret = readFromSP(fd, &size,0);

    //writes to the serial port, trying to connect
    res = writeToSP(fd,buf,SUPERVISION_MESSAGE_SIZE);
    
    //verifies if it was written correctly
 
    printf("Connected successfully!\n");
    
    
    sleep(1);

    closeSP(fd, &oldtio);

    return 0;
}
/*
Nota: falta tratamentos de erro

*/
