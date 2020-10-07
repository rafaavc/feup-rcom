#include "protocol.h"


volatile int STOP=FALSE;

extern unsigned logicConnectionFlag;

void alarmHandler(int signo) {
    logicConnectionFlag = TRUE;
}

int main(int argc, char** argv){
    int fd, sum = 0, speed = 0;
    struct termios oldtio;

    checkCmdArgs(argc, argv);
    printf("Before configure\n");
    fd = openConfigureSP(argv[1], &oldtio);

    //writes to the serial port
    size_t res;
    char* buf = NULL;
    char* ret = NULL;
    ssize_t size; 
    int counter = 0;
    int valid = FALSE;

    buf = constructSupervisionMessage(ADDR_SENT_EM, CTRL_SET);
   


    if(signal(SIGALRM, alarmHandler) < 0)
        printf("Wrong alarm\n");  // instala  rotina que atende interrupcao

    //write it
    while(!valid && counter < 3) {
        //turns the alarm on
        if(logicConnectionFlag){
            alarm(TIME_OUT);              
            logicConnectionFlag = FALSE;

            //writes to the serial port, trying to connect
            res = writeToSP(fd,buf,SUPERVISION_MESSAGE_SIZE);
            
            //verifies if it was written correctly
            if(res != (SUPERVISION_MESSAGE_SIZE+1)){
                printf("Wrong message size\n");
            }

            // TODO: RECHECK READS IF NO REPLY
            
            //tries to read the message back from the serialPort
            ret = readFromSP(fd, &size);
            //verificar se recebeu e se é valida, caso nao reeenvia set enquanto nao for valida um maximo de 3 vezes
            if(size != (SUPERVISION_MESSAGE_SIZE + 1)){
                valid = FALSE;
            }
            else if(ret[2] != (char)CTRL_UA){
                valid = FALSE;
            }
            //another condition for BCC
            else{
                valid = TRUE;
            }
            counter++;
        }
    } 

    printf("Ligação lógica estabelecida!");

    sleep(1);
    
    closeSP(fd, &oldtio);

    return 0;
}

/*
Notas: pus a correr com /dev/ttyS0
Nao fiz tratamento de erros nas leituras e escritas
*/
