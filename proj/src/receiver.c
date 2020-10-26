#include "receiver.h"

extern int fd;

int length = 0;

int idx = 0;
int counter = 0;
int fileIndex = 0;
int sequenceNumber = 0;
bool end = false;

void receiver(int serialPort){    

    if ((fd = llopen(serialPort, RECEIVER_STRING)) == -1) {// Establishes communication with transmitter
        printError("Wasn't able to establish logic connection!\n");
        exit(EXIT_FAILURE); //provavelmente dar nomes signifcativos--LLOPENFAILED
    }

    bool endOfFile = false;
    char buffer[MAX_DATA_LENGTH];//mudar isto
    char* fileData = NULL;
    char* fileName = NULL;
    size_t fileSize;
    enum checkReceptionState *state = CTRL_START;
   

    while(!endOfFile){
        if(llread(fd,buffer) < 0){
            printError("Error in reading the file");
            exit(EXIT_FAILURE);
        }
        else{
            checkStateReception(state, buffer, &fileSize ,fileName, fileData);
            if(*state == CTRL_START){
                printError("There was an error in transmission");
                break;
            }
            else if(*state == END){ //it has received the end of the end control packry
                printf("We've received the full file\n");
                endOfFile = true; //nothing left to read
            }
            else if (*state == ERROR){
                printError("There was an error in the reception, received invalid data. Terminating connection");
                break;
            }
        }

    }
    
    /*
    Reads the data, until it receives a disconnect, when this happens sends back a disconect and the program hands
    */
   /* char buffer[MAX_DATA_LENGTH];
    while (true) {
        size_t dataLen = llread(fd, buffer);
        if (dataLen == 3 && buffer[0] == 'e' && buffer[1] == 'n' && buffer[2] == 'd') break;
        write(STDOUT_FILENO, buffer, dataLen);
    }
*/
    free(fileData);
    free(fileName);
    /* After receiving and end control packet, it has received the full file so it's going to disconnect from the transmitter*/
    if (llclose(fd) != 0) {
        printError("Wasn't able to disconnect!\n");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLCLOSEFAILED
    }
}


void checkStateReception(enum checkReceptionState *state,char* byte, size_t *fileSize, char*fileName, char*fileData){

    switch(*state){
        case(CTRL_START):
            if(*byte == START_CTRL){
                *state = T;
            }
            break;
        case(T):
            if(*byte == APP_FILE_T_SIZE && idx == 0){
                idx = 1;
                *state = V;
            }
           else if (*byte == APP_FILE_T_NAME && idx == 1){
             idx = 2;
                fileData = (char*) malloc(sizeof(char) * (*fileSize));//we already know the size of the file so we can allocate memory for it's data
                *state = V;
           }
           else{
               *state = ERROR;
           }
            break;
        case(L):
            length = *byte;
            if(idx == 2){
                fileName = (char*) malloc(length * sizeof(char));//we now know the size of the fileName so we can allocate memory for storing it 
            }
            *state = V;
            break;
        case(V):
            if(length > 0 && idx == 1){//fileSize
                fileSize[counter++] = *byte;//provavelmente isto nao funciona IDK
                length--;
                if(length == 0){
                    *state = T; //going to receive the fileName
                    counter = 0; 
                }
            }
            else if(length > 0 && idx == 2){//fileName
                fileName[counter++] = *byte;
                length--;
                if(length == 0){
                    if(end == true){
                        *state = END;
                    }
                    else{
                        *state = RECEIVING_DATA_PACKETS; //end of the begin control packet, we're going to start to receive data packets
                        counter = 0;
                    }
                }
            }
            else{
                *state = ERROR;
            }
            break;
        case(RECEIVING_DATA_PACKETS):
           /* if(*byte == END_CTRL && sequenceNumber > 0){//to make sure we receive at least one data packet, this means we're receiving the end control packet
                *state = T;
                end = true;
             idx = 0;
            }*/
            if(*byte == DATA_CTRL){
                *state = CTRL_DATA;
            }
            else 
                *state = ERROR;

            break;
        case CTRL_DATA:
            if(*byte >= 0 && *byte <= 255){
                if(*byte == sequenceNumber){
                    sequenceNumber++;
                    *state = SEQUENCE_NUMBER;
                }
                else{
                    *state = ERROR;//sequence number not valid
                }
                
            }
            else{
                *state = ERROR;
            }
            break;
        case SEQUENCE_NUMBER:
            counter = *byte;//number of bytes to read to complete the data 
            *state = L_DATA;
            break;
        case L_DATA:
            if(counter > 0){ // we store the data
                fileData[fileIndex++] = *byte;
                counter --;
            }
            else if (counter == 0){// no more data to store, we expect to receive the end control packet
                *state = DATA_PACKET_FINISH;
            }
            else{
                *state = ERROR;
            }
            break;
            
        case DATA_PACKET_FINISH:
            if(*byte == DATA_CTRL){ //means we're going to receive another data packet 
                *state = CTRL_DATA;
            }
            else if(*byte == END_CTRL){ //means we're starting to receive the end control packet
                *state = T;
                end = true; 
                idx = 0;
            }
            else{
                *state = ERROR;
            }
            break;
        case ERROR:
        case END://so por causa do warning, nao é necessaria
            break;
    }


}