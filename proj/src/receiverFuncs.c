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

        // readFromSP only returns acceptance state
        char buf[SUPERVISION_MSG_SIZE];
        if (isI(&state)) {
            if(res == REJ) {
                debugMessage("Sending REJ");
                constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_REJ(r));
                writeToSP(buf, SUPERVISION_MSG_SIZE);
            }
            else {
                debugMessage("Sending RR");
                constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_RR(r));
                writeToSP(buf, SUPERVISION_MSG_SIZE);
                r++;
                if(res == SAVE) {
                    size_t dataLength = size - 6;
                    memcpy(buffer, &ret[BCC1_IDX+1], dataLength);
                    return dataLength;
                }
            }
            writeToSP(buf, SUPERVISION_MSG_SIZE);
        } else {
            return -1;
        }
    }
}


bool receiverConnecting() {
    ssize_t size;
    enum stateMachine state;
    char buf[SUPERVISION_MSG_SIZE];

    while (TRUE) {
        char ret[MAX_I_MSG_SIZE] = {'\0'};
        if (readFromSP(ret, &state, &size, ADDR_SENT_EM, CTRL_SET) == READ_ERROR) break;
        //printCharArray(ret, size);
        debugMessage("RECEIVED SET");
        constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_UA);
        writeToSP(buf, SUPERVISION_MSG_SIZE);
        return TRUE;
    }
    return FALSE;
}


size_t receiverDisconnecting(){
    ssize_t size;
    enum stateMachine state;
    char buf[SUPERVISION_MSG_SIZE];

    //printCharArray(ret, size);
    while (TRUE) {
        char ret[MAX_I_MSG_SIZE] = {'\0'};
        if (readFromSP(ret, &state, &size, ADDR_SENT_EM, CTRL_DISC) == READ_ERROR) return -1;

        if (isAcceptanceState(&state)) {
            debugMessage("RECEIVED DISC");
            constructSupervisionMessage(buf, ADDR_SENT_RCV, CTRL_DISC);
            writeToSP(buf, SUPERVISION_MSG_SIZE);
        }
        else return -1;

        if (readFromSP(ret, &state, &size, ADDR_SENT_RCV, CTRL_UA) == READ_ERROR) return -1;

        if (isAcceptanceState(&state)) {
            debugMessage("RECEIVED UA");
            return 0;
        }
    }
    return -1;

}





        