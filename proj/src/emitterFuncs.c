#include "emitterFuncs.h"

extern unsigned logicConnectionFlag;
extern enum stateMachine state;

unsigned establishLogicConnection(int fd) {
    
    //writes to the serial port
    size_t res;
    ssize_t size; 
    char buf[SUPERVISION_MSG_SIZE], ret[MAX_I_MSG_SIZE];
    int counter = 0;

    constructSupervisionMessage(buf, ADDR_SENT_EM, CTRL_SET);
    

    //write it
    while(counter < NO_TRIES) {
        //turns the alarm on
        if(logicConnectionFlag) {            
            //writes to the serial port, trying to connect
            res = writeToSP(fd, buf, SUPERVISION_MSG_SIZE);

            alarm(TIME_OUT);  

            logicConnectionFlag = FALSE;
            
            //verifies if it was written correctly
            if (res != (SUPERVISION_MSG_SIZE + 1)) {
                printf("Wrong message size\n");
            }
            
            enum stateMachine state;

            //tries to read the message back from the serialPort
            readFromSP(fd, ret, &state, &size, ADDR_SENT_EM, CTRL_UA);

            if(isAcceptanceState(&state))
                return TRUE;
            
            counter++;
        }
    }

    return FALSE;
}
