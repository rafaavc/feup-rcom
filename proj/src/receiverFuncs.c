#include "receiverFuncs.h"

unsigned establishLogicConnection(int fd) {
    //unsigned valid = FALSE, counter = 0;
    char* ret;
    char* buf = NULL;
    ssize_t res, size;
    buf = constructSupervisionMessage(ADDR_SENT_EM, CTRL_UA);

    enum stateMachine state;

    //tries to read the message back from the serialPort
    ret = readFromSP(fd, &state, &size, ADDR_SENT_EM, CTRL_SET);
    //printf("ret: %s, size: %ld\n", ret, size);
    //writes to the serial port, trying to connect
    if (isAcceptanceState(&state))
        res = writeToSP(fd, buf, SUPERVISION_MESSAGE_SIZE);
    else return FALSE;

    return TRUE;
}

