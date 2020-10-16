#include "emitterFuncs.h"

extern unsigned stopAndWaitFlag;
extern int fd;
extern enum stateMachine state;

unsigned stopAndWait(unsigned (*functionToExec)(char*), char * msgToWrite) {
    unsigned counter = 0;

    #ifdef DEBUG
    struct myTimer timer;
    initTimer(&timer, "SIGALARM");
    #endif
    
    while(counter < NO_TRIES) {
        if(stopAndWaitFlag) {
            stopAndWaitFlag = FALSE;
            counter++;

            alarm(TIME_OUT);

            #ifdef DEBUG
            stopTimer(&timer, TRUE);
            startTimer(&timer);
            #endif

            if (functionToExec(msgToWrite))
                return TRUE;
        }
    }
    return FALSE;
}

unsigned logicConnectionFunction(char * msg) {
    size_t res;
    ssize_t size; 
    char ret[MAX_I_MSG_SIZE];

    res = writeToSP(fd, msg, SUPERVISION_MSG_SIZE);

    //verifies if it was written correctly
    if (res != (SUPERVISION_MSG_SIZE + 1)) {
        printf("Wrong message size\n");
    }
    
    enum stateMachine state;            
    readFromSP(fd, ret, &state, &size, ADDR_SENT_EM, CTRL_UA);

    if(isAcceptanceState(&state)) {
        debugMessage("[LOGIC CONNECTION] SUCCESS");
        return TRUE;
    }
    return FALSE;
}

unsigned establishLogicConnection() {
    // the message to send
    char buf[SUPERVISION_MSG_SIZE];

    constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_SET);
    
    // writes to the serial port, trying to connect
    return stopAndWait(&logicConnectionFunction, buf);
}


unsigned disconnectionFunction(char * msg) {
    size_t res;
    ssize_t size; 
    char ret[MAX_I_MSG_SIZE];

    //writes to serial port, trying to connect
    res = writeToSP(fd, msg, SUPERVISION_MSG_SIZE);//write DISC
    
    //verifies if it was written correctly
    if (res != (SUPERVISION_MSG_SIZE + 1)) {
        printf("Wrong message size\n");
    }
    
    enum stateMachine state;

    //tries to read the message back from the serialPort
    readFromSP(fd, ret, &state, &size, ADDR_SENT_RCV, CTRL_DISC);//read DISC

    if(isAcceptanceState(&state))
        return TRUE;

    return FALSE;
}

unsigned establishDisconnection() {
    char buf[SUPERVISION_MSG_SIZE];

    constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_DISC);
    
    if (stopAndWait(&disconnectionFunction, buf)) {
        constructSupervisionMessage(buf, ADDR_SENT_RCV, CTRL_UA);
        writeToSP(fd, buf, SUPERVISION_MSG_SIZE);//write UA
        return TRUE;
    }
    
    return FALSE;
}
