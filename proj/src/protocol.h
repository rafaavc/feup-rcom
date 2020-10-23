#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include "utilities.h"



void alarmHandler(int signo);

/**
 * Opens and configures the serial port
 * @return the SP's file descriptor
 */
int openConfigureSP(char* port, struct termios *oldtio);


/**
 * Writes the serial port
 */
size_t writeToSP( char * message, size_t messageSize);

/**
 * Reads from the serial port using the state machine implementation
 * @param fd the fd of the serial port
 * @param state return variable (at end of execution, contains the final state)
 * @param stringSize return variable (at end of execution, contains the string's size)
 * @param addressField the desired address value (ANY_VALUE if not specified)
 * @param controlField the desired control field (ANY_VALUE if not specified)
 * @return 
 */
enum readFromSPRet readFromSP(char * buf, enum stateMachine *state, ssize_t * stringSize, char addressField, char controlField);

void constructSupervisionMessage(char * ret, char addr, char ctrl);

/**
 * @param dataSize the number of bytes in the data array (if it's a string, exclude \0)
 */
void constructInformationMessage(char* ret ,char* data, size_t * dataSize);

void byteStuffing(char * ret, size_t * dataSize);

void byteDestuffing(char * ret, size_t * dataSize);

int closeSP(struct termios *oldtio);

/**
 * @param state the state variable
 * @return TRUE if is acceptance state
 */
bool isAcceptanceState(enum stateMachine *state);

/**
 * @param state the state variable
 * @return TRUE if is DONE_I
 */
bool isI(enum stateMachine *state);

/**
 * @param state the state variable
 * @return TRUE if is DONE_S_U
 */
bool isSU(enum stateMachine *state);

bool isRR(unsigned char ctrl);

bool isREJ(unsigned char ctrl);

int getS(unsigned char ctrl);

int getR(unsigned char ctrl);

bool checkDestuffedBCC(char* buf, char bcc, size_t bufSize, int noFlag);
/**
 * Deals with the state transitions
 * @param state the state variable
 * @param bcc an array of two chars, the first is the address field and the second is the control field. They are used to calculate the bcc
 * @param byte the byte received
 * @param addressField the desired address value (ANY_VALUE if not specified)
 * @param controlField the desired control field (ANY_VALUE if not specified)
 * @return 0 if OK, 1 if this byte doesn't count
 */
enum checkStateRET checkState(enum stateMachine *state, char * bcc, char * byte, char*data, char addressField, char controlField);

/*void checkState_end(enum stateMachine_S_U *state, char *bcc, char byte);
void checkState_begin(enum stateMachine_S_U *state, char *bcc, char byte, int emitter);
void checkState_data(enum stateMachine_I *state, char *bcc, char byte, int emitter);*/
