#include "emitter.h"

extern unsigned stopAndWaitFlag;
extern int fd;

void emitter(int serialPort){
        
    fd = llopen(serialPort, EMITTER); // Establish communication with receiver

    if (fd == -1) {
        printError("Wasn't able to establish logic connection!");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLOPENFAILED
    }
    
    debugMessage("Connection established successfully!");


    /*Starts to write all information frames, keeping in mind the need to resend, etc.
    While there is info to write, writes with the stop&wait mechanism as in other situations*/

    FILE * file = fopen("fileToTransfer.txt", "r");
    char buffer[MAX_DATA_LENGTH];

    while (!feof(file)) {
        size_t amount = fread(buffer, sizeof(char), MAX_DATA_LENGTH, file);
        size_t ret = llwrite(fd, buffer, amount); // second arg has a maximum size of MAX_DATA_LENGTH
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

    /*
        When there is no more information to write, it's going to disconnect: sends a DISC frame, receives a DISC frame
        and sends back an UA frame, ending the program execution
    */
    debugMessage("SENDING DISC...");
 
    if (llclose(fd) != 0) {
        printError("Wasn't able to disconnect!");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLCLOSEFAILED
    }
}

/*
! FAZER TRATAMENTOS DE ERROS NAS ESCRITAS E LEITURAS!
*/