#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include "utilities.h"


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

/**
 * Reads from the serial port using the state machine implementation
 * @param fd the fd of the serial port
 * @param state return variable (at end of execution, contains the final state)
 * @param stringSize return variable (at end of execution, contains the string's size)
 * @param addressField the desired address value (ANY_VALUE if not specified)
 * @param controlField the desired control field (ANY_VALUE if not specified)
 * @return the string read
 */
void readFromSP(int fd, char * buf, enum stateMachine *state, ssize_t * stringSize, char addressField, char controlField);

void constructSupervisionMessage(char * ret, char addr, char ctrl);

void constructInformationMessage(char* ret ,char* data, size_t dataSize);

void byteStuffing(char* ret, size_t dataSize);

char* byteDestuffing(char * ret, size_t dataSize);

void closeSP(int fd, struct termios *oldtio);

/**
 * @param the state variable
 * @return TRUE if is acceptance state
 */
unsigned isAcceptanceState(enum stateMachine *state);

/**
 * @param the state variable
 * @return TRUE if is DONE_I
 */
unsigned isI(enum stateMachine *state);

/**
 * @param the state variable
 * @return TRUE if is DONE_S_U
 */
unsigned isSU(enum stateMachine *state);


/**
 * Deals with the state transitions
 * @param state the state variable
 * @param bcc an array of two chars, the first is the address field and the second is the control field. They are used to calculate the bcc
 * @param byte the byte received
 * @param addressField the desired address value (ANY_VALUE if not specified)
 * @param controlField the desired control field (ANY_VALUE if not specified)
 * @return 0 if OK, 1 if the machine restarted
 */
unsigned checkState(enum stateMachine *state, char * bcc, char byte, char addressField, char controlField);

/*void checkState_end(enum stateMachine_S_U *state, char *bcc, char byte);
void checkState_begin(enum stateMachine_S_U *state, char *bcc, char byte, int emitter);
void checkState_data(enum stateMachine_I *state, char *bcc, char byte, int emitter);*/
