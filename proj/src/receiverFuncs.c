#include "receiverFuncs.h"

extern int fd;
extern enum programState progState;
static int r = 1;

bool dealWithReceivedMessage(enum stateMachine state, char * msg, size_t msgSize, enum readFromSPRet res) {
    char buf[SUPERVISION_MSG_SIZE];
    if (isI(&state)) {
        if(res == REJ){
            constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_REJ(r));
        }
        else{
            if(res == SAVE){
                //guardar dados
            }
            constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_RR(r));
            r++;
        }
        writeToSP(buf, SUPERVISION_MSG_SIZE);
        printf("RECEIVED: ");
        for (int i = BCC1_IDX+1; i < msgSize-2; i++) {
            printf("%c", msg[i]);
        }
        printf("\n");

    } 
    return FALSE;
}

size_t receiverLoop(char * buffer) {
    ssize_t size;
    enum stateMachine state;
    enum readFromSPRet res;

    while (TRUE) {
        char ret[MAX_I_MSG_SIZE] = {'\0'};
        res = readFromSP(ret, &state, &size, ANY_VALUE, ANY_VALUE);
        //printCharArray(ret, size);
        if (isAcceptanceState(&state)) {
            if (dealWithReceivedMessage(state, ret, size, res)) return -1; 
            // NEED TO RECHECK THIS!!!!!! if it receives SET or DISC while llread is happening, it's an error!
            if (isI(&state)) {
                if(size < 0)
                    return -1;
                size_t dataLength = size - 6;
                memcpy(buffer, &buffer[BCC1_IDX+1], dataLength);
                return dataLength;
            }
        }
    }
}

size_t receiverDisconnecting(){
    ssize_t size;
    enum stateMachine state;
    enum readFromSPRet res;
    char buf[SUPERVISION_MSG_SIZE];

    while (TRUE) {
        char ret[MAX_I_MSG_SIZE] = {'\0'};
        res = readFromSP(ret, &state, &size, ADDR_SENT_EM, CTRL_DISC);
        //printCharArray(ret, size);
        if (isAcceptanceState(&state)) {
            if(ret[CTRL_IDX] == CTRL_DISC){
                if (progState == WaitingForLC) break;

                debugMessage("RECEIVED DISC");
                constructSupervisionMessage(buf, ADDR_SENT_RCV, CTRL_DISC);
                writeToSP(buf, SUPERVISION_MSG_SIZE);
                progState = WaitingForDISC;
                
            }


        }
    }





    //debugMessage("RECEIVED «S,U» DATA");
     /*   if (progState == WaitingForDISC && msg[CTRL_IDX] != CTRL_UA) {
            progState = LogicallyConnected;
        }
        switch (msg[CTRL_IDX]) {

            case CTRL_DISC:
                if (progState == WaitingForLC) break;

                debugMessage("RECEIVED DISC");
                constructSupervisionMessage(buf, ADDR_SENT_RCV, CTRL_DISC);
                writeToSP(buf, SUPERVISION_MSG_SIZE);
                progState = WaitingForDISC;
                break;
            
                    }
    */


}


        