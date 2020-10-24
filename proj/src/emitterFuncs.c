#include "emitterFuncs.h"

extern unsigned stopAndWaitFlag;
extern int fd;
extern int nextS;
extern enum stateMachine state;
bool rejFlag = FALSE;

bool stopAndWait(unsigned (*functionToExec)(char*,size_t, size_t*), char * msgToWrite, size_t msgSize, size_t  *res) {
    unsigned counter = 0;

    #ifdef DEBUG
    struct myTimer timer;
    initTimer(&timer, "SIGALARM");
    #endif

    stopAndWaitFlag = TRUE;
    
    while(counter < NO_TRIES) {
        if(stopAndWaitFlag) {
            stopAndWaitFlag = FALSE;
            rejFlag = FALSE;

            counter++;

            alarm(TIME_OUT);
            //printf("sending message with s = %d and size = %d\n", nextS, msgSize);

            #ifdef DEBUG
            stopTimer(&timer, TRUE);
            startTimer(&timer);
            #endif
            if (functionToExec(msgToWrite, msgSize, res)) { // NEED TO FIND WAY TO HANDLE ERRORS IN THIS FUNCTION
                alarm(0); // unset alarm
                return TRUE;
            }
            /* REJ is a retransmission request, so everytime it receives a REJ resends the data
            NO_TRIES is only for retransmission tries due to timeout*/
            if (rejFlag) counter--; 
        }
    }

    printError("Error sending message (tried %d times with no/invalid response)\n", NO_TRIES);
    alarm(0);

    exit(EXIT_FAILURE);
}    

bool logicConnectionFunction(char * msg, size_t msgSize, size_t *res ) {
    ssize_t size; 
    char ret[MAX_I_MSG_SIZE];

    *res = writeToSP(msg, SUPERVISION_MSG_SIZE);
    if (*res == -1) {
        perror("Error writing to SP");
        return FALSE;
    } else if (*res != msgSize) { //verifies if it was written correctly
        printError("Error while writing to SP (size doesn't match)\n");
        return FALSE;
    }
    
    enum stateMachine state;
    if (readFromSP(ret, &state, &size, ADDR_SENT_EM, CTRL_UA) == READ_ERROR) return FALSE;

    if(isAcceptanceState(&state)) {
        debugMessage("[LOGIC CONNECTION] SUCCESS");
        return TRUE;
    }
    return FALSE;
}

bool establishLogicConnection() {
    char buf[SUPERVISION_MSG_SIZE]; // the message to send
    size_t res; 

    constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_SET);
    
    return stopAndWait(&logicConnectionFunction, buf, SUPERVISION_MSG_SIZE, &res); // writes to the serial port, trying to connect
}


bool disconnectionFunction(char * msg, size_t msgSize, size_t *res ) {
    ssize_t size; 
    char ret[MAX_I_MSG_SIZE];

    //writes to serial port, trying to connect

    *res = writeToSP(msg, SUPERVISION_MSG_SIZE);//write DISC
    
    //verifies if it was written correctly
    if (*res == -1) {
        perror("Error writing to SP");
        return FALSE;
    } 
    else if (*res != msgSize) { 
        printError("Error while writing to SP (size doesn't match)\n");
        return FALSE;
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
    if (stopAndWait(&disconnectionFunction, buf, SUPERVISION_MSG_SIZE, &res)) {
        constructSupervisionMessage(buf, ADDR_SENT_RCV, CTRL_UA);
        writeToSP(buf, SUPERVISION_MSG_SIZE); //write UA
        debugMessage("[DISC] SUCCESS");
        return TRUE;
    }
    
    return FALSE;
}

bool informationExchange(char* msg, size_t msgSize, size_t *res ){
    //need to check errors
    ssize_t size; 
    char ret[MAX_I_MSG_SIZE];
    
    *res = writeToSP(msg, msgSize);
    if (*res == -1) {
        perror("Error writing to SP");
        return FALSE;
    } else if (*res != msgSize) { //verifies if it was written correctly
        printError("Error while writing to SP (size doesn't match)\n");
        return FALSE;
    }
    
    enum stateMachine state;
    enum readFromSPRet result;
    result = readFromSP(ret, &state, &size, ADDR_SENT_EM, ANY_VALUE);
    //if (result == READ_ERROR) return FALSE;
    
    if(!isAcceptanceState(&state)) {
        debugMessage("[SENDING DATA] WRONG MESSAGE RECEIVED\n");
        return FALSE;
    }
    if(result == REJ){
        debugMessage("Received REJ\n");
        stopAndWaitFlag = TRUE;
        rejFlag = TRUE;
        return FALSE;
    }
    
    return TRUE;
}

//Rej reenvia, Rr termina alarme e envia a proxima

bool sendMessage(char* msg, size_t msgSize) {
    size_t bytesWritten;
    if (!stopAndWait(&informationExchange, msg, msgSize, &bytesWritten)) return -1;
    return bytesWritten;
}


