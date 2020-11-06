#include "stateMachine.h"

extern int MAX_DATA_PACKET_SIZE;

char prevByte;

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
    return ctrl == 0x85 || ctrl == 0x05;  // depending on the value of r
}

bool isREJ(unsigned char ctrl) {
    return ctrl == 0x81 || ctrl == 0x01;  // depending on the value of r
}

int getS(unsigned char ctrl) {
    return ctrl >> 6;
}

int getR(unsigned char ctrl) {
    return ctrl >> 7;
}

bool checkDestuffedBCC(char* buf, char bcc, size_t dataCount){
    #ifdef EFFICIENCY_TEST
    generateDataError(buf,dataCount);
    #endif

    
    char aux = 0x0;
    if (dataCount == 0) return false;

    // Can be done like this because dataCount is the number of data bytes received and that will always be true

    #ifdef DEBUG_STATE_MACHINE
    printf("BCC calculated with: ");
    #endif
    for(size_t i = 4; i < dataCount + 4; i++){
        #ifdef DEBUG_STATE_MACHINE
        printf("%x ", (unsigned char)buf[i]);
        #endif
        aux ^= (unsigned char)buf[i];
    }

    #ifdef DEBUG_STATE_MACHINE
    printf("\nCalculated BCC: %x, real BCC: %x\n", aux, bcc);
    #endif

    if(aux == bcc) return true;
    else return false;
    
}

bool receivedMessageFlag(char * byte, enum destuffingState destuffing) {
    return *byte == MSG_FLAG && destuffing == DESTUFF_OK;
}

void goBackToStart(enum stateMachine * state, enum destuffingState * destuffing) {
    *state = START;
    *destuffing = DESTUFF_OK;
}

void goBackToFLAG_RCV(enum stateMachine * state, enum destuffingState * destuffing) {
    *state = FLAG_RCV;
    *destuffing = DESTUFF_OK;
}

enum checkStateRET checkState(enum stateMachine *state, char * bcc, char * byte, char*buf, char addressField, char controlField) { 
    static unsigned dataCount = 4;
    static bool dataBCC = 0;
    static enum destuffingState destuffing = DESTUFF_OK;

    // Destuffs the message while it's reading it
    switch (destuffing) {
        case DESTUFF_VIEWING:
            destuffing = DESTUFF_OK;
            // break is missing intentionally
        case DESTUFF_OK:
            if (*byte == 0x7D){
                destuffing = DESTUFF_WAITING;
                return IGNORE_CHAR;
            }
            break;
        case DESTUFF_WAITING:
            if (*byte == 0x5E){
                *byte = 0x7E;
            } else if (*byte == 0x5D){
                *byte = 0x7D;
            } else {
                printf("Error while destuffing in checkstate!\n");
                //exit(EXIT_FAILURE);
            }
            destuffing = DESTUFF_VIEWING;
            break;
        default: 
            break;
    }

    #ifdef DEBUG_STATE_MACHINE
    printf("\nSM: byte received: %x; currentState: %s\n", *byte, stateNames[*state]);
    #endif

    switch (*state){
    case START:
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
        } else {
            return IGNORE_CHAR; // Ignores the flag in case it had received one imediately before
        }
        break;
        
    case A_RCV:
        // Only advances when a valid control field is received
        if (!receivedMessageFlag(byte, destuffing) && 
            (*byte == controlField || controlField == ANY_VALUE)) {
            *state = C_RCV;
            bcc[1] = *byte;
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
        #ifdef EFFICIENCY_TEST
            generateHeadError(bcc);
        #endif
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
            if (dataCount >= MAX_DATA_PACKET_SIZE) { *state = DATA_OK;} 
            // if it doesn't receive a flag, then it only looks for the bcc when the max data packet length has been used
            break;
        } else {  // receives a flag; the bcc is the previous byte
            dataCount--; // discounts the BCC from the data count
            
            dataBCC = checkDestuffedBCC(buf, prevByte, dataCount);    
            if (dataBCC){
                *state = DONE_I;
            }
            else {  // BCC is wrong
                goBackToFLAG_RCV(state, &destuffing);
                return DATA_INVALID;
            }
        }
        break;
    case DATA_OK:
        // verify BCC
        if (!receivedMessageFlag(byte, destuffing)) {
            //printf("ERROR. datacount: %d, byte: %x, prevByte: %x\n", dataCount, (unsigned char) *byte, (unsigned char) prevByte);

            dataBCC = checkDestuffedBCC(buf, *byte, dataCount);
           
            if(dataBCC){
                *state = BCC_DATA_OK;   
            }
            else{
                goBackToStart(state, &destuffing);
                return DATA_INVALID;
            }
        } else {   // if it receives a flag when dataOk, it means that the bcc is the previous byte
            dataBCC = checkDestuffedBCC(buf, prevByte, dataCount);    
            dataCount--;  // removes the bcc from the dataCount
            if (dataBCC){
                #ifdef EFFICIENCY_TEST
                    delayGenerator();
                #endif
                *state = DONE_I;
            }
            else {  // BCC is wrong
                goBackToFLAG_RCV(state, &destuffing);
                return DATA_INVALID;
            }
        }
        break;

    case BCC_DATA_OK:
        if (receivedMessageFlag(byte, destuffing)) {  // receives the end flag
            *state = DONE_I;
        } else {
            goBackToStart(state, &destuffing);
            return DATA_INVALID;
        }
        break;
    
    case DONE_I:
    case DONE_S_U:
        destuffing = DESTUFF_OK;
    default:
        break;
    }
    prevByte = *byte;

    return StateOK;
}
