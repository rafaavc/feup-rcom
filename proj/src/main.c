#include "application/application.h"

void applicationLoop(int argc, char ** argv) {
    checkCmdArgs(argc, argv);

    if (strcmp(argv[2], TRANSMITTER_STRING) == 0) { // TRANSMITTER
        transmitter(atoi(argv[1]), (argc >= 4 ? argv[3] : NULL), (argc >= 5 ? argv[4] : NULL));
    } 
    else { // RECEIVER
        receiver(atoi(argv[1]));
    }

}

int main(int argc, char ** argv) {
    applicationLoop(argc, argv);
    return 0;
}
