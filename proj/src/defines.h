#ifndef _DEFINES_H_
#define _DEFINES_H_

/* Message */
#define MSG_FLAG    0x7E /*all messages are delimited by this flag*/


/* Address */
#define ADDR_SENT_EM    0x03 /*Commands sent by emitter and answers sent by receiver*/
#define ADDR_SENT_RCV   0x01 /*Commands sent by receiver and answers sent by emitter*/

#define SOCAT           

#define CTRL_SET        0x03
#define CTRL_UA         0x07 
#define CTRL_DISC       0x0B
#define CTRL_RR         0x03
#define CTRL_REJ        0x01

#define DATA_LENGTH     64

#define ADDR_IDX        1
#define CTRL_IDX        2

#define ANY_VALUE       -1

#define BCC(a,c) (a^c)

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1



#define SUPERVISION_MESSAGE_SIZE 5
#define TIME_OUT    3
#define NO_TRIES    3


enum stateMachine{Start, FLAG_RCV, A_RCV, C_RCV, BCC_HEAD_OK, DATA, DATA_OK, BCC_DATA_OK, DONE_S_U, DONE_I};

#endif /* _DEFINES_H_*/
