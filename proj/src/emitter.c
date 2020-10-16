#include "emitterFuncs.h"

extern unsigned stopAndWaitFlag;
extern int fd;

void alarmHandler(int signo) {
    if (signo == SIGALRM) {
        stopAndWaitFlag = TRUE;
        #ifdef DEBUG
        debugMessage("[SIG HANDLER] SIGALRM");
        #endif
    }
}

int main(int argc, char** argv){
    // sum = 0, speed = 0;
    struct termios oldtio;  

    stopAndWaitFlag = TRUE;

    checkCmdArgs(argc, argv);

    openConfigureSP(argv[1], &oldtio);

    
    if(signal(SIGALRM, alarmHandler) < 0) {
        perror("Alarm handler wasn't installed");  // instala  rotina que atende interrupcao
        exit(EXIT_FAILURE);
    }

    if (!establishLogicConnection()) {
        printf("Wasn't able to establish logic connection\n");
        exit(EXIT_FAILURE);
    }

    //Começar a escrita da informação
    /*
    tudo acerca da escrita da informaçao

    //enquanto tiver informação para escrever vai tentar escrever da mesma forma que no estabelecimento de conexao
    //quando nao tiver, vai mandar DISC, espera um Disc e manda UA, terminando o programa
    
    */

    establishDisconnection();

    sleep(1);
    
    closeSP(&oldtio);

    return 0;
}

/*
! FAZER TRATAMENTOS DE ERROS NAS ESCRITAS E LEITURAS!
*/