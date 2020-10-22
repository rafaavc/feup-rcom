#include "receiver.h"

extern int fd;
enum programState progState = WaitingForLC;

void receiver(int serialPort){
    // sum=0, speed=0;

    //openConfigureSP(argv[1], &oldtio);

      // Establish communication with emitter
    if ((fd = llopen(serialPort, RECEIVER)) == -1) {
        fprintf(stderr, "\nWasn't able to establish logic connection\n");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLOPENFAILED
    }
    /*
    Reads the data, until it receives a disconnect, when this happens sends back a disconect and the program hands
    */
    //receiverLoop();
    char buffer[MAX_DATA_LENGTH];
    size_t dataLen = llread(fd, buffer);
    printCharArray(buffer, dataLen);

    //sleep(1);
    //closeSP(&oldtio);
    if (llclose(fd) != 0) {
        fprintf(stderr, "\nIssues during disconnection\n");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLCLOSEFAILED
    }
}
