#include "protocol.h"

unsigned logicConnectionFlag = TRUE;

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
        exit(1);
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
    if (fd < 0) { perror(port); exit(-1); }

    if (tcgetattr(fd, oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */



    /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
    */

    tcflush(fd, TCIOFLUSH);   // discards from the queue data received but not read and data written but not transmitted

    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    return fd;
}

size_t writeToSP(int fd, char* message, size_t messageSize) {
    message[messageSize]='\0';

    return write(fd, message, (messageSize+1)*sizeof(message[0]));
}


char * readFromSP(int fd, ssize_t * stringSize) {
    char *buf = malloc(700*sizeof(char)), reading;

    //reads from the serial port
    int counter = 0;
    while(TRUE) {
        read(fd, &reading, 1);
        buf[counter] = reading;
        if (logicConnectionFlag) break;
        if(reading =='\0') break;

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
    msg[3] = MSG_FLAG ^ addr ^ ctrl;
    msg[4]= MSG_FLAG;

    return msg;
}

void closeSP(int fd, struct termios *oldtio) {
    if (tcsetattr(fd, TCSANOW, oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
}

