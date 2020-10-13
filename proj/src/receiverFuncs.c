#include "receiverFuncs.h"

unsigned establishLogicConnection(int fd) {
    //unsigned valid = FALSE, counter = 0;
    char* ret;
    char* buf = NULL;
    ssize_t res, size;
    buf = constructSupervisionMessage(ADDR_SENT_RCV, CTRL_UA);


    //tries to read the message back from the serialPort
    ret = readFromSP(fd, &size,0);
    
    //writes to the serial port, trying to connect
    res = writeToSP(fd,buf,SUPERVISION_MESSAGE_SIZE);

    return TRUE;
}

