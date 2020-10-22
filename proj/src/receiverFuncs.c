#include "receiverFuncs.h"

extern int fd;
extern enum programState progState;
int r = 1;

size_t receiverLoop(char * buffer) {
    ssize_t size;
    enum stateMachine state;
    enum readFromSPRet res;

    while (TRUE) {
        char ret[MAX_I_MSG_SIZE] = {'\0'};
        res = readFromSP(ret, &state, &size, ANY_VALUE, ANY_VALUE);
        if (res == READ_ERROR) return -1;
        // readFromSP only return acceptance state
        char buf[SUPERVISION_MSG_SIZE];
        if (isI(&state)) {
            if(res == REJ){
                constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_REJ(r));
            }
            else{
                if(res == SAVE){
                    size_t dataLength = size - 6;
                    memcpy(buffer, &buffer[BCC1_IDX+1], dataLength);
                    return dataLength;
                }
                constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_RR(r));
                r++;
            }
            writeToSP(buf, SUPERVISION_MSG_SIZE);
            printf("RECEIVED: ");
            for (int i = BCC1_IDX+1; i < size-2; i++) {
                printf("%c", ret[i]);
            }
            printf("\n");
        } else {
            return -1;
        }
    }
}


void receiverConnecting() {
    ssize_t size;
    enum stateMachine state;
    char buf[SUPERVISION_MSG_SIZE];
    while (TRUE) {
        char ret[MAX_I_MSG_SIZE] = {'\0'};
        readFromSP(ret, &state, &size, ADDR_SENT_EM, CTRL_SET);
        //printCharArray(ret, size);
        if (isAcceptanceState(&state)) {
            debugMessage("RECEIVED SET");
            constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_UA);
            writeToSP(buf, SUPERVISION_MSG_SIZE);
            progState = WaitingForDISC;
        }        
    }
    //return 0;
}


size_t receiverDisconnecting(){
    ssize_t size;
    enum stateMachine state;
    char buf[SUPERVISION_MSG_SIZE];

    //printCharArray(ret, size);
    while (TRUE) {
        char ret[MAX_I_MSG_SIZE] = {'\0'};
        readFromSP(ret, &state, &size, ADDR_SENT_EM, CTRL_DISC);

        if (isAcceptanceState(&state)) {
            debugMessage("RECEIVED DISC");
            constructSupervisionMessage(buf, ADDR_SENT_RCV, CTRL_DISC);
            writeToSP(buf, SUPERVISION_MSG_SIZE);
        }
        else return -1;

        readFromSP(ret, &state, &size, ADDR_SENT_RCV, CTRL_UA);

        if (isAcceptanceState(&state)) {
            debugMessage("RECEIVED UA");
            return 0;
        }
    }
    return -1;

}





        