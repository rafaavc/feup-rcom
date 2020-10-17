#include "receiverFuncs.h"

extern int fd;
extern enum programState progState;

bool dealWithReceivedMessage(enum stateMachine state, char * msg) {
    char buf[SUPERVISION_MSG_SIZE];
    if (state == DONE_I) {
        debugMessage("RECEIVED «I» DATA");
    } else if (state == DONE_S_U) {
        debugMessage("RECEIVED «S,U» DATA");
        if (progState == WaitingForDISC && msg[CTRL_IDX] != CTRL_UA) {
            progState = LogicallyConnected;
        }
        switch (msg[CTRL_IDX]) {
            case CTRL_UA:
                debugMessage("RECEIVED UA");
                if (progState == WaitingForDISC) {
                    debugMessage("[DISC] SUCCESSFUL");
                    return TRUE;
                }
                break;
            case CTRL_DISC:
                debugMessage("RECEIVED DISC");
                constructSupervisionMessage(buf, ADDR_SENT_RCV, CTRL_DISC);
                writeToSP(fd, buf, SUPERVISION_MSG_SIZE);
                progState = WaitingForDISC;
                break;
            case CTRL_SET:
                constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_UA);
                writeToSP(fd, buf, SUPERVISION_MSG_SIZE);

                debugMessage("[LOGIC CONNECTION] SUCCESS");
                progState = LogicallyConnected;
                break;
            default: 
                break;
        }
    }
    return FALSE;
}

void receiverLoop() {
    char ret[MAX_I_MSG_SIZE];
    ssize_t size;
    enum stateMachine state;

    while (TRUE) {
        readFromSP(fd, ret, &state, &size, ANY_VALUE, ANY_VALUE);
        if (isAcceptanceState(&state)) {
            if (dealWithReceivedMessage(state, ret)) return;
        }
    }
}

