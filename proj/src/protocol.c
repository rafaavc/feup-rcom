#include "protocol.h"

unsigned logicConnectionFlag = FALSE;

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

char * readFromSP(int fd, enum stateMachine *state, ssize_t * stringSize, unsigned addressField, unsigned controlField) {// emitter is 1 if it's the emitter reading and 0 if it's the receiver
    char *buf = malloc(700*sizeof(char)), reading;
    int counter = 0;

    STOP = FALSE;
    *state = Start;

    char bcc[2];

    //reads from the serial port
    while(STOP == FALSE) {
        int readRet = read(fd, &reading, 1);

        if (logicConnectionFlag) STOP=TRUE; // if the alarm interrupts
        //printf("1\n");
        if (readRet < 0) {
            //perror("Unsuccessful read");
            break;
        } else if (readRet == 0) continue; // if didn't read anything
        //printf("2\n");

        // if read is successful

        if (checkState(state, bcc, reading, addressField, controlField) != 0) continue;
        //printf("state: %ud\n", state);


        //printf("3\n");
        if(isAcceptanceState(state)) STOP = TRUE;

        buf[counter] = reading;
        counter++;
    }

    if (!isAcceptanceState(state)) {
        (*stringSize) = 0;
        return NULL;
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
    msg[4] = MSG_FLAG;

    return msg;
}

void closeSP(int fd, struct termios *oldtio) {
    if (tcsetattr(fd, TCSANOW, oldtio) == -1) {
      perror("tcsetattr");
      exit(EXIT_FAILURE);
    }

    close(fd);
}

unsigned isAcceptanceState(enum stateMachine *state) {
    return *state == DONE_I || *state == DONE_S_U;
}

unsigned isI(enum stateMachine *state) {
    return *state == DONE_I;
}
unsigned isSU(enum stateMachine *state) {
    return *state == DONE_S_U;
}

unsigned checkState(enum stateMachine *state, char * bcc, char byte, unsigned addressField, unsigned controlField) { 
    // emitter is 1 if it's the emitter reading and 0 if it's the receiver
    //checkar melhor o bcc
    static dataCount = 0;
    static dataBCC = 0;

    enum stateMachine prevState = *state;


    
    switch (*state){
    case Start:
        if(byte == MSG_FLAG){
            *state = FLAG_RCV;
            //printf("flag ok\n");
        }
        break;
    
    case FLAG_RCV:
        if(byte == addressField || addressField == ANY_VALUE){
            *state = A_RCV;
            bcc[0] = byte;
            //printf("address ok\n");
        }
        else if(byte != MSG_FLAG){
            *state = Start;
        }
        break;
        
    case A_RCV:
        if (byte == controlField || controlField == ANY_VALUE) {
            *state = C_RCV;
            bcc[1] = byte;
            //printf("control ok\n");
        }
        else if (byte == MSG_FLAG) {
            *state = FLAG_RCV;
        }
        else {
            *state = Start;
        }
        break;

    case C_RCV:
        if(byte == BCC(bcc[0], bcc[1])){
            *state = BCC_HEAD_OK;
            //printf("bcc ok\n");
        }
        else if(byte == MSG_FLAG){
            *state = FLAG_RCV;
            //printf("received MSG_FLAG :(\n");
        }
        else{
            *state = Start;
            //printf("received %x, bcc: %x\n", byte, BCC(bcc[0], bcc[1]));
        }
        break;

    case BCC_HEAD_OK:
        if(byte == MSG_FLAG){
            *state = DONE_S_U; // S and U
        }
        else{
            *state = DATA; // I
        }
        break;

    case DATA:
        if(byte == MSG_FLAG){ 
            *state = FLAG_RCV;
            dataCount = 0;
            dataBCC = 0;
        } else {
            dataCount++;
            dataBCC ^= byte;
            if (dataCount >= DATA_LENGTH) { *state = DATA_OK; dataCount = 0; dataBCC = 0; }
        }
        break;

    case DATA_OK:
        if (byte == dataBCC) {
            *state = BCC_DATA_OK;
        } else {
            *state = Start;
        }
        break;

    case BCC_DATA_OK:
        if (byte == MSG_FLAG) {
            *state = DONE_I;
        } else {
            *state = Start;
        }
        break;

    case DONE_I:
    case DONE_S_U:
    default:
        break;
    }

    if ((*state == FLAG_RCV && prevState != Start) || *state == Start) { // if the state was restarted
        return 1;
    }
    return 0;
}