#include "emitterFuncs.h"

extern unsigned stopAndWaitFlag;
extern int fd;
extern enum stateMachine state;

bool stopAndWait(unsigned (*functionToExec)(char*,size_t), char * msgToWrite, size_t msgSize, size_t res) {
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
            if (functionToExec(msgToWrite, msgSize, &res)) {
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

    res = writeToSP(msg, SUPERVISION_MSG_SIZE);

    //verifies if it was written correctly
    if (res != SUPERVISION_MSG_SIZE) {
        printf("Wrong message size\n");
    }
    
    enum stateMachine state;
    readFromSP(ret, &state, &size, ADDR_SENT_EM, CTRL_UA);

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
    int res; 
    // writes to the serial port, trying to connect
    return stopAndWait(&logicConnectionFunction, buf,SUPERVISION_MSG_SIZE, &res);
}


bool disconnectionFunction(char * msg, size_t msgSize) {
    size_t res;
    ssize_t size; 
    char ret[MAX_I_MSG_SIZE];

    //writes to serial port, trying to connect
    res = writeToSP(msg, SUPERVISION_MSG_SIZE);//write DISC
    
    //verifies if it was written correctly
    if (res != SUPERVISION_MSG_SIZE) {
        printf("Wrong message size\n");
    }
    
    enum stateMachine state;

    //tries to read the message back from the serialPort
    readFromSP(ret, &state, &size, ADDR_SENT_RCV, CTRL_DISC);//read DISC

    if(isAcceptanceState(&state))
        return TRUE;

    return FALSE;
}

bool establishDisconnection() {
    char buf[SUPERVISION_MSG_SIZE];
    size_t res;
    constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_DISC);
    if (stopAndWait(&disconnectionFunction, buf,SUPERVISION_MSG_SIZE,&res)) {
        constructSupervisionMessage(buf, ADDR_SENT_RCV, CTRL_UA);
        writeToSP(buf, SUPERVISION_MSG_SIZE); //write UA
        debugMessage("[DISC] SUCCESS");
        return TRUE;
    }
    
    return FALSE;
}

bool informationExchange(char* msg, size_t msgSize, size_t *res ){
    ssize_t size; 
    char ret[MAX_I_MSG_SIZE];
    
    res = writeToSP(msg, msgSize);

    
    if (res != msgSize) { //verifies if it was written correctly
        printf("Wrong message size\n");
    }
    
    enum stateMachine state;
    enum readFromSPRet result;
    result = readFromSP(ret, &state, &size, ADDR_SENT_EM, ANY_VALUE);

    
    if(!isAcceptanceState(&state)) {
        debugMessage("[SENDING DATA] WRONG MESSAGE RECEIVED\n");
        return FALSE;
    }
    if(result == REJ){
        stopAndWaitFlag = TRUE;
        return FALSE;
    } 
    
    return TRUE;
}

//Rej reenvia, Rr termina alarme e envia a proxima

bool sendMessage(char* msg, size_t msgSize) {
    size_t bytesWritten;
    stopAndWait(&informationExchange, msg, msgSize, bytesWritten);
    if(bytesWritten < 0)
        return -1;
    return bytesWritten;
}

/*bool sendMessageArray(char ** msgs, size_t msgsSize) {
    for (int i = 0; i < msgSize; i++) {
        if (!sendMessage(msgs[i], ))
            return FALSE
    }
    return TRUE;
}*/


