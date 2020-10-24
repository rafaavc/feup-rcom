#include "transmitter.h"

extern unsigned stopAndWaitFlag;
extern int fd;

void transmitter(int serialPort){

    fd = llopen(serialPort, TRANSMITTER); // Establish communication with receiver

    if (fd == -1) {
        printError("Wasn't able to establish logic connection!");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLOPENFAILED
    }
    
    debugMessage("Connection established successfully!");


    /*Starts to write all information frames, keeping in mind the need to resend, etc.
    While there is info to write, writes with the stop&wait mechanism as in other situations*/

    char* filename = "fileToTransfer.txt";
    FILE * file = fopen(filename, "r");

  
    struct stat st;
    stat(filename, &st);
    size_t fileSize = st.st_size;

    char buffer[MAX_DATA_LENGTH];
    char ret[CTRL_PACKET_SIZE]; 

    if(feof(file)){
        printError("The file is empty, nothing to send");
        exit(EXIT_FAILURE);
    }
    else{//the file is not empty
        constructControlPacket(ret,START_CTRL,filename, fileSize);
        if(llwrite(fd,ret, CTRL_PACKET_SIZE*sizeof(char)) == -1){  //sending start control packet
            printError("Error sending the start control packet");
            exit(EXIT_FAILURE);
        }
    }

    int msgNr = 0;
    char dataPacket[4 + MAX_DATA_LENGTH]; //se calhar aumentar MAX_DATA_LENGTH ? 

    while (!feof(file)) {
        size_t amount = fread(buffer, sizeof(char), MAX_DATA_LENGTH, file);
        constructDataPacket(dataPacket,buffer,amount,msgNr);
        size_t ret = llwrite(fd, dataPacket, amount + 4); // second arg has a maximum size of MAX_DATA_LENGTH
        if (ret == -1) {
            printError("Error in llwrite\n");
            exit(EXIT_FAILURE);
        }
        if (ferror(file)) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }
        msgNr++;    
    }

    constructControlPacket(ret, END_CTRL,filename, fileSize);

    if(llwrite(fd,ret, CTRL_PACKET_SIZE*sizeof(char)) == -1){ // sending end control packet, sends the same packet as start control packet with the difference in the control
        printError("Error sending the end control packet");
        exit(EXIT_FAILURE);
    }

    //llwrite(fd, "end", 3);

    /*
        When there is no more information to write, it's going to disconnect: sends a DISC frame, receives a DISC frame
        and sends back an UA frame, ending the program execution
    */
    debugMessage("SENDING DISC...");
 
    if (llclose(fd) != 0) {
        printError("Wasn't able to disconnect!");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLCLOSEFAILED
    }
}

/*
! FAZER TRATAMENTOS DE ERROS NAS ESCRITAS E LEITURAS!
*/