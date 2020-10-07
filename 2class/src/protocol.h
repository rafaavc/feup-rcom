#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include "defines.h"


/*enum type {
    RECEIVER,
    EMITTER
} typeEnum;

struct ProtocolSU {
    uint8_t flag, addressField, controllField, blockCheckCharacter;
};

struct ProtocolI {
    struct ProtocolSU control;
    uint8_t blockCheckCharacter1;
    char * data;
};


struct ProtocolI getProtocolI(enum type endType, char * data);*/

/**
 * Verifies the command line arguments
 */
void checkCmdArgs(int argc, char** argv);


/**
 * Opens and configures the serial port
 * @return the SP's file descriptor
 */
int openConfigureSP(char* port, struct termios *oldtio);


/**
 * Writes the serial port
 */
size_t writeToSP(int fd, char * message, size_t messageSize);

char * readFromSP(int fd, ssize_t * stringSize);

char *  constructSupervisionMessage(char  addr, char ctrl);

void closeSP(int fd, struct termios *oldtio);