#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include <signal.h>
#include <stdbool.h>
#include "../utilities/utilities.h"
#include "../tests/efficiency.h"

enum stateMachine { START, FLAG_RCV, A_RCV, C_RCV, BCC_HEAD_OK, DATA, DATA_OK, BCC_DATA_OK, DONE_S_U, DONE_I };
enum destuffingState { DESTUFF_OK, DESTUFF_WAITING, DESTUFF_VIEWING };
enum checkStateRET { 
    StateOK, 
    IGNORE_CHAR,
    HEAD_INVALID,
    DATA_INVALID
};

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
