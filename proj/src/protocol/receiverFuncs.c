#include "receiverFuncs.h"

extern int fd;

size_t receiverLoop(char * buffer) {
    ssize_t size;
    enum stateMachine state;
    enum readFromSPRet res;

    while (true) { // acho que este while pode ser removido
        char * ret = myMalloc(MAX_I_MSG_SIZE);


        res = readFromSP(ret, &state, &size, ANY_VALUE, ANY_VALUE);

        if (res == READ_ERROR) return -1;
        // readFromSP only returns acceptance state
        char *buf = (char*)myMalloc(SUPERVISION_MSG_SIZE*sizeof(char));

        // checks if it read from a valid information message
        // OR has an RR or REJ to send (SAVE only happens if msg is valid, so it is implicit)
        // otherwise an error occurred
        if (isI(&state) || res == RR || res == REJ) { 
            //printf("Received I\n");
            int s = getS(ret[CTRL_IDX]);
            if(res == REJ) {
                //debugMessage("Sending REJ");
                constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_REJ((s+1)%2));
                writeToSP(buf, SUPERVISION_MSG_SIZE);
                free(buf);
            }
            else { // SAVE or RR
                //debugMessage("Sending RR");
                constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_RR((s+1)%2));
                writeToSP(buf, SUPERVISION_MSG_SIZE);
                free(buf);
                if(res == SAVE) {
                    //debugMessage("Saving Message");
                    //printf("Saving message, size: %ld\n", size);
                    size_t dataLength = size - 6;
                    memcpy(buffer, &ret[BCC1_IDX+1], dataLength);
                    free(ret);
                    return dataLength;
                }
            }
            free(ret);
            // (REJ == STOPPED_OR_SU)
        } else { // in this case it means that we received an SU msg
            printError("Received invalid data (SU message) while running llread. Protocol reader state: %u\n", state);
            free(ret);
            return -1;
        }
    }
}


bool receiverConnecting() {
    ssize_t size;
    enum stateMachine state;
    char *buf = (char*)myMalloc(SUPERVISION_MSG_SIZE*sizeof(char));

    while (true) {
        char * ret = myMalloc(MAX_I_MSG_SIZE);
        if (readFromSP(ret, &state, &size, ADDR_SENT_EM, CTRL_SET) == READ_ERROR) break;
        debugMessage("RECEIVED SET");
        free(ret);
        constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_UA);
        writeToSP(buf, SUPERVISION_MSG_SIZE);
        free(buf);
        return true;
    }
    return false;
}


size_t receiverDisconnecting(){
    ssize_t size;
    enum stateMachine state;
    char *buf = (char*)myMalloc(SUPERVISION_MSG_SIZE*sizeof(char));

    while (true) {
        char * ret = myMalloc(MAX_I_MSG_SIZE);
        if (readFromSP(ret, &state, &size, ADDR_SENT_EM, CTRL_DISC) == READ_ERROR) return -1;

        if (isAcceptanceState(&state)) {
            debugMessage("RECEIVED DISC");
            constructSupervisionMessage(buf, ADDR_SENT_RCV, CTRL_DISC);
            writeToSP(buf, SUPERVISION_MSG_SIZE);
            free(buf);
        }
        else{
            free(buf);
            return -1;
        } 

        if (readFromSP(ret, &state, &size, ADDR_SENT_RCV, CTRL_UA) == READ_ERROR) return -1;

        free(ret);
        if (isAcceptanceState(&state)) {
            debugMessage("RECEIVED UA");
            return 0;
        }
    }
    return -1;

}





        