#include "receiverFuncs.h"

extern int fd;
enum programState progState = WaitingForLC;

int main(int argc, char** argv){
    // sum=0, speed=0;
    struct termios oldtio;

    checkCmdArgs(argc, argv);

    openConfigureSP(argv[1], &oldtio);

    /*
    Reads the data, until it receives a disconnect, when this happens sends back a disconect and the program hands
    */
    receiverLoop();

    sleep(1);

    closeSP(&oldtio);

    return 0;
}
