#include "receiverFuncs.h"

extern int logicConnectionFlag;
extern enum stateMachine state;

int main(int argc, char** argv){
    int fd;
    // sum=0, speed=0;
    struct termios oldtio;

    logicConnectionFlag = FALSE;

    checkCmdArgs(argc, argv);


    fd = openConfigureSP(argv[1], &oldtio);

    if (establishLogicConnection(fd))
        printf("Connected successfully!\n");
    
    
    sleep(1);

    closeSP(fd, &oldtio);

    return 0;
}
/*
Nota: falta tratamentos de erro

*/
