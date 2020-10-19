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
    else if (isSU(&state)) {
        //debugMessage("RECEIVED «S,U» DATA");
        if (progState == WaitingForDISC && msg[CTRL_IDX] != CTRL_UA) {
            progState = LogicallyConnected;
        }
        switch (msg[CTRL_IDX]) {
            case CTRL_UA:
                if (progState == WaitingForLC) break;

                debugMessage("RECEIVED UA");
                if (progState == WaitingForDISC) {
                    debugMessage("[DISC] SUCCESSFUL");
                    return TRUE;
                }
                break;
            case CTRL_DISC:
                if (progState == WaitingForLC) break;

                debugMessage("RECEIVED DISC");
                constructSupervisionMessage(buf, ADDR_SENT_RCV, CTRL_DISC);
                writeToSP(buf, SUPERVISION_MSG_SIZE);
                progState = WaitingForDISC;
                break;
            case CTRL_SET:
                constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_UA);
                writeToSP(buf, SUPERVISION_MSG_SIZE);

                debugMessage("[LOGIC CONNECTION] SUCCESS\n");
                progState = LogicallyConnected;
                break;
            default:
                if (progState == WaitingForDISC) {
                    progState = LogicallyConnected;
                }
                break;
        }
    }
    return FALSE;
}

void receiverLoop() {
    ssize_t size;
    enum stateMachine state;
    enum readFromSPRet res;

    while (TRUE) {
        char ret[MAX_I_MSG_SIZE] = {'\0'};
        res = readFromSP(ret, &state, &size, ANY_VALUE, ANY_VALUE);
        //printCharArray(ret, size);
        if (isAcceptanceState(&state)) {
            if (dealWithReceivedMessage(state, ret, size, res)) return;
        }
    }
}

