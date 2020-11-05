#include "application.h"

extern char *optarg;

void printUsage() {
    printf("Usage:\n  ./app <serial port> <actor>\nWhere <serial port> is the 'x' in:\n/dev/ttySx\nAnd <actor> is either RECEIVER or TRANSMITTER.\nWhen <actor> is TRANSMITTER you can add two optional arguments: <fileToSend> <destFileName>.\n");
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

        /*
        int option;
        while ((option = getopt(argc, argv, "tb")) != -1) {
            switch(option) {
                case 't':
                    if (!isUnsignedNumber(optarg)) {
                        printError("Frame size (-t) has to be an unsigned number.\n");
                    } else {
                        //printf("%u\n", atoi(optarg));
                    }
                    break;
                case 'b':
                    if (!isUnsignedNumber) {
                        printError("Frame size (-t) has to be an unsigned number.\n");
                    } else {
                        speed_t baudrate;
                        sscanf(optarg, "%u", baudrate);
                    }
                    break;
                default:
                    break;
            }
        }
        */

    }
}
