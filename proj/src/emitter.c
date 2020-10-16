#include "emitterFuncs.h"

extern unsigned logicConnectionFlag;

void alarmHandler(int signo) {
    if (signo == SIGALRM) {
        logicConnectionFlag = TRUE;
        #ifdef DEBUG
        debugMessage("[SIG HANDLER] SIGALRM");
        #endif
    }
}

int main(int argc, char** argv){
    int fd;
    // sum = 0, speed = 0;
    struct termios oldtio;  

    checkCmdArgs(argc, argv);

    fd = openConfigureSP(argv[1], &oldtio);

    logicConnectionFlag = TRUE;
    
    if(signal(SIGALRM, alarmHandler) < 0) {
        perror("Alarm handler wasn't installed");  // instala  rotina que atende interrupcao
        exit(EXIT_FAILURE);
    }

    if (!establishLogicConnection(fd)) {
        printf("Wasn't able to establish logic connection\n");
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
