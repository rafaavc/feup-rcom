#include "protocol.h"

volatile int STOP = FALSE;
unsigned stopAndWaitFlag = FALSE;

int s = 0, fd;

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

    debugMessage("[SP] OPENED AND CONFIGURED");

    return fd;
}

size_t writeToSP(int fd, char* message, size_t messageSize) {
    message[messageSize]='\0';

    return write(fd, message, (messageSize+1)*sizeof(message[0]));
}

enum readFromSPRet readFromSP(int fd, char * buf, enum stateMachine *state, ssize_t * stringSize, char addressField, char controlField) {// emitter is 1 if it's the emitter reading and 0 if it's the receiver
    char reading;
    int counter = 0;

    STOP = FALSE;
    *state = Start;

    char bcc[2];

    int pS;
    //reads from the serial port
    while(STOP == FALSE) {
        int readRet = read(fd, &reading, 1);

        if (stopAndWaitFlag) STOP=TRUE; // if the alarm interrupts
        //printf("1\n");
        if (readRet < 0) {
            perror("Unsuccessful read");
            break;
        } 
        else if (readRet == 0) continue; // if didn't read anything
        //printf("2\n");

        // if read is successful
        switch (checkState(state, bcc, &reading, buf, addressField, controlField)) {
            case HEAD_INVALID: // IGNORE ALL, can start reading next one
                counter = 0;
                continue;
            case DATA_INVALID: // Evaluate head && act accordingly
                STOP = TRUE;
                continue;
            case IGNORE_CHAR:
                continue;
            case StateOK:
            default: break;
        }
        //printf("state: %ud\n", state);


        //printf("3\n");
      
        buf[counter] = reading;
        counter++;

        if (isAcceptanceState(state))
            STOP = TRUE;

    }

    if(isAcceptanceState(state)) {   
        if(s == pS){// send RR & discard
            return RR;
        }
        else{// send RR and accept data 
            return SAVE; 
        }
        STOP = TRUE;
    }
    else{
        if(s == pS){//send RR 
            return RR;
        }
        else{//send REJ 
            return REJ;
        }
    }
}

void constructSupervisionMessage(char * ret, char addr, char ctrl){
    ret[BEGIN_FLAG_IDX] = MSG_FLAG;
    ret[ADDR_IDX] = addr;
    ret[CTRL_IDX] = ctrl;
    ret[BCC1_IDX] = BCC(addr, ctrl);
    ret[BCC1_IDX+1] = MSG_FLAG;
}

void constructInformationMessage(char* ret ,char* data, size_t * dataSize){//ret size = 6 + dataSize OR NOT (if stuffing actually replaces bytes)
    if(*dataSize == 0) return;
    char bcc = 0;

    ret[BEGIN_FLAG_IDX] = MSG_FLAG;
    ret[ADDR_IDX] = ADDR_SENT_EM;
    ret[CTRL_IDX] = CTRL_S(s);
    ret[BCC1_IDX] = BCC(ret[2], ret[1]);

    for(size_t i = 0; i < *dataSize; i++){
        if (data[i] == '\0') debugMessage("data string brings \\0 character");
        bcc = BCC(bcc, data[i]);
        ret[BCC1_IDX+1+i] = data[i];
    }

    ret[BCC1_IDX+*dataSize+1] = bcc;
    ret[BCC1_IDX+*dataSize+2] = MSG_FLAG;

    *dataSize += 6;

    byteStuffing(ret, dataSize); // dataSize is updated in the byteStuffing function

    s++;
}

void byteStuffing(char * ret, size_t * retSize){//confirmar estes andamentos de um para a frente
/*Provavelmente ha uma maneira melhor de fazer isto com shifts mas por agora isto faz sentido na minha cabeça*/
    char buf[MAX_I_BUFFER_SIZE];

    buf[0] = MSG_FLAG;
    unsigned bufferIdx = 1;

    for (unsigned i = bufferIdx; i < (*retSize)-1; i++){
        if(ret[i] == 0x7E){// caso de ser igual ao padrão que corresponde a uma flag
            buf[bufferIdx++] = 0x7D;
            buf[bufferIdx++] = 0x5E;
        }
        else if (ret[i] == 0x7D){//caso de ser igual ao padrao que corresponde ao octeto de escape
            buf[bufferIdx++] = 0x7D;
            buf[bufferIdx++] = 0x5D;
        } else {
            buf[bufferIdx++] = ret[i];
        }
    }
    buf[bufferIdx] = MSG_FLAG;
    *retSize = bufferIdx+1;
    memcpy(ret, buf, *retSize);
}

// this function is not needed, but is here for reference
void byteDestuffing(char * ret, size_t * retSize){// dataSize = tamanho do array recebido - 6-->2flag, addr, ctr, bcc1, bcc2
    unsigned amountOfDestuffedChars = 0;
    for(int i = 0; i < (*retSize); i++){
        char val = ret[i];
        ret[i] = '\0';
        if(val == 0x7D){
            if(ret[i+1] == 0x5E || ret[i+1] == 0x5D){
                ret[i-amountOfDestuffedChars] =  ret[i+1] == 0x5E ? 0x7E : 0x7D;
                ret[i+1] = '\0';
                i++;
                amountOfDestuffedChars++;
                continue;
            }
        }
        ret[i-amountOfDestuffedChars] = val;
    }
    *retSize -= amountOfDestuffedChars;
}



