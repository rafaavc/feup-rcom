#include "receiver.h"

extern int fd;

int sequenceNumber = 0;
void recAlarmHandler(int signo) {
    if (signo == SIGALRM) {
        printError("Turning off due to inactivity.\n");
        exit(EXIT_FAILURE);
    }
}

void receiver(int serialPort){  
        
    if (signal(SIGALRM, recAlarmHandler) < 0) {  // Instals the handler for the alarm interruption
            perror("Alarm handler wasn't installed"); 
            exit(EXIT_FAILURE);
    }
    alarm(INACTIVITY_TIME);

    if ((fd = llopen(serialPort, RECEIVER_STRING)) == -1) {// Establishes communication with transmitter
        printError("Wasn't able to establish logic connection!\n");
        exit(EXIT_FAILURE); 
    }

    
    char *buffer = (char*)myMalloc(MAX_DATA_PACKET_LENGTH*sizeof(char));//mudar isto
    char* fileName = NULL;
    size_t fileSize;
    bool receivedStart = false;
    FILE * fileToSave;
    

    unsigned bytesReceived = 0;
    while(true){
        int dataRead = llread(fd,buffer);
        if (dataRead < 0) { //case of error
            printError("Error reading the file.\n");
            exit(EXIT_FAILURE);
        }
        else if (dataRead == 0) continue; //case of REJ sent or repeated data received
        else {
            alarm(INACTIVITY_TIME);
            int dataAmount = -1;
            enum checkReceptionState receptionRet = checkStateReception(buffer, dataRead, &fileSize, &fileName, &dataAmount);
            //printf("Checking the following buffer, with size %d.\n", dataRead);
            //printCharArray(buffer, dataRead);
            if(receptionRet == START_RECEIVED && !receivedStart){
                printf("Starting to receive file '%s' with %ld bytes.\n", fileName, fileSize);
                fileToSave = fopen(fileName, "wb");
                receivedStart = true;
                // check current receiver state (whether it has already received start, if it has, error)
            } else if(receptionRet == DATA_FINISHED && receivedStart){ //it has received the end of the end control packry
                // check if receiver has received start, it it didn't, it's error
                if (dataAmount > 0){
                    fwrite(&buffer[4], sizeof(char), (size_t)dataAmount, fileToSave);
                    bytesReceived += dataAmount;
                } else if (dataAmount == 0) {
                    printError("Amount of data received is 0.\n"); 
                    exit(EXIT_FAILURE);
                } else if (dataAmount < 0) {
                    printError("Amount of data received is negative.\n");
                    exit(EXIT_FAILURE);
                }
                printProgressBar(fileSize, bytesReceived);
            } else if (receptionRet == END_RECEIVED && receivedStart) {
                fclose(fileToSave);
                if (fileSize == bytesReceived) {
                    printf("File received successfully!\n");
                    break;
                } else {    
                    printError("Specified file size (%ld) and received number of bytes (%d) do not match.\n", fileSize, bytesReceived);
                }
            } else if (receptionRet == ERROR){
                printError("\nThere was an error in the reception, received invalid data. Terminating connection.\n");
                exit(EXIT_FAILURE);
            } else {
                printError("Received invalid data package! State: %d\n", receptionRet);
                
                exit(EXIT_FAILURE);
            }
            bzero(buffer, MAX_DATA_PACKET_LENGTH);
        }

    }

    free(buffer);
    free(fileName);

    /* After receiving and end control packet, it has received the full file so it's going to disconnect from the transmitter*/
    if (llclose(fd) != 0) {
        printError("Wasn't able to disconnect!\n");
        exit(EXIT_FAILURE);
    }
}


enum checkReceptionState checkStateReception(char * buffer, int bufferSize, size_t * fileSize, char ** fileName, int * dataAmount){
    int totalAmountOfChars = -1;
    int currentAmountOfChars = -1;
    int idx = 1;
    bool end = false;
    size_t auxFileSize = 0x0;
    char * auxFileName = NULL;
    enum checkReceptionState state = CTRL;

