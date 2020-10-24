#include <stdlib.h>
#include <stdio.h>
#include "emitter.h"


/* Control Packet*/
#define START_CTRL      0x02
#define END_CTRL        0x03


/* Indexes */
#define APP_CTRL_IDX             0
#define APP_FILE_T_SIZE_IDX      1
#define APP_FILE_L_SIZE_IDX      2
#define APP_FILE_V_SIZE_IDX      3
#define APP_FILE_T_NAME_IDX      4
#define APP_FILE_L_NAME_IDX      5
#define APP_FILE_V_NAME_IDX      6
#define APP_FILE_T_SIZE          0
#define APP_FILE_T_NAME          1
#define APP_SEQUENCE_NUM_IDX     1
#define APP_L2_IDX               2
#define APP_L1_IDX               3
#define APP_DATA_START_IDX       4     

/* Data Packet*/
#define DATA_CTRL       0x01


    

#define APP_SEQ_NUM_SIZE    256

#define PORT_AMOUNT 2
#define TRANSMITTER_STRING "TRANSMITTER"
#define RECEIVER_STRING "RECEIVER"

void printUsage(int * ports);


void checkCmdArgs(int argc, char ** argv);

void constructControlPacket(char * ret,char ctrl, char* fileName, size_t fileSize);

void constructDataPacket(char * ret, char* data, size_t dataSize, int msgNr);

void applicationLoop(int argc, char ** argv);