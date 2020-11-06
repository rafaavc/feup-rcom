#include "application.h"

extern char *optarg;

int MAX_DATA_PACKET_DATA_SIZE;
extern int MAX_DATA_PACKET_SIZE;

void printUsage() {
    printf("Usage:\n  ./app <serial port> <actor>\nWhere <serial port> is the 'x' in:\n/dev/ttySx\nAnd <actor> is either RECEIVER or TRANSMITTER.\nWhen <actor> is TRANSMITTER you can add two optional arguments: <fileToSend> <destFileName>.\n");
    exit(EXIT_FAILURE);
}

void checkCmdArgs(int argc, char ** argv) {
    if (argc < 3) {
        printUsage();
    } 
    else {
        bool isValidSP = isUnsignedNumber(argv[1]);
        if ((strcmp(argv[2], TRANSMITTER_STRING) != 0 && strcmp(argv[2], RECEIVER_STRING) != 0) || !isValidSP) {
            printUsage();
        }

        int frameSize = -1, baudrate = -1;

        
        int option;
        while ((option = getopt(argc, argv, "-t:b:")) != -1) {
            switch(option) {
                case 't':
                    if (!isUnsignedNumber(optarg)) {
                        printError("Frame size (-t) has to be an unsigned number.\n");
                    } else {
                        frameSize = atoi(optarg);
                    }
                    break;
                case 'b':
                    if (!isUnsignedNumber(optarg)) {
                        printError("Baudrate (-b) has to be an unsigned number. Available:\n4800\n9600\n19200\n38400\n57600\n115200\n230400\n");
                    } else {
                        baudrate = atoi(optarg);
                    }
                    break;
                case 1:
                default:
                    break;
            }
        }

        llset(baudrate, frameSize);

        MAX_DATA_PACKET_DATA_SIZE = MAX_DATA_PACKET_SIZE - 4;

    }
}


