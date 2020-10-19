#include "emitterFuncs.h"

extern unsigned stopAndWaitFlag;
extern int fd;
extern enum stateMachine state;

bool stopAndWait(unsigned (*functionToExec)(char*,size_t), char * msgToWrite, size_t msgSize) {
    unsigned counter = 0;

    #ifdef DEBUG
    struct myTimer timer;
    initTimer(&timer, "SIGALARM");
    #endif

    stopAndWaitFlag = TRUE;
    
    while(counter < NO_TRIES) {
        if(stopAndWaitFlag) {
            stopAndWaitFlag = FALSE;
            counter++;

            alarm(TIME_OUT);

            #ifdef DEBUG
            stopTimer(&timer, TRUE);
            startTimer(&timer);
            #endif
            if (functionToExec(msgToWrite,msgSize)) {
                alarm(0); // unset alarm
                return TRUE;
            }
        }
    }
    alarm(0);
    return FALSE;
}

bool logicConnectionFunction(char * msg, size_t msgSize) {
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
        debugMessage("[LOGIC CONNECTION] SUCCESS\n");
        return TRUE;
    }
    return FALSE;
}

bool establishLogicConnection() {
    // the message to send
    char buf[SUPERVISION_MSG_SIZE];

    constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_SET);
    
    // writes to the serial port, trying to connect
    return stopAndWait(&logicConnectionFunction, buf,SUPERVISION_MSG_SIZE);
}


bool disconnectionFunction(char * msg, size_t msgSize) {
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

bool establishDisconnection() {
    char buf[SUPERVISION_MSG_SIZE];

    constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_DISC);
    if (stopAndWait(&disconnectionFunction, buf,SUPERVISION_MSG_SIZE)) {
        constructSupervisionMessage(buf, ADDR_SENT_RCV, CTRL_UA);
        writeToSP(fd, buf, SUPERVISION_MSG_SIZE); //write UA
        debugMessage("[DISC] SUCCESS");
        return TRUE;
    }
    
    return FALSE;
}

bool informationExchange(char *msg, size_t msgSize){
    size_t res;
    ssize_t size; 
    char ret[MAX_I_MSG_SIZE];

    res = writeToSP(fd, msg, msgSize);

    //verifies if it was written correctly
    if (res != (msgSize + 1)) {
        printf("Wrong message size\n");
    }
    
    enum stateMachine state;
    readFromSP(fd, ret, &state, &size, ADDR_SENT_EM, ANY_VALUE);

    if(!isAcceptanceState(&state)) {
        debugMessage("[SENDING DATA] WRONG MESSAGE RECEIVED\n");
        return FALSE;
    }
    return TRUE;

}
