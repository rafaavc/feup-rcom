#include "tests.h"

#define RECEIVER 0
#define TRANSMITTER 1


int testReceiver(int fd) {
    char *buffer = (char*)myMalloc(MAX_DATA_PACKET_LENGTH*sizeof(char));//mudar isto
    //while(true) {
        int n;
        if ((n = llread(fd, buffer)) == -1) {
            return -1;
        } else {
            printf("Just received %d bytes: ", n);
            fwrite(buffer, sizeof(char), n, stdout);
            printf("\n");
        }
    //}

    free(buffer);
}


int main(int argc, char ** argv) {
    checkCmdArgs(argc, argv);

    int serialPort = atoi(argv[1]);
    unsigned role = strcmp(argv[2], RECEIVER_STRING) == 0 ? RECEIVER : TRANSMITTER;

    int fd;
    if ((fd = llopen(serialPort, role == RECEIVER ? RECEIVER_STRING : TRANSMITTER_STRING)) == -1) {
        printError("Wasn't able to establish logic connection!\n");
        exit(EXIT_FAILURE); 
    }

    switch(role) {
        case RECEIVER:
            testReceiver(fd);
            break;
        case TRANSMITTER:
            if (argc >= 4 && isUnsignedNumber(argv[3])) {
                test(fd, atoi(argv[3]));
            } else {
                printError("You didn't specify a test value.\n");
            }
            break;
        default: break;
    }



    if (llclose(fd) != 0) {
        printError("Wasn't able to disconnect!\n");
        exit(EXIT_FAILURE);
    }
     
    
    return 0;
}

