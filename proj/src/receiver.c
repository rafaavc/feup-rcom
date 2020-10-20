#include "receiverFuncs.h"

extern int fd;
enum programState progState = WaitingForLC;

int main(int argc, char** argv){
    // sum=0, speed=0;
    struct termios oldtio;

    checkCmdArgs(argc, argv);

    //openConfigureSP(argv[1], &oldtio);

      // Establish communication with emitter
    if (llopen(argv[1], EMITTER) != 0) {
        fprintf(stderr, "\nWasn't able to establish logic connection\n");
        return -1;//provavelmente dar nomes signifcativos--LLOPENFAILED
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
        return -1;//provavelmente dar nomes signifcativos--LLCLOSEFAILED
    }


    return 0;
}
