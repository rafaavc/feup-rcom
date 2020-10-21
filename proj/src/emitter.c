#include "emitter.h"

extern unsigned stopAndWaitFlag;
extern int fd;

void alarmHandler(int signo) {
    if (signo == SIGALRM) {
        stopAndWaitFlag = TRUE;
        debugMessage("[SIG HANDLER] SIGALRM");
    }
}

void emitter(int serialPort){
    // sum = 0, speed = 0;
    //struct termios oldtio;  


    if(signal(SIGALRM, alarmHandler) < 0) {
        perror("Alarm handler wasn't installed");  // instala rotina que atende interrupcao
        exit(EXIT_FAILURE);
    }

    //openConfigureSP(argv[1], &oldtio);

    // Establish communication with receiver
    llopen(serialPort, EMITTER); 
    if (fd == -1) {
        fprintf(stderr, "\nWasn't able to establish logic connection\n");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLOPENFAILED
    }
    
    fprintf(stdout,"\n Connection established successfully|\n");

    //Começa a escrever todas as tramas de informação, tendo em conta a necessidade de reenvios e etc
    //enquanto tiver informaçao para escrever, escreve com o mesmo mecanismo de stop & wait como nas outras situações    

    char ret[MAX_I_BUFFER_SIZE] = {'\0'};
    size_t s = 12;
    
    constructInformationMessage(ret, "testi}ng!!!~", &s);  // note: '}' is 0x7D and '~' is 0x7E
    //note cant always change s, only when we dont need to retransmit it
    sendMessage(ret,s);
    //printCharArray(ret, s);

    //Quando ja nao tiver mais informaçao para escrever vai disconectar,entao
    // envia um DISC, espera um Disc e envia um UA, terminando o programa
    debugMessage("SENDING DISC...");
    //establishDisconnection();
    

    //sleep(1);
    
    //closeSP(&oldtio);
    if (llclose(fd) != 0) {
        fprintf(stderr, "\nIssues during disconnection\n");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLCLOSEFAILED
    }
}

/*
! FAZER TRATAMENTOS DE ERROS NAS ESCRITAS E LEITURAS!
*/