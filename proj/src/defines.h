#ifndef _DEFINES_H_
#define _DEFINES_H_

#define EMITTER     0
#define RECEIVER    1


#define BIT(n)      (0x01 << (n) )

#define SOCAT
#define DEBUG
//#define DEBUG_STATE_MACHINE
#define MAX_DEBUG_MSG_LENGTH 100

/* Message */
#define MSG_FLAG    0x7E /*all messages are delimited by this flag*/


/* Address */
#define ADDR_SENT_EM    0x03 /*Commands sent by emitter and answers sent by receiver*/
#define ADDR_SENT_RCV   0x01 /*Commands sent by receiver and answers sent by emitter*/


/* Control Protocol*/
#define CTRL_SET        0x03 /*Set up control flag*/
#define CTRL_UA         0x07 /*Unnumbered acknowledgment control flag*/
#define CTRL_DISC       0x0B /*Disconnect control flag*/
#define CTRL_S(s)       ((s%2)<<6) /*0S000000 S = N(s)*/
#define CTRL_RR(r)      ((r%2)<<7) | BIT(2) | BIT(0) /*Receiver ready / positive ACK control flag*/
#define CTRL_REJ(r)      ((r%2)<<7) | BIT(0) /*Reject / negative ACK control flag */



/* Indexes */
#define BEGIN_FLAG_IDX  0
#define ADDR_IDX        1
#define CTRL_IDX        2
#define BCC1_IDX        3

#define BCC(a,c) (a^c) /*Calculation of BCC = XOR*/


/* Fixed Constants */
#define SUPERVISION_MSG_SIZE 5
#define ANY_VALUE       -1
#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define printError(args...) fprintf(stderr, ##args)

#define FALSE 0
#define TRUE 1
typedef unsigned bool;


/* Changeable Constants */
#define MAX_I_MSG_SIZE MAX_DATA_LENGTH + SUPERVISION_MSG_SIZE + 1 // the 1 is from the data bcc
#define MAX_I_BUFFER_SIZE MAX_I_MSG_SIZE*2 // needs to be this value due to the stuffing operation
#define TIME_OUT    3   // time between tries
#define NO_TRIES    3   // tries to send message 3 times
#define MAX_DATA_LENGTH     30   // max message data size (excluding header and tail)



enum stateMachine { Start, FLAG_RCV, A_RCV, C_RCV, BCC_HEAD_OK, DATA, DATA_OK, BCC_DATA_OK, DONE_S_U, DONE_I };
enum programState { WaitingForLC, LogicallyConnected, WaitingForDISC, WaitingForUA, WaitingForRR };
enum destuffingState { DestuffingOK, WaitingForSecondByte, ViewingDestuffedByte };
enum readFromSPRet{ RR, REJ, SAVE, READ_ERROR };

enum checkStateRET { 
    StateOK, 
    IGNORE_CHAR,
    // need to manage where state changed to and the implications on the buffer inside checkState 
    HEAD_INVALID,
    DATA_INVALID
};

#endif /* _DEFINES_H_*/
