#include "receiverFuncs.h"

extern int fd;

size_t receiverLoop(char * buffer) {
    ssize_t size;
    enum stateMachine state;
    enum readFromSPRet res;

    while (true) {
        char ret[MAX_I_MSG_SIZE] = {'\0'};

        res = readFromSP(ret, &state, &size, ANY_VALUE, ANY_VALUE);
        if (res == READ_ERROR) return -1;
        // readFromSP only returns acceptance state
        char buf[SUPERVISION_MSG_SIZE];

        if (isI(&state)) { // checks if it read from an information message, otherwise an error occurred
            //printf("Received I\n");
            int s = getS(ret[CTRL_IDX]);
            if(res == REJ) {
                //debugMessage("Sending REJ");
                constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_REJ((s+1)%2));
                writeToSP(buf, SUPERVISION_MSG_SIZE);
            }
            else { // SAVE or RR
                //debugMessage("Sending RR");
                constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_RR((s+1)%2));
                writeToSP(buf, SUPERVISION_MSG_SIZE);
                if(res == SAVE) {
                    size_t dataLength = size - 6;
                    memcpy(buffer, &ret[BCC1_IDX+1], dataLength);
                    return dataLength;
                }
            }
        } else {

            printError("Received invalid data while running llread. Protocol reader state: %u\n", state);
            return -1;
        }
    }
}


bool receiverConnecting() {
    ssize_t size;
    enum stateMachine state;
    char buf[SUPERVISION_MSG_SIZE];

    while (true) {
        char ret[MAX_I_MSG_SIZE] = {'\0'};
        if (readFromSP(ret, &state, &size, ADDR_SENT_EM, CTRL_SET) == READ_ERROR) break;
        debugMessage("RECEIVED SET");
        constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_UA);
        writeToSP(buf, SUPERVISION_MSG_SIZE);
        return true;
    }
    return false;
}


size_t receiverDisconnecting(){
    ssize_t size;
    enum stateMachine state;
    char buf[SUPERVISION_MSG_SIZE];

    while (true) {
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





        