void closeSP(struct termios *oldtio) {
    if (tcsetattr(fd, TCSANOW, oldtio) == -1) {
      perror("tcsetattr");
      exit(EXIT_FAILURE);
    }

    close(fd);
}

bool isAcceptanceState(enum stateMachine *state) {
    return *state == DONE_I || *state == DONE_S_U;
}

bool isI(enum stateMachine *state) {
    return *state == DONE_I;
}
bool isSU(enum stateMachine *state) {
    return *state == DONE_S_U;
}

bool checkDestuffedBCC(char* buf, char bcc, size_t bufSize, int noFlag){
    char aux = buf[4];

    if(noFlag == 0){//caso de ja ter recebido MSG_FLAG
        for(size_t i = 5; i < bufSize - 2; i++){
            aux ^= buf[i];
        }
    }
    else if(noFlag == 1){//caso de ter recebido agora o BCC
        for(size_t i = 5; i < bufSize-1; i++){
            aux ^= buf[i];
        }
    }
    if(aux == bcc) return TRUE;
    else return FALSE;
    
}

bool receivedMessageFlag(char * byte, enum destuffingState destuffing) {
    return *byte == MSG_FLAG && destuffing == DestuffingOK;
}

void goBackToStart(enum stateMachine * state, enum destuffingState * destuffing) {
    *state = Start;
    *destuffing = DestuffingOK;
}

void goBackToFLAG_RCV(enum stateMachine * state, enum destuffingState * destuffing) {
    *state = FLAG_RCV;
    *destuffing = DestuffingOK;
}

enum checkStateRET checkState(enum stateMachine *state, char * bcc, char * byte, char*buf, char addressField, char controlField) { 
    static unsigned dataCount = 4;
    static bool dataBCC = 0;
    static enum destuffingState destuffing = DestuffingOK;

    //enum stateMachine prevState = *state;

    switch (destuffing) {
        case DestuffingOK:
            if (*byte == 0x7D){
                destuffing = WaitingForSecondByte;
                return IGNORE_CHAR;
            }
            break;
        case WaitingForSecondByte:
            if (*byte == 0x5E){
                *byte = 0x7E;
            } else if (*byte == 0x5D){
                *byte = 0x7D;
            } else {
                printf("Error while destuffing in checkstate!\n");
            }
            destuffing = ViewingDestuffedByte;
            break;
        case ViewingDestuffedByte:
            destuffing = DestuffingOK;
        default: 
            break;
    }


    switch (*state){
    case Start:
        // Advances from Start when flag id received
        if(receivedMessageFlag(byte, destuffing)){
            *state = FLAG_RCV;
        }
        break;
    
    case FLAG_RCV:
        // Only advances when a valid address field is received
        if(!receivedMessageFlag(byte, destuffing)
            && (*byte == addressField || addressField == ANY_VALUE)) {
            *state = A_RCV;
            bcc[0] = *byte;
        }
        else if(!receivedMessageFlag(byte, destuffing)){ // If it receives a flag, it doesn't change state
            goBackToStart(state, &destuffing);
            return HEAD_INVALID;
        }
        break;
        
    case A_RCV:
        // Only advances when a valid control field is received
        if (!receivedMessageFlag(byte, destuffing) && 
            (*byte == controlField || controlField == ANY_VALUE)) {
            *state = C_RCV;
            bcc[1] = *byte;
            //printf("control ok\n");
        }
        else { // INVALID
            receivedMessageFlag(byte, destuffing) ? 
                goBackToFLAG_RCV(state, &destuffing) : 
                goBackToStart(state, &destuffing);
            return HEAD_INVALID;
        }
        break;

    case C_RCV:
        // Only advances if BCC is correct
        if (*byte == BCC(bcc[0], bcc[1]) && !receivedMessageFlag(byte, destuffing)) {
            *state = BCC_HEAD_OK;
        }
        else { // INVALID
            receivedMessageFlag(byte, destuffing) ? 
                goBackToFLAG_RCV(state, &destuffing) : 
                goBackToStart(state, &destuffing);
            return HEAD_INVALID;
        }
        break;

    case BCC_HEAD_OK:
        if (receivedMessageFlag(byte, destuffing)) {
            *state = DONE_S_U; // S and U
        }
        else {
            *state = DATA; // I
            dataCount++;
        }
        break;

    case DATA:
        if(!receivedMessageFlag(byte, destuffing)){
            dataCount++;
            if (dataCount >= MAX_DATA_LENGTH) { *state = DATA_OK;}
            break;
        } 
        // break is missing intentionally!
    case DATA_OK:
        // verify BCC
        if(receivedMessageFlag(byte, destuffing)){
            dataBCC = checkDestuffedBCC(buf, prevByte, dataCount, 0);    
             if(dataBCC ){
                dataCount = 0;
                *state = DONE_I;
             }
             else{

                 return DATA_INVALID;
             }
        }
        else{
            dataBCC = checkDestuffedBCC(buf, *byte, dataCount, 1);
            if(dataBCC){
                *state = BCC_DATA_OK;   
            }
            else{
                goBackToStart(byte, &destuffing);
                return DATA_INVALID;
            }   
            dataCount = 0; 
        }
        break;

    case BCC_DATA_OK:
        if (receivedMessageFlag(byte, destuffing)) {
            *state = DONE_I;
        } else {
            goBackToStart(byte, destuffing);
            return DATA_INVALID;
        }
        break;

    case DONE_I:
    case DONE_S_U:
        destuffing = DestuffingOK;
    default:
        break;
    }
    prevByte = *byte;

    return StateOK;
}