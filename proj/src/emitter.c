#include "emitterFuncs.h"

extern unsigned logicConnectionFlag;
extern enum stateMachine_S_U state;


void alarmHandler(int signo) {
    //if(state != DONE)
        logicConnectionFlag = TRUE;
}

int main(int argc, char** argv){
    int fd;
    // sum = 0, speed = 0;
    struct termios oldtio;  
   
    checkCmdArgs(argc, argv);

    fd = openConfigureSP(argv[1], &oldtio);
    
    if(signal(SIGALRM, alarmHandler) < 0) {
        perror("Alarm handler wasn't installed");  // instala  rotina que atende interrupcao
        exit(EXIT_FAILURE);
    }

    if (establishLogicConnection(fd))
        printf("Ligação lógica estabelecida!\n");
    else {
        perror("Wasn't able to establish logic connection");
        exit(EXIT_FAILURE);
    }

    sleep(1);
    
    closeSP(fd, &oldtio);

    return 0;
}

/*
Notas: pus a correr com /dev/ttyS0
Nao fiz tratamento de erros nas leituras e escritas
*/
