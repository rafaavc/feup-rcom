#include "applicationFunctions.h"

void printUsage(int * ports) {
    printf("Usage:\n  ./app <serial port> <actor>\nWhere <serial port> is the final number of one of:\n");
    for (unsigned i = 0; i < PORT_AMOUNT; i++) {
        printf("/dev/ttyS%d\n", ports[i]);
    }
    printf("And <actor> is either RECEIVER or TRANSMITTER.\n");
    exit(EXIT_FAILURE);
}

void checkCmdArgs(int argc, char ** argv) {
    int ports[PORT_AMOUNT] = {
        #ifndef SOCAT 
        0,
        1
        #else
        10,
        11
        #endif
    };

    if (argc < 3) {
        printUsage(ports);
    } else {
        unsigned isValidSP = 0;
        for (unsigned i = 0; i < PORT_AMOUNT; i++) {
            if (atoi(argv[1]) == ports[i]){
                isValidSP = 1;
                break;
            }
                
        }
        if ((strcmp(argv[2], TRANSMITTER_STRING) != 0 && strcmp(argv[2], RECEIVER_STRING) != 0) || !isValidSP) {
            printUsage(ports);
        }
    }
}

void constructControlPacket(char * ret, char ctrl, char* fileName, size_t fileSize){
    size_t fileNameSize = strlen(fileName)+1;
    if(fileNameSize > 255){
        printError("Size name can not be larger than 255 characters!");
        exit(EXIT_FAILURE);
    }
    ret[APP_CTRL_IDX] = ctrl;
    ret[APP_FILE_T_SIZE_IDX] = APP_FILE_T_SIZE;
    ret[APP_FILE_L_SIZE_IDX] = sizeof(size_t);
    ret[APP_FILE_V_SIZE_IDX] = fileSize;
    ret[APP_FILE_T_NAME_IDX] = APP_FILE_T_NAME;
    ret[APP_FILE_L_NAME_IDX] = strlen(fileName) + 1;
    ret[APP_FILE_V_NAME_IDX ] = *fileName;

    return;
}

//K = 256*L2 +L1 = 2⁸ * L2 + L1 = L2<<8 +L1, L2 is the most significant byte of dataSize and L1 the least signifcant byte
void constructDataPacket(char * ret, char* data, size_t dataSize, int msgNr){
    ret[APP_CTRL_IDX] = DATA_CTRL;
    ret[APP_SEQUENCE_NUM_IDX] = msgNr % 255;
    ret[APP_L2_IDX] = dataSize & 0x00FF;
    ret[APP_L1_IDX] = (dataSize & 0xFF00) >> 8;

    memcpy(ret + APP_DATA_START_IDX, data, dataSize);//inserts the data in the DataPacket at it's right index

    return;
}   