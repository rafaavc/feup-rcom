#include "transmitterFuncs.h"

extern unsigned stopAndWaitFlag;
extern int fd;
extern int nextS;
extern enum stateMachine state;
bool rejFlag = false;

bool stopAndWait(bool (*functionToExec)(char*,size_t, size_t*), char * msgToWrite, size_t msgSize, size_t  *res) {
    unsigned counter = 0;

    #ifdef DEBUG
    struct myTimer timer;
    initTimer(&timer, "SIGALARM");
    #endif

    stopAndWaitFlag = true;   // used by the alarm
    
    while(counter < NO_TRIES) { // sends the message NO_TRIES times
        if(stopAndWaitFlag) {
            stopAndWaitFlag = false;
            rejFlag = false;

            counter++;

            alarm(TIME_OUT);
            //printf("sending message with s = %d and size = %d\n", nextS, msgSize);

            #ifdef DEBUG
            stopTimer(&timer, true);
            startTimer(&timer);
            #endif
            if (functionToExec(msgToWrite, msgSize, res)) { // RR, sends the next one
                alarm(0); // unset alarm
                return true;
            }
            /* REJ is a retransmission request, so everytime it receives a REJ resends the data
            NO_TRIES is only for retransmission tries due to timeout*/
            if (rejFlag) counter--; 
        }
    }

    printError("Error sending message (tried %d times with no/invalid response)\n", NO_TRIES);
    printCharArray(msgToWrite, msgSize);
    alarm(0);

    exit(EXIT_FAILURE);
}    

bool logicConnectionFunction(char * msg, size_t msgSize, size_t *res ) {
    ssize_t size; 
    char *ret = (char*)myMalloc(MAX_I_MSG_SIZE*sizeof(char));

    *res = writeToSP(msg, SUPERVISION_MSG_SIZE);
    if (*res == -1) {
        perror("Error writing to SP");
        return false;
    } else if (*res != msgSize) { //verifies if it was written correctly
        printError("Error while writing to SP (size doesn't match)\n");
        return false;
    }
    
    enum stateMachine state;
    if (readFromSP(ret, &state, &size, ADDR_SENT_EM, CTRL_UA) == READ_ERROR) return false;

    free(ret);

    if(isAcceptanceState(&state)) {
        debugMessage("[LOGIC CONNECTION] SUCCESS");
        return true;
    }
    return false;
}

bool establishLogicConnection() {
    char *buf = (char*)myMalloc(sizeof(char)*SUPERVISION_MSG_SIZE); // the message to send
    size_t res; 

    constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_SET);
    
    return stopAndWait(&logicConnectionFunction, buf, SUPERVISION_MSG_SIZE, &res); // writes to the serial port, trying to connect
}


bool disconnectionFunction(char * msg, size_t msgSize, size_t *res ) {
    ssize_t size; 
    char *ret = (char*)myMalloc(MAX_I_MSG_SIZE*sizeof(char));

    //writes to serial port, trying to connect

    *res = writeToSP(msg, SUPERVISION_MSG_SIZE);//write DISC
    
    //verifies if it was written correctly
    if (*res == -1) {
        perror("Error writing to SP");
        return false;
    } 
    else if (*res != msgSize) { 
        printError("Error while writing to SP (size doesn't match)\n");
        return false;
    }
    
    enum stateMachine state;

    //tries to read the message back from the serialPort
    readFromSP(ret, &state, &size, ADDR_SENT_RCV, CTRL_DISC);//read DISC

    free(ret);
    if(isAcceptanceState(&state))
        return true;

    return false;
}

bool establishDisconnection() {
    char *buf = (char*) myMalloc(SUPERVISION_MSG_SIZE*sizeof(char));
    size_t res;
    constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_DISC);
    if (stopAndWait(&disconnectionFunction, buf, SUPERVISION_MSG_SIZE, &res)) {
        constructSupervisionMessage(buf, ADDR_SENT_RCV, CTRL_UA);
        writeToSP(buf, SUPERVISION_MSG_SIZE); //write UA
        free(buf);
        debugMessage("[DISC] SUCCESS");
        return true;
    }
    
    return false;
}

bool informationExchange(char* msg, size_t msgSize, size_t *res ){
    ssize_t size; 
    char *ret = (char*)myMalloc(MAX_I_MSG_SIZE*sizeof(char));
    
    *res = writeToSP(msg, msgSize);
    if (*res == -1) {
        perror("Error writing to SP");
        return false;
    } else if (*res != msgSize) { //verifies if it was written correctly
        printError("Error while writing to SP (size doesn't match)\n");
        return false;
    }
    
    enum stateMachine state;
    enum readFromSPRet result;
    result = readFromSP(ret, &state, &size, ADDR_SENT_EM, ANY_VALUE);
    /*printf("Returned message: ");
    fwrite(ret, sizeof(char), size, stdout);
    printf("\n");*/
    if (result == READ_ERROR) return false;
    
    free(ret);

    if(!isAcceptanceState(&state)) {
        debugMessage("[SENDING DATA] WRONG MESSAGE RECEIVED\n");
        return false;
    }
    if(result == REJ){
        debugMessage("Received REJ\n");
        stopAndWaitFlag = true;
        rejFlag = true;
        return false;
    }
    
    return true;
}

size_t sendMessage(char* msg, size_t msgSize) {
    size_t bytesWritten;
    if (!stopAndWait(&informationExchange, msg, msgSize, &bytesWritten)) return -1;
    return bytesWritten;
}


