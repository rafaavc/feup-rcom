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

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1



#define SUPERVISION_MESSAGE_SIZE 5
#define TIME_OUT    3


#endif /* _DEFINES_H_*/