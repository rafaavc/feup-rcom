#include "receiver.h"

extern int fd;
enum programState progState = WaitingForLC;

void receiver(int serialPort){
    // sum=0, speed=0;

    // Establish communication with emitter
    if ((fd = llopen(serialPort, RECEIVER)) == -1) {
        printError("Wasn't able to establish logic connection!\n");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLOPENFAILED
    }
    /*
    Reads the data, until it receives a disconnect, when this happens sends back a disconect and the program hands
    */
    char buffer[MAX_DATA_LENGTH];
    while (TRUE) {
        size_t dataLen = llread(fd, buffer);
        if (dataLen == 3 && buffer[0] == 'e' && buffer[1] == 'n' && buffer[2] == 'd') break;
        write(STDOUT_FILENO, buffer, dataLen);
    }


    if (llclose(fd) != 0) {
        printError("Wasn't able to disconnect!\n");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLCLOSEFAILED
    }
}
