#include "application.h"

void printUsage() {
    printf("Usage:\n  ./app <serial port> <actor>\nWhere <serial port> is the 'x' in:\n/dev/ttySx\nAnd <actor> is either RECEIVER or TRANSMITTER.\n");
    exit(EXIT_FAILURE);
}

void checkCmdArgs(int argc, char ** argv) {
    if (argc < 3) {
        printUsage();
    } else {
        bool isValidSP = isUnsignedNumber(argv[1]);
        if ((strcmp(argv[2], TRANSMITTER_STRING) != 0 && strcmp(argv[2], RECEIVER_STRING) != 0) || !isValidSP) {
            printUsage();
        }
    }
}
