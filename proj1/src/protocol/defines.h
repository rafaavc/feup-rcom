#ifndef _DEFINES_H_
#define _DEFINES_H_


#define TRANSMITTER     0
#define RECEIVER        1

#define BIT(n)      (0x01 << (n))

#define MAX_DEBUG_MSG_LENGTH 100

/* Message */
#define MSG_FLAG    0x7E /*all messages are delimited by this flag*/


/* Address */
#define ADDR_SENT_EM    0x03 /*Commands sent by transmitter and answers sent by receiver*/
#define ADDR_SENT_RCV   0x01 /*Commands sent by receiver and answers sent by transmitter*/


/* Control Protocol*/
#define CTRL_SET        0x03 /*Set up control flag*/
#define CTRL_UA         0x07 /*Unnumbered acknowledgment control flag*/
#define CTRL_DISC       0x0B /*Disconnect control flag*/
#define CTRL_S(s)       (s<<6) /*0S000000 S = N(s)*/
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
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define printError(args...) fprintf(stderr, ##args)

#endif /* _DEFINES_H_*/
