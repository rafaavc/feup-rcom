#include "emitterFuncs.h"

extern unsigned stopAndWaitFlag;
extern int fd;

void alarmHandler(int signo) {
    if (signo == SIGALRM) {
        stopAndWaitFlag = TRUE;
        debugMessage("[SIG HANDLER] SIGALRM");
    }
}

int main(int argc, char** argv){
    // sum = 0, speed = 0;
    struct termios oldtio;  

    checkCmdArgs(argc, argv);

    if(signal(SIGALRM, alarmHandler) < 0) {
        perror("Alarm handler wasn't installed");  // instala rotina que atende interrupcao
        exit(EXIT_FAILURE);
    }

    openConfigureSP(argv[1], &oldtio);

    if (!establishLogicConnection()) {
        printf("Wasn't able to establish logic connection\n");
    } 
    else {

        //Começar a escrita da informação
        /*
        tudo acerca da escrita da informaçao

        //enquanto tiver informação para escrever vai tentar escrever da mesma forma que no estabelecimento de conexao
        //quando nao tiver, vai mandar DISC, espera um Disc e manda UA, terminando o programa
        
        */

        char ret[MAX_I_BUFFER_SIZE] = {'\0'};
        size_t s = 12;
        constructInformationMessage(ret, "testi}ng!!!~", &s);  // note: '}' is 0x7D and '~' is 0x7E
        writeToSP(fd, ret, s);
        //printCharArray(ret, s);
    
        debugMessage("SENDING DISC...");
        establishDisconnection();
    }

    sleep(1);
    
    closeSP(&oldtio);

    return 0;
}

/*
! FAZER TRATAMENTOS DE ERROS NAS ESCRITAS E LEITURAS!
*/