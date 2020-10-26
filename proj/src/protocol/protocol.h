#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include <signal.h>
#include <stdbool.h>
#include "defines.h"
#include "../utilities/utilities.h"

/**
 * Handler for signo signal
 * @param signo  signal to be handled 
 */
void alarmHandler(int signo);

/**
 * Opens and configures the serial port
 * @param port  port string /dev/ttySx
 * @param oldtio  old termios struct
 * @return the SP's file descriptor
 */
int openConfigureSP(char* port, struct termios *oldtio);


/**
 * Writes the serial port
 * @param message  Message to write 
 * @param messageSize  Size of the message 
 */
size_t writeToSP( char * message, size_t messageSize);

/**
 * Reads from the serial port using the state machine implementation
 * @param buf where the read data is stored
 * @param state return variable (at end of execution, contains the final state)
 * @param stringSize return variable (at end of execution, contains the string's size)
 * @param addressField the desired address value (ANY_VALUE if not specified)
 * @param controlField the desired control field (ANY_VALUE if not specified)
 * @return The answer to give the transmitter accordingly to what was received
 */
enum readFromSPRet readFromSP(char * buf, enum stateMachine *state, ssize_t * stringSize, char addressField, char controlField);
/**
 * Contructs the supervision message with the necessary flags
 * @param ret buffer where the supervision message will be stored
 * @param addr the desired address value 
 * @param ctrl the desired control field 
 */
void constructSupervisionMessage(char * ret, char addr, char ctrl);

/**
 * Constructions the information message 
 * @param ret buffer where the information message will be stored
 * @param data data to include in the information message
 * @param dataSize the number of bytes in the data array (if it's a string, exclude \0)
 */
void constructInformationMessage(char* ret ,char* data, size_t * dataSize);
/**
 * Stuffs the data with the escape octet when needed 
 * @param ret buffer where the stuffed data will be stored
 * @param dataSize the number of bytes in the data array (if it's a string, exclude \0)
 */
void byteStuffing(char * ret, size_t * dataSize);
/**
 * Destuffs the data with the escape octet when needed 
 * @param ret buffer where the destuffed data will be stored
 * @param dataSize the number of bytes in the data array (if it's a string, exclude \0)
 */
void byteDestuffing(char * ret, size_t * dataSize);
/**
 * 
 * @param oldtio old termios struct
 * @return 0 is ok, -1 in case of error
 */
int closeSP(struct termios *oldtio);

/**
 * @param state the state variable
 * @return true if is acceptance state
 */
bool isAcceptanceState(enum stateMachine *state);

/**
 * @param state the state variable
 * @return true if is DONE_I
 */
bool isI(enum stateMachine *state);

/**
 * @param state the state variable
 * @return true if is DONE_S_U
 */
bool isSU(enum stateMachine *state);
/**
 * @param ctrl the ctrl variable
 * @return true if is RR
 */
bool isRR(unsigned char ctrl);
/**
 * @param ctrl the ctrl variable
 * @return true if is REJ
 */
bool isREJ(unsigned char ctrl);
/**
 * @param ctrl the ctrl variable
 * @return the value of s in the control flag
 */
int getS(unsigned char ctrl);
/**
 * @param ctrl the ctrl variable
 * @return the value of r in the RR/REJ control flag
 */
int getR(unsigned char ctrl);
/**
 * Calculates the BCC with the destuffed data and checks if it is equal to the BCC received
 * @param state the state variable
 * @param buf  buffer with the frame received
 * @param bcc an array of two chars, the first is the address field and the second is the control field. They are used to calculate the bcc
 * @param bufSize size of the buffer
 * @return True if it equal, false otherwise
 */

bool checkDestuffedBCC(char* buf, char bcc, size_t bufSize);
/**
 * Deals with the state transitions
 * @param state the state variable
 * @param bcc an array of two chars, the first is the address field and the second is the control field. They are used to calculate the bcc
 * @param byte the byte received
 * @param buf buffer with the frame received
 * @param addressField the desired address value (ANY_VALUE if not specified)
 * @param controlField the desired control field (ANY_VALUE if not specified)
 * @return 
 */
enum checkStateRET checkState(enum stateMachine *state, char * bcc, char * byte, char*buf, char addressField, char controlField);

/**
 * Checks if it has received the Message flag and if the frame has been destuffed
 * @param byte the byte received
 * @param destuffing state of destuffing the frame is currently
 * @return true if it is, false otherwise
 */
bool receivedMessageFlag(char * byte, enum destuffingState destuffing);
/**
 * Does the necessary things to go back to the start state
 * @param state current state
 * @param destuffing state of destuffing the frame is currently
 */
void goBackToStart(enum stateMachine * state, enum destuffingState * destuffing);
/**
 * Does the necessary things to go back to the flag received state
 * @param state current state
 * @param destuffing state of destuffing the frame is currently
 */
void goBackToFLAG_RCV(enum stateMachine * state, enum destuffingState * destuffing);
