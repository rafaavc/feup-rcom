#include "emitter.h"

extern unsigned stopAndWaitFlag;
extern int fd;

void emitter(int serialPort){
    // sum = 0, speed = 0;

    // Establish communication with receiver
    fd = llopen(serialPort, EMITTER); 
    if (fd == -1) {
        printError("Wasn't able to establish logic connection!");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLOPENFAILED
    }
    
    debugMessage("Connection established successfully!");

    //Começa a escrever todas as tramas de informação, tendo em conta a necessidade de reenvios e etc
    //enquanto tiver informaçao para escrever, escreve com o mesmo mecanismo de stop & wait como nas outras situações    
    FILE * file = fopen("fileToTransfer.txt", "r");
    char buffer[MAX_DATA_LENGTH];
    while (!feof(file)) {
        size_t amount = fread(buffer, sizeof(char), MAX_DATA_LENGTH, file);
        size_t ret = llwrite(fd, buffer, amount); // o segundo argumento tem tamanho máximo = MAX_DATA_LENGTH
        if (ret == -1) {
            printError("Error in llwrite\n");
            exit(EXIT_FAILURE);
        }
        if (ferror(file)) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }
    }
    llwrite(fd, "end", 3);

    //Quando ja nao tiver mais informaçao para escrever vai disconectar,entao
    // envia um DISC, espera um Disc e envia um UA, terminando o programa
    debugMessage("SENDING DISC...");
    //establishDisconnection();
    

    //sleep(1);
    
    //closeSP(&oldtio);
    if (llclose(fd) != 0) {
        printError("Wasn't able to disconnect!");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLCLOSEFAILED
    }
}

/*
! FAZER TRATAMENTOS DE ERROS NAS ESCRITAS E LEITURAS!
*/