    for (int i = 0; i < bufferSize; i++) {
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
                } 
                else if(byte == END_CTRL){
                    end = true;
                    state = T;
                }
                else {
                    printError("Error in package SM: Received invalid CTRL\n");
                    state = ERROR;
                }
                break;
            case(T):
                #ifdef DEBUG_APP_STATE_MACHINE
                debugMessage("T");
                #endif
                //printf("Received %u in T\n", byte);
                if ((byte == APP_FILE_T_SIZE && idx == 1) || (byte == APP_FILE_T_NAME && idx == 2)){
                    if (!end && idx == 1) *fileSize = 0x0;
                    state = L;
                } else {
                    printError("Error in package SM: Received invalid T\n");
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
                        if (!end)
                            *fileName = (char*) myMalloc(sizeof(char) * (totalAmountOfChars + 1)); // the +1 is because of the \0 end character
                        else
                            auxFileName = (char*) myMalloc(sizeof(char) * (totalAmountOfChars + 1));
                    }
                } else {
                    printError("Error in package SM: Received invalid L\n");
                    state = ERROR;
                }
                break;
            case(V):
                #ifdef DEBUG_APP_STATE_MACHINE
                debugMessage("V");
                #endif
                if(currentAmountOfChars < totalAmountOfChars && idx == 1){//fileSize
                    if(!end){
                        *fileSize = (*fileSize << 8) | (unsigned char)byte;
                    }
                    else{
                        auxFileSize = (auxFileSize << 8) | (unsigned char)byte;
                    }
                    //printf("Value of byte: %x\n", (unsigned char) byte);
                    //printf("Value of fileSize: %lx\n", (unsigned char) *fileSize);
                    currentAmountOfChars++;
                    if(currentAmountOfChars == totalAmountOfChars){
                        //printf("Received file size\n");
                        if(!end){
                            state = T; //going to receive the fileName
                            idx = 2;
                        }
                        else{
                            //printf("Received file size - end. %ld, %ld\n", auxFileSize, *fileSize);
                            if(auxFileSize != *fileSize){
                                printError("Error in package SM: Received invalid file size in end package\n");
                                state = ERROR;
                            }
                            else {
                                //printf("changed to t\n");
                                state = T;
                                idx = 2;
                            }
                        }
                    }
                }
                else if(currentAmountOfChars < totalAmountOfChars && idx == 2){//fileName                 
                    if(!end)
                        (*fileName)[currentAmountOfChars++] = byte;
                    else 
                        auxFileName[currentAmountOfChars++]= byte;
                    /*printf("Current string: ");
                    fwrite(*fileName, sizeof(char), currentAmountOfChars-1, stdout);
                    printf("\n");*/
                    if(currentAmountOfChars == totalAmountOfChars){
                        if(!end){
                            (*fileName)[currentAmountOfChars] = '\0';
                            state = START_RECEIVED;
                        }
                        else{
                            if(strcmp(*fileName, auxFileName) != 0){
                                state = ERROR;
                                printError(" V: End packet with wrong filename\n");
                            }
                            else 
                                state = END_RECEIVED;
                        }
                    }
                }
                else{
                    state = ERROR;
                    printError("V: Not filename or filesize\n");
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
                        printError("DataN: Sequence number not valid 1 \n");
                    }
                    
                }
                else{
                    state = ERROR;
                    printError("Data N: Sequence number not valid 2 \n");
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
                     printError("Data L1: Invalid total amount of chars\n");
                }
                break;
                
            case RECEIVING_DATA:
                #ifdef DEBUG_APP_STATE_MACHINE
                debugMessage("DATA_PACKET_FINISH");
                #endif
                *dataAmount = totalAmountOfChars;
                state = DATA_FINISHED;
                break;
            case DATA_FINISHED: case END_RECEIVED: case START_RECEIVED:
            case ERROR:
                break;
        }
    }
    return state;
}