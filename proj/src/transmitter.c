#include "transmitter.h"

extern unsigned stopAndWaitFlag;
extern int fd;


int constructControlPacket(char * ret, char ctrl, char* fileName, size_t fileSize){
    size_t fileNameSize = strlen(fileName)+1;
    if(fileNameSize > 255){
        printError("Size name can not be larger than 255 characters!");
        exit(EXIT_FAILURE);
    }
    ret[APP_CTRL_IDX] = ctrl;
    ret[APP_FILE_T_SIZE_IDX] = APP_FILE_T_SIZE;
    int fileSizeLength = sizeof(size_t);
    ret[APP_FILE_L_SIZE_IDX] = (unsigned char)fileSizeLength;
    for (int i = fileSizeLength-1; i >= 0; i--) {
        ret[APP_FILE_V_SIZE_IDX + ((fileSizeLength-1) - i)] = (unsigned char)((fileSize >> i*8) & 0x0FF);
        //printf("File size: %lx; File size current: %x\n", fileSize, ((unsigned char)fileSize) >> i);
    }
    unsigned nameTPosition = APP_FILE_T_NAME_IDX + fileSizeLength-1;
    ret[nameTPosition] = APP_FILE_T_NAME;

    unsigned nameLPosition = APP_FILE_L_NAME_IDX + fileSizeLength-1;
    ret[nameLPosition] = (unsigned char)strlen(fileName);
    for (int i = 0; i < strlen(fileName); i++) {
        ret[nameLPosition + 1 + i] = (unsigned char)fileName[i];
    }

    return CTRL_PACKET_SIZE + fileSizeLength-1 + strlen(fileName);
}

//K = 256*L2 +L1 = 2⁸ * L2 + L1 = L2<<8 +L1, L2 is the most significant byte of dataSize and L1 the least signifcant byte
void constructDataPacket(char * ret, char* data, size_t dataSize, int msgNr){
    ret[APP_CTRL_IDX] = DATA_CTRL;
    ret[APP_SEQUENCE_NUM_IDX] = msgNr % 255;
    ret[APP_L2_IDX] = (dataSize & 0x0FF00) >> 8;
    ret[APP_L1_IDX] = dataSize & 0x00FF;

    memcpy(ret + APP_DATA_START_IDX, data, dataSize);//inserts the data in the DataPacket at its right index

}   

void transmitter(int serialPort, char * fileToSend, char * destFile){

    fd = llopen(serialPort, TRANSMITTER_STRING); // Establish communication with receiver

    if (fd == -1) {
        printError("Wasn't able to establish logic connection!");
        exit(EXIT_FAILURE);
    }


    /*Starts to write all information frames, keeping in mind the need to resend, etc.
    While there is info to write, writes with the stop&wait mechanism as in other situations*/

    char* filename = fileToSend != NULL ? fileToSend : "awesome.jpg";
    char* destFilename = destFile != NULL ? destFile : "receivedFile.jpg";
    FILE * file = fopen(filename, "rb");

    if(file == NULL){
        char * str = (char*)malloc(sizeof(char)*512);
        sprintf(str,"\nFile %s does not exist \n", filename);
        printError(str);
        free(str);
        exit(EXIT_FAILURE);
    }
  
    struct stat st;
    stat(filename, &st);
    size_t fileSize = st.st_size;

    char *buffer = (char*)myMalloc(MAX_DATA_PACKET_LENGTH*sizeof(char));
    char *ret = (char*)myMalloc(MAX_DATA_PACKET_LENGTH*sizeof(char)); 

    if(feof(file)){
        printError("The file is empty, nothing to send");
        exit(EXIT_FAILURE);
    }
    else{//the file is not empty
        int packetSize = constructControlPacket(ret, START_CTRL, destFilename, fileSize);
        //printCharArray(ret, packetSize);
        if(llwrite(fd,ret, packetSize*sizeof(char)) == -1){  //sending start control packet
            printError("Error sending the start control packet");
            exit(EXIT_FAILURE);
        }
    }
    
    printf("Starting to send file '%s' (destination: '%s') with %ld bytes.\n", filename, destFilename, fileSize);

    int msgNr = 0;
    char *dataPacket = (char*) myMalloc(MAX_DATA_PACKET_LENGTH*sizeof(char));
    size_t amountTransfered = 0;

    while (!feof(file)) {
        size_t amount = fread(buffer, sizeof(char), MAX_DATA_PACKET_DATA_LENGTH, file);
        constructDataPacket(dataPacket, buffer, amount, msgNr);
        //printCharArray(dataPacket, amount+4);
        size_t ret = llwrite(fd, dataPacket, amount+4); // second arg has a maximum size of MAX_DATA_PACKET_LENGTH
        if (ret == -1) {
            printError("Error in llwrite\n");
            exit(EXIT_FAILURE);
        }
        if (ferror(file)) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }
        msgNr++;
        amountTransfered += amount;
        printProgressBar(fileSize, amountTransfered);   
    }

    if (amountTransfered == fileSize) {
        printf("File transfered successfully!\n");
    } else {
        printError("Was supposed to transfer %ld bytes, only transfered %ld\n", fileSize, amountTransfered);
    }

    fclose(file);

    int endSize = constructControlPacket(ret, END_CTRL,destFilename, fileSize);

    if(llwrite(fd,ret, endSize*sizeof(char)) == -1){ // sending end control packet, sends the same packet as start control packet with the difference in the control
        printError("Error sending the end control packet");
        exit(EXIT_FAILURE);
    }

    free(ret);
    free(buffer);
    free(dataPacket);
    /*
        When there is no more information to write, it's going to disconnect: sends a DISC frame, receives a DISC frame
        and sends back an UA frame, ending the program execution
    */
    debugMessage("SENDING DISC...");
 
    if (llclose(fd) != 0) {
        printError("Wasn't able to disconnect!");
        exit(EXIT_FAILURE);
    }
}
