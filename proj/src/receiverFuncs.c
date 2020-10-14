#include "receiverFuncs.h"

unsigned establishLogicConnection(int fd) {
    //unsigned valid = FALSE, counter = 0;
    char buf[SUPERVISION_MSG_SIZE], ret[MAX_I_MSG_SIZE];
    ssize_t res, size;
    constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_UA);

    enum stateMachine state;

    //tries to read the message back from the serialPort
    readFromSP(fd, ret, &state, &size, ADDR_SENT_EM, CTRL_SET);
    
    //printf("ret: %s, size: %ld\n", ret, size);
    //writes to the serial port, trying to connect
    if (isAcceptanceState(&state))
        res = writeToSP(fd, buf, SUPERVISION_MSG_SIZE);
    else return FALSE;

    return TRUE;
}

