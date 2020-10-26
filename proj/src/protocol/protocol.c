#include "protocol.h"

volatile int STOP = false;
unsigned stopAndWaitFlag = false;
int fd = -1;
char prevByte;
char * stateNames[] = { "Start", "FLAG_RCV", "A_RCV", "C_RCV", "BCC_HEAD_OK", "DATA", "DATA_OK", "BCC_DATA_OK", "DONE_S_U", "DONE_I" };
unsigned nextS = 0;

void alarmHandler(int signo) {
    if (signo == SIGALRM) {
        stopAndWaitFlag = true;
        debugMessage("[SIG HANDLER] SIGALRM");
    }
}

int openConfigureSP(char* port, struct termios *oldtio) {
    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */
    struct termios newtio; 

    fd = open(port, O_RDWR | O_NOCTTY );

    if (fd < 0) { perror(port); return -1; }

    if (tcgetattr(fd, oldtio) == -1) { /* save current port settings */
        perror("Error on tcgetattr");
        return -1;
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
    /*
        VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
        leitura do(s) pr�ximo(s) caracter(es)
        t = TIME * 0.1s
    */
    newtio.c_cc[VTIME]    = 0;   // if read only blocks for VTIME seconds, or until a character is received
    newtio.c_cc[VMIN]     = 0;   

    tcflush(fd, TCIOFLUSH);   // discards from the queue data received but not read and data written but not transmitted

    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        perror("tcsetattr");
        return -1;
    }

    debugMessage("[SP] OPENED AND CONFIGURED");

    return fd;
}

size_t writeToSP(char* message, size_t messageSize) {
    message[messageSize]='\0';

    return write(fd, message, messageSize*sizeof(message[0]));
}

enum readFromSPRet readFromSP(char * buf, enum stateMachine *state, ssize_t * stringSize, char addressField, char controlField) {
    char reading;
    int counter = 0;

    #ifdef DEBUG
    static int sendREJ = 4;
    #endif

    STOP = false;
    *state = Start;

    char bcc[2];

    static int pS = 1;
    //reads from the serial port
    while(STOP == false) {
        int readRet = read(fd, &reading, 1);

        if (stopAndWaitFlag) STOP=true; // if the alarm interrupts
        //printf("1\n");
        if (readRet < 0) {
            perror("Unsuccessful read");
            return READ_ERROR;
        } 
        else if (readRet == 0) continue; // if didn't read anything
        //printf("2\n");

        // if read is successful
        switch (checkState(state, bcc, &reading, buf, addressField, controlField)) {
            case HEAD_INVALID: // IGNORE ALL, can start reading next one
                #ifdef DEBUG_STATE_MACHINE
                debugMessage("HEAD_INVALID");
                #endif
                counter = 0;
                continue;
            case DATA_INVALID: // Evaluate head && act accordingly
                #ifdef DEBUG_STATE_MACHINE
                debugMessage("DATA_INVALID");
                #endif
                STOP = true;
                int s = getS(buf[CTRL_IDX]);
                if(s == pS){//send RR, confirme reception
                    //printf("COUNTER/SiZE: %d\n", counter);
                    return RR;
                }
                else{//send REJ,needs retransmission
                    //printf("COUNTER/SiZE: %d\n", counter);
                    return REJ;
                }
            case IGNORE_CHAR:
                #ifdef DEBUG_STATE_MACHINE
                debugMessage("IGNORE_CHAR");
                #endif
                continue;
            case StateOK:
                #ifdef DEBUG_STATE_MACHINE
                debugMessage("StateOK");
                #endif
            default: break;
            
        }

        #ifdef DEBUG_STATE_MACHINE
        printf("state after checkstate: %s\n", stateNames[*state]);
        #endif
        buf[counter++] = reading;
        if (isAcceptanceState(state)) break;
    }

