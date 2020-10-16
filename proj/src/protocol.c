#include "protocol.h"

volatile int STOP = FALSE;
unsigned logicConnectionFlag = FALSE;

int s = 0;

char prevByte;
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
    newtio.c_cc[VTIME]    = 0;   // if read only blocks for 2 seconds, or until a character is received
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

    #ifdef DEBUG
    debugMessage("[SP] OPENED AND CONFIGURED");
    #endif

    return fd;
}

size_t writeToSP(int fd, char* message, size_t messageSize) {
    message[messageSize]='\0';

    return write(fd, message, (messageSize+1)*sizeof(message[0]));
}

void readFromSP(int fd, char * buf, enum stateMachine *state, ssize_t * stringSize, char addressField, char controlField) {// emitter is 1 if it's the emitter reading and 0 if it's the receiver
    char reading;
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

        if (checkState(state, bcc, reading, addressField, controlField) != 0) continue;//tramas com cabeçalho errado sao ignoradas
        //printf("state: %ud\n", state);


        //printf("3\n");
        if(isAcceptanceState(state)) STOP = TRUE;

        buf[counter] = reading;
        counter++;
    }

    if (!isAcceptanceState(state)) {
        buf = NULL;
        (*stringSize) = 0;
        return;
    }

    (*stringSize) = counter+1;
}

void constructSupervisionMessage(char * ret, char addr, char ctrl){
    ret[BEGIN_FLAG_IDX] = MSG_FLAG;
    ret[ADDR_IDX] = addr;
    ret[CTRL_IDX] = ctrl;
    ret[BCC1_IDX] = BCC(addr, ctrl);
    ret[BCC1_IDX+1] = MSG_FLAG;
}

void constructInformationMessage(char* ret ,char* data, size_t dataSize){//ret size = 5 + dataSize
    if(dataSize == 0) return;
    char bcc;

    ret[BEGIN_FLAG_IDX] = MSG_FLAG;
    ret[ADDR_IDX] = ADDR_SENT_EM;
    ret[CTRL_IDX] = CTRL_S(s);
    ret[BCC1_IDX] = BCC(ret[2], ret[1]);

    for(size_t i = 0; i < dataSize; i++){
        ret[BCC1_IDX+1+i] = data[i];
        char aux = data[i];
        if(i == 1) bcc = aux;
        else bcc = BCC(bcc, aux);
    }

    ret[BCC1_IDX+dataSize+1] = bcc;
    ret[BCC1_IDX+dataSize+2] = MSG_FLAG;

    byteStuffing(ret, dataSize);
    
    s++;

}

void byteStuffing(char* ret, size_t dataSize){//confirmar estes andamentos de um para a frente
/*Provavelmente ha uma maneira melhor de fazer isto com shifts mas por agora isto faz sentido na minha cabeça*/
    char* buf = ret;
    int startDataIdx = BCC1_IDX + 1;
    int dataIdx = startDataIdx;

    buf[BEGIN_FLAG_IDX] = ret[BEGIN_FLAG_IDX];
    buf[ADDR_IDX] = ret[ADDR_IDX];
    buf[CTRL_IDX] = ret[CTRL_IDX];
    buf[BCC1_IDX] = ret[BCC1_IDX];

    for(int i = startDataIdx; i < startDataIdx + dataSize; i++){

        if(ret[i] == 0x7E){// caso de ser igual ao padrão que corresponde a uma flag
            buf[dataIdx++] = 0x7D;
            buf[dataIdx++] = 0x5E;
        }
        else if (ret[i] == 0x7D){//caso de ser igual ao padrao que corresponde ao octeto de escape
            buf[dataIdx++] = 0x7D;
            buf[dataIdx++] = 0x5D;
        }
        else{
            buf[dataIdx++] = ret[i];
        }
    }
    buf[dataIdx++] = ret[startDataIdx+dataSize];//BCC2 fica igual
    buf[dataIdx] = MSG_FLAG;

    ret = buf;
    return;
}


char* byteDestuffing(char * ret, size_t dataSize){// dataSize = tamanho do array recebido - 6-->2flag, addr, ctr, bcc1, bcc2
    char* buf = ret;
    int startDataIdx = BCC1_IDX + 1;
    int dataIdx = startDataIdx;   

    buf[BEGIN_FLAG_IDX] = ret[BEGIN_FLAG_IDX];
    buf[ADDR_IDX] = ret[ADDR_IDX];
    buf[CTRL_IDX] = ret[CTRL_IDX];
    buf[BCC1_IDX] = ret[BCC1_IDX];

    for(int i = startDataIdx; i < startDataIdx + dataSize; i++){
        if(ret[i] == 0x7D){
            if(ret[i+1] == 0x5E){
                buf[dataIdx++] = 0x7E;
                i++; 
                continue;
            }
            else if(ret[i+1] == 0x5D){
                buf[dataIdx++] = 0x7D;
                i++;
                continue;
            }
        }
        buf[dataIdx++] = ret[i];

    } 
    return buf;
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

unsigned checkState(enum stateMachine *state, char * bcc, char byte, char addressField, char controlField) { 
    //checkar melhor o bcc
    static unsigned dataCount = 0;
    static char dataBCC = 0;

    enum stateMachine prevState = *state;

    prevByte = byte;

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
            //dataBCC ^= byte;
            if (dataCount >= MAX_DATA_LENGTH) { *state = DATA_OK; dataCount = 0; dataBCC = 0; }
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