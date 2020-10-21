#include "application.h"

#define PORT_AMOUNT 2
#define TRANSMITTER_STRING "TRANSMITTER"
#define RECEIVER_STRING "RECEIVER"

void printUsage(int * ports) {
    printf("Usage:\n  ./app <serial port> <actor>\nWhere <serial port> is the final number of one of:\n");
    for (unsigned i = 0; i < PORT_AMOUNT; i++) {
        printf("/dev/ttyS%d\n", ports[i]);
    }
    printf("And <actor> is either RECEIVER or TRANSMITTER.\n");
    exit(EXIT_FAILURE);
}

void checkCmdArgs(int argc, char ** argv) {
    int ports[PORT_AMOUNT] = {
        #ifndef SOCAT 
        0,
        1
        #else
        10,
        11
        #endif
    };

    if (argc < 3) {
        printUsage(ports);
    } else {
        unsigned isValidSP = 0;
        for (unsigned i = 0; i < PORT_AMOUNT; i++) {
            if (argv[1] == ports[i]){
                isValidSP = 1;
                break;
            }
                
        }
        if ((strcmp(argv[2], TRANSMITTER_STRING) != 0 && strcmp(argv[2], RECEIVER_STRING) != 0) || !isValidSP) {
            printUsage(ports);
        }
    }
}

void applicationLoop(int argc, char ** argv) {
    checkCmdArgs(argc, argv);

    if (strcmp(argv[2], TRANSMITTER_STRING) == 0) {
        // TRANSMITTER
        emitter(stoi(argv[1]));
    } else {
        // RECEIVER
        receiver(atoi(argv[1]));
    }

}


