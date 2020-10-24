#include "application.h"

void applicationLoop(int argc, char ** argv) {
    checkCmdArgs(argc, argv);

    if (strcmp(argv[2], TRANSMITTER_STRING) == 0) { // TRANSMITTER
        transmitter(atoi(argv[1]));
    } 
    else { // RECEIVER
        receiver(atoi(argv[1]));
    }

}
