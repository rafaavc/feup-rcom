#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include "utilities.h"

/**
 * Handler for signo signal
 * @param signo - signal to be handled 
 */
void alarmHandler(int signo);

/**
 * Opens and configures the serial port
 * @param port -
 * @param oldtio - 
 * @return the SP's file descriptor
 */
int openConfigureSP(char* port, struct termios *oldtio);


/**
 * Writes the serial port
 * @param message -
 * @param messageSize -
 */
size_t writeToSP( char * message, size_t messageSize);

/**
 * Reads from the serial port using the state machine implementation
 * @param buf 
 * @param state return variable (at end of execution, contains the final state)
 * @param stringSize return variable (at end of execution, contains the string's size)
 * @param addressField the desired address value (ANY_VALUE if not specified)
 * @param controlField the desired control field (ANY_VALUE if not specified)
 * @return 
 */
enum readFromSPRet readFromSP(char * buf, enum stateMachine *state, ssize_t * stringSize, char addressField, char controlField);
/**
 * Contructs the supervision message with the necessary flags
 * @param ret buffer where the supervision message will be stored
 * @param addressField the desired address value 
 * @param controlField the desired control field 
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
 * @param oldtio 
 * @return
 */
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
/**
 * @param ctrl the ctrl variable
 * @return TRUE if is RR
 */
bool isRR(unsigned char ctrl);
/**
 * @param ctrl the ctrl variable
 * @return TRUE if is REJ
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
 * 
 * @param state the state variable
 * @param buf -
 * @param bcc an array of two chars, the first is the address field and the second is the control field. They are used to calculate the bcc
 * @param bufSize -
 * @param noFlag -
 * @return 0 if OK, 1 if this byte doesn't count
 */

bool checkDestuffedBCC(char* buf, char bcc, size_t bufSize, int noFlag);
/**
 * Deals with the state transitions
 * @param state the state variable
 * @param bcc an array of two chars, the first is the address field and the second is the control field. They are used to calculate the bcc
 * @param byte the byte received
 * @param data
 * @param addressField the desired address value (ANY_VALUE if not specified)
 * @param controlField the desired control field (ANY_VALUE if not specified)
 * @return 0 if OK, 1 if this byte doesn't count
 */
enum checkStateRET checkState(enum stateMachine *state, char * bcc, char * byte, char*data, char addressField, char controlField);

/*void checkState_end(enum stateMachine_S_U *state, char *bcc, char byte);
void checkState_begin(enum stateMachine_S_U *state, char *bcc, char byte, int transmitter);
void checkState_data(enum stateMachine_I *state, char *bcc, char byte, int transmitter);*/
