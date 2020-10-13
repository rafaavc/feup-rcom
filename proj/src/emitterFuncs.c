#include "emitterFuncs.h"

extern unsigned logicConnectionFlag;
extern enum stateMachine state;

unsigned establishLogicConnection(int fd) {
    
    //writes to the serial port
    size_t res;
    char* buf = NULL;
    char* ret = NULL;
    ssize_t size; 
    int counter = 0;
    int valid = FALSE;

    buf = constructSupervisionMessage(ADDR_SENT_EM, CTRL_SET);
    

    //write it
    while(counter < NO_TRIES) {
        //turns the alarm on
        if(logicConnectionFlag) {            
            //writes to the serial port, trying to connect
            res = writeToSP(fd,buf,SUPERVISION_MESSAGE_SIZE);

            alarm(TIME_OUT);  

            logicConnectionFlag = FALSE;
            state = Start;
            
            //verifies if it was written correctly
            if (res != (SUPERVISION_MESSAGE_SIZE+1)) {
                printf("Wrong message size\n");
            }
            
            //tries to read the message back from the serialPort
            ret = readFromSP(fd, &size,1);

            if(state == DONE)
                return TRUE;
            
            counter++;
        }
    } 
}