    *stringSize = counter;
    if (isI(state)) {
        int s = getS(buf[CTRL_IDX]);
        //printf("Received s: %d, pS: %d\n", s, pS);
            
        if(s == pS){// send RR & discard
            //printf("COUNTER/SiZE S == PS: %d\n", counter);
            return RR;
        }
        else{// send RR and accept data
            //printf("COUNTER/SiZE ELSE: %d\n", counter);
            //printf("COUNTER/SiZE: %d\n", counter);
            #ifdef DEBUG
            if (sendREJ > 0) {
                sendREJ--;
                return REJ;
            }
            #endif
            pS = s; 
            return SAVE; 
        }
    } else if (isSU(state) && (isRR(buf[CTRL_IDX])|| isREJ(buf[CTRL_IDX]))) {
        int r = getR(buf[CTRL_IDX]);
        //printf("Received r: %d, nextS: %d\n", r, nextS%2);

        if (r != (nextS%2)) { // Repeated control -> R needs to be always equal to the S of the next information msg
            // RR (ignore)
            return RR;
        } else { // if it isREJ && new control (r == nextS)
            // act upon the result
            return isRR(buf[CTRL_IDX]) ? RR : REJ;
        }
    }

    return SAVE; // This means it was stopped by the stop and wait alarm OR it is either UA, SET or DISC
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
    ret[CTRL_IDX] = CTRL_S(nextS);
    ret[BCC1_IDX] = BCC(ret[2], ret[1]);

    for(size_t i = 0; i < *dataSize; i++){
        bcc = BCC(bcc, data[i]);
        ret[BCC1_IDX+1+i] = data[i];
    }

    ret[BCC1_IDX+*dataSize+1] = bcc;
    ret[BCC1_IDX+*dataSize+2] = MSG_FLAG;

    *dataSize += 6;
    nextS++;

    byteStuffing(ret, dataSize); // dataSize is updated in the byteStuffing function
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



int closeSP(struct termios *oldtio) {
    if (tcsetattr(fd, TCSANOW, oldtio) == -1) {
      perror("Error on tcsetattr");
      return -1;
    }

    if(close(fd) != 0){
        perror("Error on close");
        return -1;
    }
    return 0;
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

bool isRR(unsigned char ctrl) {
    return ctrl == 0x85 || ctrl == 0x05;
}

bool isREJ(unsigned char ctrl) {
    return ctrl == 0x81 || ctrl == 0x01;
}

int getS(unsigned char ctrl) {
    return ctrl >> 6;
}

int getR(unsigned char ctrl) {
    return ctrl >> 7;
}

bool checkDestuffedBCC(char* buf, char bcc, size_t dataCount, int noFlag){
    char aux = 0;
    if (dataCount == 0) return false;

    // Can be done like this because dataCount is the number of data bytes received, and that will always be true

    #ifdef DEBUG_STATE_MACHINE
    printf("BCC calculated with: ");
    #endif
    for(size_t i = 4; i < dataCount + 4; i++){
        #ifdef DEBUG_STATE_MACHINE
        printf("%x ", buf[i]);
        #endif
        aux ^= buf[i];
    }

    #ifdef DEBUG_STATE_MACHINE
    printf("\nCalculated BCC: %x, real BCC: %x\n", aux, bcc);
    #endif

    if(aux == bcc) return true;
    else return false;
    
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

    #ifdef DEBUG_STATE_MACHINE
    printf("\nSM: byte received: %x; currentState: %s\n", *byte, stateNames[*state]);
    #endif

    switch (*state){
    case Start:
        // Advances from Start when flag id received
        if(receivedMessageFlag(byte, destuffing)){
            *state = FLAG_RCV;
        } else {
            return HEAD_INVALID;
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
            dataCount = 1;
        }
        break;

    case DATA:
        if(!receivedMessageFlag(byte, destuffing)){
            dataCount++;
            if (dataCount >= MAX_DATA_PACKET_LENGTH) { *state = DATA_OK;}
            break;
        } else {
            dataCount--; // discounts the BCC from the data count
            dataBCC = checkDestuffedBCC(buf, prevByte, dataCount, 0);    
            if (dataBCC){
                *state = DONE_I;
            }
            else {
                goBackToStart(state, &destuffing);
                return DATA_INVALID;
            }
        }
        break;
    case DATA_OK:
        // verify BCC
        if (!receivedMessageFlag(byte, destuffing)) {
            dataBCC = checkDestuffedBCC(buf, *byte, dataCount, 1);
            if(dataBCC){
                *state = BCC_DATA_OK;   
            }
            else{
                goBackToStart(state, &destuffing);
                return DATA_INVALID;
            }
        } else {
            goBackToStart(state, &destuffing);
            return DATA_INVALID;
        }
        break;

    case BCC_DATA_OK:
        if (receivedMessageFlag(byte, destuffing)) {
            *state = DONE_I;
        } else {
            goBackToStart(state, &destuffing);
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