#include "stateMachine.h"

/**
 * Handler for signo signal
 * @param signo  signal to be handled 
 */
void alarmHandler(int signo);

/**
 * Opens and configures the serial port
 * @param port  port string /dev/ttySx
 * @return the SP's file descriptor
 */
int openConfigureSP(char* port);

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
 * @return 0 is ok, -1 in case of error
 */
int closeSP();
