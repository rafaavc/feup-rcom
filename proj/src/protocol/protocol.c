#include "protocol.h"
#include <errno.h>


#ifdef DEBUG_STATE_MACHINE
static char * stateNames[] = { "Start", "FLAG_RCV", "A_RCV", "C_RCV", "BCC_HEAD_OK", "DATA", "DATA_OK", "BCC_DATA_OK", "DONE_S_U", "DONE_I" };
#endif

volatile bool stopAndWaitFlag = false;

int openConfigureSP(char* port) {
    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */

    struct termios newtio; 

    setFD(open(port, O_RDWR | O_NOCTTY));

    if (getFD() < 0) { perror(port); return -1; }

    if (tcgetattr(getFD(), getOldTIO()) == -1) { /* save current port settings */
        perror("Error on tcgetattr");
        return -1;
    }

    atexit((void *)&closeSP); //resets configuration when program terminates with exit(EXIT_FAILURE)

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = getBaudrate() | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
    /*
        VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
        leitura do(s) pr�ximo(s) caracter(es)
        t = TIME * 0.1s
    */
    newtio.c_cc[VTIME]    = 0;   // if read only blocks for VTIME*0.1 seconds, or until a character is received
    newtio.c_cc[VMIN]     = 1;   // blocks until a character is read

    tcflush(getFD(), TCIOFLUSH);   // discards from the queue data received but not read and data written but not transmitted

    if (tcsetattr(getFD(), TCSANOW, &newtio) == -1) {
        perror("tcsetattr");
        return -1;
    }

    debugMessage("[SP] OPENED AND CONFIGURED");

    return getFD();
}

size_t writeToSP(char* message, size_t messageSize) {
    return write(getFD(), message, messageSize*sizeof(message[0]));
}

enum readFromSPRet readFromSP(char * buf, enum stateMachine *state, ssize_t * stringSize, char addressField, char controlField) {
    char reading;
    int counter = 0;

    #ifdef DEBUG
    static int sendREJ = 0;   // Simulates sending REJ (if > 0, sends REJ)
    #endif

    bool STOP = false;
    *state = START;

    char bcc[2];

    static int pS = 1;
    //reads from the serial port
    while(!STOP) {
        int readRet = read(getFD(), &reading, 1);

        if (stopAndWaitFlag) STOP = true; // if the alarm interrupts
        if (readRet < 0 && errno != EINTR) {
            perror("Unsuccessful read");
            return READ_ERROR;
        } 
        else if (readRet == 0) continue; // if didn't read anything

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
                if(s == pS){  //send RR, confirm reception
                    //printf("S == pS (RR): %d == %d\n", s, pS);
                    return RR;
                }
                else{  //send REJ, needs retransmission
                    //printf("S != pS (REJ): %d == %d\n", s, pS);
                    return REJ;
                }
            case IGNORE_CHAR:
                #ifdef DEBUG_STATE_MACHINE
                debugMessage("IGNORE_CHAR");
                #endif
                continue;
            case StateOK:   // adds the byte to the buffer
                #ifdef DEBUG_STATE_MACHINE
                debugMessage("StateOK");
                #endif
            default: break;
            
        }

        #ifdef DEBUG_STATE_MACHINE
        printf("state after checkstate: %s, counter: %d\n", stateNames[*state], counter);
        #endif
        buf[counter++] = reading;
        if (isAcceptanceState(state)) break;
    }

    *stringSize = counter;
    if (isI(state)) {
        int s = getS(buf[CTRL_IDX]);
        //printf("Received s: %d, pS: %d\n", s, pS);
            
        if(s == pS){  // s == previousS -> send RR & discard
            //printf("COUNTER/SIZE S == PS: %d\n", counter);
            return RR;
        }
        else{ // send RR and accept data
            //printf("COUNTER/SIZE ELSE: %d\n", counter);
            //printf("COUNTER/SIZE: %d\n", counter);
            #ifdef DEBUG   // simulates REJ
            if (sendREJ > 0) {
                sendREJ--;
                return REJ;
            }
            #endif
            pS = s; // updates s
            return SAVE; 
        }
    } else if (isSU(state) && (isRR(buf[CTRL_IDX])|| isREJ(buf[CTRL_IDX]))) {   // Received ack / nack  -> this is the transmitter instance
        int r = getR(buf[CTRL_IDX]);
        //printf("Received r: %d, nextS: %d\n", r, getNextSequenceNumber());

        if (r != (getNextSequenceNumber())) { // Repeated control -> R needs to be always equal to the S of the next information msg
            // RR (ignore)
            return RR;
        } else { // if it isREJ or RR && new control (r == nextS)
            // act upon the result
            return isRR(buf[CTRL_IDX]) ? RR : REJ;
        }
    }

    // This means it was stopped by the stop and wait alarm OR it is either UA, SET or DISC
    return STOPPED_OR_SU; 
}

int closeSP() {
    if (fcntl(getFD(), F_GETFD) != 0 && errno == EBADF) return -1; // verifies if fd is valid
    sleep(1);
    if (tcsetattr(getFD(), TCSANOW, getOldTIO()) == -1) {
      perror("Error on tcsetattr");
      return -1;
    }

    if(close(getFD()) != 0){
        perror("Error on close");
        return -1;
    }
    return 0;
}

void constructSupervisionMessage(char * ret, char addr, char ctrl) {
    ret[BEGIN_FLAG_IDX] = MSG_FLAG;
    ret[ADDR_IDX] = addr;
    ret[CTRL_IDX] = ctrl;
    ret[BCC1_IDX] = BCC(addr, ctrl);
    ret[BCC1_IDX+1] = MSG_FLAG;
}

void constructInformationMessage(char* ret ,char* data, size_t * dataSize) {//ret size = 6 + dataSize OR NOT (if stuffing actually replaces bytes)
    if(*dataSize == 0) {
        printError("Tried to construct an information message without any data.\n"); 
        return;
    }
    char bcc = 0;

    ret[BEGIN_FLAG_IDX] = MSG_FLAG;
    ret[ADDR_IDX] = ADDR_SENT_EM;
    ret[CTRL_IDX] = CTRL_S(getNextSequenceNumber());
    ret[BCC1_IDX] = BCC(ret[2], ret[1]);

    for (size_t i = 0; i < *dataSize; i++) {  // calculates the data bcc (between all data bytes)
        bcc = BCC(bcc, data[i]);
        ret[BCC1_IDX+1+i] = data[i];
    }
    ret[BCC1_IDX+*dataSize+1] = bcc;
    ret[BCC1_IDX+*dataSize+2] = MSG_FLAG;

    *dataSize += 6;
    incrementNextSequenceNumber();

    byteStuffing(ret, dataSize); // dataSize is updated in the byteStuffing function
}

void byteStuffing(char * ret, size_t * retSize){
    char * buf = myMalloc(getMaxFrameBufferSize()*sizeof(char));
    buf[0] = MSG_FLAG;
    unsigned bufferIdx = 1;

    for (unsigned i = bufferIdx; i < (*retSize)-1; i++){
        if(ret[i] == 0x7E){// case it is equal to the flag pattern
            buf[bufferIdx++] = 0x7D;
            buf[bufferIdx++] = 0x5E;
        }
        else if (ret[i] == 0x7D){// case it is equal to the escape pattern
            buf[bufferIdx++] = 0x7D;
            buf[bufferIdx++] = 0x5D;
        } else {
            buf[bufferIdx++] = ret[i];
        }
    }
    buf[bufferIdx] = MSG_FLAG;
    *retSize = bufferIdx+1;
    memcpy(ret, buf, *retSize);
    free(buf);
}




