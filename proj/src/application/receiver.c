#include "receiver.h"

extern int fd;


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

    
    char *buffer = (char*) myMalloc(MAX_DATA_PACKET_LENGTH*sizeof(char));//mudar isto
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
                FILE * aux;
                unsigned tries = 1;
                char fileNameAux[260];
            
                strcpy(fileNameAux, fileName);
                
                while ((aux = fopen(fileNameAux, "rb")) != NULL){
                    fclose(aux);
                    if (tries > 3) {
                        printError("Conflicts with destination filename! Tried to find an unused name 3 times.\n");
                        exit(EXIT_FAILURE);
                    }
                    printError("Received filename '%s' exists, trying with '%s'.\n", fileName, fileNameAux);
                    sprintf(fileNameAux, "%s%d", fileName,tries);
                    printf("%s\n", fileNameAux);
                    tries++;
                    }
                
                fileToSave = fopen(fileNameAux, "wb");
                if (ferror(fileToSave)) {
                    perror("Error opening file");
                    exit(EXIT_FAILURE);
                }

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

