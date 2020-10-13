#include "protocol.h"


extern unsigned logicConnectionFlag;
extern enum stateMachine state;

void alarmHandler(int signo) {
    //if(state != DONE)
        logicConnectionFlag = TRUE;
}

int main(int argc, char** argv){
    int fd, sum = 0, speed = 0;
    struct termios oldtio;  


    checkCmdArgs(argc, argv);
    fd = openConfigureSP(argv[1], &oldtio);

    if(signal(SIGALRM, alarmHandler) < 0)
        printf("Wrong alarm\n");  // instala  rotina que atende interrupcao


    //writes to the serial port
    size_t res;
    char* buf = NULL;
    char* ret = NULL;
    ssize_t size; 
    int counter = 0;
    int valid = FALSE;

    buf = constructSupervisionMessage(ADDR_SENT_EM, CTRL_SET);
   

    //write it
    while(!valid && counter < NO_TRIES) {
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
                valid = TRUE;
            else valid = FALSE;
            
            counter++;
        }
    
    } 
    if (valid)
        printf("Ligação lógica estabelecida!\n");

    sleep(1);
    
    closeSP(fd, &oldtio);

    return 0;
}

/*
Notas: pus a correr com /dev/ttyS0
Nao fiz tratamento de erros nas leituras e escritas
*/
