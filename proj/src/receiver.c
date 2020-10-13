#include "protocol.h"

extern int logicConnectionFlag;
extern enum stateMachine state;

int main(int argc, char** argv){
    int fd;
    // sum=0, speed=0;
    struct termios oldtio;

    logicConnectionFlag = FALSE;

    checkCmdArgs(argc, argv);


    fd = openConfigureSP(argv[1], &oldtio);

    unsigned valid = FALSE;
// counter = 0;
    char* ret;
    char* buf = NULL;
    ssize_t res, size;
    buf = constructSupervisionMessage(ADDR_SENT_RCV, CTRL_UA);

    state = Start;
    //tries to read the message back from the serialPort
    ret = readFromSP(fd, &size,0);
    printf("Read from sp\n");
    if(state == DONE) valid = TRUE;
    //writes to the serial port, trying to connect
    res = writeToSP(fd,buf,SUPERVISION_MESSAGE_SIZE);
    
    if(valid && res>0){
        printf("Connected successfully!\n");
    }
    else {
        printf("no connection madwe \n");        
    }
   
 
    sleep(1);

    closeSP(fd, &oldtio);

    return 0;
}
/*
Nota: falta tratamentos de erro

*/
