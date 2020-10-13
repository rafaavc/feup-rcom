#include "protocol.h"

unsigned logicConnectionFlag = TRUE;
enum stateMachine_S_U state;
volatile int STOP=FALSE;

void checkCmdArgs(int argc, char ** argv) {
    char * ports[2] = {
        #ifndef SOCAT 
        "/dev/ttyS0",
        "/dev/ttyS1"
        #else
        "/dev/ttyS10",
        "/dev/ttyS11"
        #endif
    };
    if ( (argc < 2) || ((strcmp(ports[0], argv[1])!=0) && (strcmp(ports[1], argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(EXIT_FAILURE);
    }
}

int openConfigureSP(char* port, struct termios *oldtio) {
    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */
    int fd;
    struct termios newtio; 

    fd = open(port, O_RDWR | O_NOCTTY );
    if (fd < 0) { perror(port); exit(EXIT_FAILURE); }

    if (tcgetattr(fd, oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    // t = TIME * 0.1 s
    newtio.c_cc[VTIME]    = 20;   // if read only blocks for 2 seconds, or until a character is received
    newtio.c_cc[VMIN]     = 0;   


    /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
    */

    tcflush(fd, TCIOFLUSH);   // discards from the queue data received but not read and data written but not transmitted

    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    printf("New termios structure set\n");

    return fd;
}

size_t writeToSP(int fd, char* message, size_t messageSize) {
    message[messageSize]='\0';

    return write(fd, message, (messageSize+1)*sizeof(message[0]));
}

char * readFromSP(int fd, ssize_t * stringSize, int emitter) {// emitter is 1 if it's the emitter reading and 0 if it's the receiver
    char *buf = malloc(700*sizeof(char)), reading;
    int counter = 0;
    char bcc[2];

    STOP = FALSE;

    //reads from the serial port
    while(STOP == FALSE) {
        int readRet = read(fd, &reading, 1);

        if (logicConnectionFlag) STOP=TRUE; // if the alarm interrupts

        if (readRet <= 0) break; // if read was not successful

        // if read is successful

        checkState(&state, bcc, reading, emitter);
        
        if(state == DONE || logicConnectionFlag) STOP = TRUE;

        buf[counter] = reading;
        counter++;
    }

    (*stringSize) = counter+1;
    return buf;
}

char *  constructSupervisionMessage(char addr, char ctrl){
    char* msg = malloc(sizeof(char)*SUPERVISION_MESSAGE_SIZE);

    msg[0] = MSG_FLAG;
    msg[1] = addr;
    msg[2] = ctrl;
    msg[3] = BCC(addr, ctrl);
    msg[4]= MSG_FLAG;

    return msg;
}

void closeSP(int fd, struct termios *oldtio) {
    if (tcsetattr(fd, TCSANOW, oldtio) == -1) {
      perror("tcsetattr");
      exit(EXIT_FAILURE);
    }

    close(fd);
}



void checkState_begin(enum stateMachine_S_U *state, char *bcc, char byte, int emitter){ 
    switch (*state){
    case Start:
        if(byte == MSG_FLAG){
            *state = FLAG_RCV;
        }
        break;
    
    case FLAG_RCV:
        if(emitter == 1){
            if(byte == ADDR_SENT_RCV){
                *state = A_RCV;
                bcc[0] = byte;
                break;
            }
            else if(byte != MSG_FLAG){
                *state = Start;
                break;
            }
            break;
        }
        else if (emitter == 0){
            if(byte == ADDR_SENT_EM){
                *state = A_RCV;
                bcc[0] = byte;
                break;
            }
            else if(byte != MSG_FLAG){
                *state = Start;
                break;
            }
            break;

        }
        break;
        
    case A_RCV:
        if(emitter == 1){
            if(byte == CTRL_UA){
                *state = C_RCV;
                bcc[1] = byte;
                break;
            }
            else if (byte == MSG_FLAG){
                *state = FLAG_RCV;
                break;
            }
            else{
                *state = Start;
                break;
            }

        }
        else if (emitter == 0){
            if(byte == CTRL_SET){
                *state = C_RCV;
                bcc[1] = byte;
                break;
            }
            else if (byte == MSG_FLAG){
                *state = FLAG_RCV;
                break;
            }
            else{
                *state = Start;
                break;
            }
        }
        break;
    case C_RCV:
        if(byte == BCC(bcc[0], bcc[1])){
            *state = BCC_OK;
            break;
        }
        else if(byte == MSG_FLAG){
            *state = FLAG_RCV;
            break;
        }
        else{
            *state = Start;
            break;
        }
        break;
    case BCC_OK:
        if(byte == MSG_FLAG){
            *state = DONE;
            break;
        }
        else{
            *state = Start;
            break;
        }
        break;
    case DONE:
        break;
    default:
        break;
    }
}



void checkState_end(enum stateMachine_S_U *state, char *bcc, char byte){  
    switch (*state){
    case Start:
        if(byte == MSG_FLAG){
            *state = FLAG_RCV;
        }
        break;
    
    case FLAG_RCV:
        if(byte == ADDR_SENT_RCV){
            *state = A_RCV;
            bcc[0] = byte;
            break;
        }
        else if(byte != MSG_FLAG){
            *state = Start;
            break;
        }
        break;
       
        
    case A_RCV:
        if(byte == CTRL_DISC){
            *state = C_RCV;
            bcc[1] = byte;
            break;
        }
        else if (byte == MSG_FLAG){
            *state = FLAG_RCV;
            break;
        }
        else{
            *state = Start;
            break;
        }

        
    case C_RCV:
        if(byte == BCC(bcc[0], bcc[1])){
            *state = BCC_OK;
            break;
        }
        else if(byte == MSG_FLAG){
            *state = FLAG_RCV;
            break;
        }
        else{
            *state = Start;
            break;
        }
        break;
    case BCC_OK:
        if(byte == MSG_FLAG){
            *state = DONE;
            break;
        }
        else{
            *state = Start;
            break;
        }
        break;
    case DONE:
        break;
    default:
        break;
    }
}


void checkState_data(enum stateMachine_I *state, char *bcc, char byte, int emitter){ 
    //TO DO: change this to have all the fields of the data
    switch (*state){
    case Start:
        if(byte == MSG_FLAG){
            *state = FLAG_RCV;
        }
        break;
    
    case FLAG_RCV:
        if(emitter == 1){
            if(byte == ADDR_SENT_RCV){
                *state = A_RCV;
                bcc[0] = byte;
                break;
            }
            else if(byte != MSG_FLAG){
                *state = Start;
                break;
            }
            break;
        }
        else if (emitter == 0){
            if(byte == ADDR_SENT_EM){
                *state = A_RCV;
                bcc[0] = byte;
                break;
            }
            else if(byte != MSG_FLAG){
                *state = Start;
                break;
            }
            break;

        }
        break;
        
    case A_RCV:
        if(emitter == 1){
            if(byte == CTRL_UA){
                *state = C_RCV;
                bcc[1] = byte;
                break;
            }
            else if (byte == MSG_FLAG){
                *state = FLAG_RCV;
                break;
            }
            else{
                *state = Start;
                break;
            }

        }
        else if (emitter == 0){
            if(byte == CTRL_SET){
                *state = C_RCV;
                bcc[1] = byte;
                break;
            }
            else if (byte == MSG_FLAG){
                *state = FLAG_RCV;
                break;
            }
            else{
                *state = Start;
                break;
            }
        }
        break;
    case C_RCV:
        if(byte == BCC(bcc[0], bcc[1])){
            *state = BCC_OK;
            break;
        }
        else if(byte == MSG_FLAG){
            *state = FLAG_RCV;
            break;
        }
        else{
            *state = Start;
            break;
        }
        break;
    case BCC_OK:
        if(byte == MSG_FLAG){
            *state = DONE;
            break;
        }
        else{
            *state = Start;
            break;
        }
        break;
    case DONE:
        break;
    default:
        break;
    }
}