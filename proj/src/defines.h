/* Control Packet*/
#define START_CTRL      0x02
#define END_CTRL        0x03

#define printError(args...) fprintf(stderr, ##args)

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

/* Data Packet */
#define DATA_CTRL       0x01


#define MAX_DATA_PACKET_DATA_LENGTH  MAX_DATA_PACKET_LENGTH-4

    
#define CTRL_PACKET_SIZE    7
#define APP_SEQ_NUM_SIZE    256

#define PORT_AMOUNT 2
#define TRANSMITTER_STRING "TRANSMITTER"
#define RECEIVER_STRING "RECEIVER"


enum checkReceptionState{CTRL, T, L, V, RECEIVING_DATA, DATA_N, DATA_L1, DATA_L2, DATA_FINISHED, END_RECEIVED, START_RECEIVED, ERROR};
