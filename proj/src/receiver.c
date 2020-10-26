#include "receiver.h"
#include <unistd.h>

extern int fd;

int sequenceNumber = 0;

void receiver(int serialPort){    

    if ((fd = llopen(serialPort, RECEIVER_STRING)) == -1) {// Establishes communication with transmitter
        printError("Wasn't able to establish logic connection!\n");
        exit(EXIT_FAILURE); //provavelmente dar nomes signifcativos--LLOPENFAILED
    }


    char buffer[MAX_DATA_PACKET_LENGTH];//mudar isto
    char* fileName = NULL;
    size_t fileSize;
   

    while(true){
        int dataRead = llread(fd,buffer);
        if( dataRead< 0){
            printError("Error reading the file\n");
            exit(EXIT_FAILURE);
        }
        else{
            int dataAmount = -1;
            enum checkReceptionState state = checkStateReception(buffer, dataRead, &fileSize , &fileName, &dataAmount);
            //printf("Checking the following buffer, with size %d.\n", dataRead);
            //printCharArray(buffer, dataRead);
            if (dataAmount > 0){
                fwrite(&buffer[4], sizeof(char), (size_t)dataAmount, stdout);
            }
            if(state == CTRL){
                printError("There was an error in transmission");
                break;
            }
            else if(state == V || state == L){ //it has received the end of the end control packry
                //printf("We've received the full file\n");
                printf("We've received fileSize = %ld and fileName = %s\n", fileSize, fileName);
            }
            else if (state == ERROR){
                printError("There was an error in the reception, received invalid data. Terminating connection");
                break;
            } else if (state != RECEIVING_DATA) {
                printf("EOOORRROORRO\n");
            }
            bzero(buffer, MAX_DATA_PACKET_LENGTH);
        }

    }
    
    free(fileName);
    /* After receiving and end control packet, it has received the full file so it's going to disconnect from the transmitter*/
    if (llclose(fd) != 0) {
        printError("Wasn't able to disconnect!\n");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLCLOSEFAILED
    }
}


enum checkReceptionState checkStateReception(char * buffer, int bufferSize, size_t * fileSize, char ** fileName, int * dataAmount){
    int totalAmountOfChars = -1;
    int currentAmountOfChars = -1;
    int idx = 1;
    *fileSize = 0x0;
    enum checkReceptionState state = CTRL;
    for (int i = 0; i < bufferSize; i++) {
        char parameterGetter = -1;
        unsigned char byte = buffer[i];    
        switch(state){
            case(CTRL):
                #ifdef DEBUG_APP_STATE_MACHINE
                debugMessage("CTRL");
                #endif
                if(byte == START_CTRL){
                    state = T;
                }
                else if (byte == DATA_CTRL) {
                    state = DATA_N;
                } else {
                    state = ERROR;
                }
                break;
            case(T):
                #ifdef DEBUG_APP_STATE_MACHINE
                debugMessage("T");
                #endif
                //printf("Received %u in T\n", byte);
                if ((byte == APP_FILE_T_SIZE && idx == 1) || (byte == APP_FILE_T_NAME && idx == 2)){
                    state = L;
                } else {
                    state = ERROR;
                }
                break;
            case(L):
                #ifdef DEBUG_APP_STATE_MACHINE
                debugMessage("L");
                #endif
                if ((idx == 1) || (idx == 2)){
                    totalAmountOfChars = byte;
                    currentAmountOfChars = 0;
                    state = V;
                    //printf("Receiving string with size %d\n", totalAmountOfChars);
                    if (idx == 2) {//we already know the size of the file so we can allocate memory for it's data
                        *fileName = (char*) malloc(sizeof(char) * (totalAmountOfChars + 1)); // the +1 is because of the \0 end character
                    }
                } else {
                    state = ERROR;
                }
                break;
            case(V):
                #ifdef DEBUG_APP_STATE_MACHINE
                debugMessage("V");
                #endif
                if(currentAmountOfChars < totalAmountOfChars && idx == 1){//fileSize
                    *fileSize = (*fileSize << 8) | (unsigned char)byte;
                    //printf("Value of byte: %x\n", (unsigned char) byte);
                    //printf("Value of fileSize: %lx\n", (unsigned char) *fileSize);

                    currentAmountOfChars++;

                    if(currentAmountOfChars == totalAmountOfChars){
                        state = T; //going to receive the fileName
                        idx = 2;
                    }
                }
                else if(currentAmountOfChars < totalAmountOfChars && idx == 2){//fileName
                    (*fileName)[currentAmountOfChars++] = byte;
                    /*printf("Current string: ");
                    fwrite(*fileName, sizeof(char), currentAmountOfChars-1, stdout);
                    printf("\n");*/
                    if(currentAmountOfChars == totalAmountOfChars){
                        (*fileName)[currentAmountOfChars] = '\0';
                        return V;
                    }
                }
                else{
                    state = ERROR;
                }
                break;
            case DATA_N:
                #ifdef DEBUG_APP_STATE_MACHINE
                debugMessage("CTRL_DATA");
                #endif
                if((unsigned)byte >= 0 && (unsigned)byte < 255){
                    if((unsigned)byte == sequenceNumber){
                        sequenceNumber = (sequenceNumber+1) % 255;
                        state = DATA_L2;
                    }
                    else{
                        state = ERROR;//sequence number not valid
                    }
                    
                }
                else{
                    state = ERROR;
                }
                break;
            case DATA_L2:
                #ifdef DEBUG_APP_STATE_MACHINE
                debugMessage("SEQUENCE_NUMBER");
                #endif
                totalAmountOfChars = (unsigned)((byte << 8) & (unsigned)0xFF00);//number of bytes to read to complete the data 
                state = DATA_L1;
                break;
            case DATA_L1:
                #ifdef DEBUG_APP_STATE_MACHINE
                debugMessage("L_DATA");
                #endif
                totalAmountOfChars |= (byte & 0x000FF);
                currentAmountOfChars = 0;
                //printf("Receiving %x bytes of data\n", totalAmountOfChars);
                if(totalAmountOfChars >= 0){ // we store the data
                    state = RECEIVING_DATA;
                }
                else{
                    state = ERROR;
                }
                break;
                
            case RECEIVING_DATA:
                #ifdef DEBUG_APP_STATE_MACHINE
                debugMessage("DATA_PACKET_FINISH");
                #endif
                *dataAmount = totalAmountOfChars;
                return RECEIVING_DATA;
                break;
            case ERROR:
                return ERROR;
        }
    }
    return ERROR;
}