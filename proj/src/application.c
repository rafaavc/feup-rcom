#include "application.h"

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
        bool isValidSP = isUnsignedNumber(argv[1]);
        if ((strcmp(argv[2], TRANSMITTER_STRING) != 0 && strcmp(argv[2], RECEIVER_STRING) != 0) || !isValidSP) {
            printUsage(ports);
        }
    }
}
