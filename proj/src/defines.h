#ifndef _DEFINES_H_
#define _DEFINES_H_

#define BIT(n)      (0x01 << n )

#define SOCAT
#define DEBUG    
#define MAX_DEBUG_MSG_LENGTH 100

/* Message */
#define MSG_FLAG    0x7E /*all messages are delimited by this flag*/


/* Address */
#define ADDR_SENT_EM    0x03 /*Commands sent by emitter and answers sent by receiver*/
#define ADDR_SENT_RCV   0x01 /*Commands sent by receiver and answers sent by emitter*/


/* Control */
#define CTRL_SET        0x03
#define CTRL_UA         0x07 
#define CTRL_DISC       0x0B
#define CTRL_RR         0x03
#define CTRL_REJ        0x01
#define CTRL_S(s)      ((s%2)<<6) /*0S000000 S = N(s)*/
#define CTRL_R(r)      ((r%2)<<7) || BIT(3)



/* Indexes */
#define BEGIN_FLAG_IDX  0
#define ADDR_IDX        1
#define CTRL_IDX        2
#define BCC1_IDX        3

#define BCC(a,c) (a^c)


/* Fixed Constants */
#define SUPERVISION_MSG_SIZE 5
#define ANY_VALUE       -1
#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1


/* Changeable Constants */
#define MAX_I_MSG_SIZE MAX_DATA_LENGTH + SUPERVISION_MSG_SIZE + 1 // the 1 is from the data bcc
#define TIME_OUT    3
#define NO_TRIES    3
#define MAX_DATA_LENGTH     64



enum stateMachine { Start, FLAG_RCV, A_RCV, C_RCV, BCC_HEAD_OK, DATA, DATA_OK, BCC_DATA_OK, DONE_S_U, DONE_I };

#endif /* _DEFINES_H_*/
