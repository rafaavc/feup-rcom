#include "emitterFuncs.h"

extern unsigned logicConnectionFlag;

void alarmHandler(int signo) {
        logicConnectionFlag = TRUE;
}

int main(int argc, char** argv){
    int fd;
    // sum = 0, speed = 0;
    struct termios oldtio;  

    logicConnectionFlag = TRUE; // says that it is ready to attempt connection
   
    checkCmdArgs(argc, argv);

    fd = openConfigureSP(argv[1], &oldtio);
    
    if(signal(SIGALRM, alarmHandler) < 0) {
        perror("Alarm handler wasn't installed");  // instala  rotina que atende interrupcao
        exit(EXIT_FAILURE);
    }

    if (establishLogicConnection(fd))
        printf("Ligação lógica estabelecida!\n");
    else {
        printf("Wasn't able to establish logic connection\n");
        exit(EXIT_FAILURE);
    }

    //Começar a escrita da informação
    /*
    tudo acerca da escrita da informaçao

    //enquanto tiver informação para escrever vai tentar escrever da mesma forma que no estabelecimento de conexao
    //quando nao tiver, vai mandar DISC, espera um Disc e manda UA, terminando o programa
    
    */

    disconnect(fd);

    sleep(1);
    
    closeSP(fd, &oldtio);

    return 0;
}

/*
! FAZER TRATAMENTOS DE ERROS NAS ESCRITAS E LEITURAS!
*/