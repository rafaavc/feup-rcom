#include "receiver.h"

extern int fd;

void receiver(int serialPort){    

    if ((fd = llopen(serialPort, RECEIVER_STRING)) == -1) {// Establishes communication with transmitter
        printError("Wasn't able to establish logic connection!\n");
        exit(EXIT_FAILURE); //provavelmente dar nomes signifcativos--LLOPENFAILED
    }

    bool endOfFile = false;
    char buffer[MAX_DATA_LENGTH];
    //char* fileData;

    while(!endOfFile){
        if(llread(fd,buffer) < 0){
            printError("Error in reading the file");
            exit(EXIT_FAILURE);
        }
        /*
        Se nao é chamada a state machine para lidar com isto que quando receber o end_packet coloca a flag endOfFile a true,
        verifica se recebe um start_packet tambem e vai fazendo o parsing das mensagens guardando os dados em fileData
        */
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
    /* After receiving and end control packet, it has received the full file so it's going to disconnect from the transmitter*/
    if (llclose(fd) != 0) {
        printError("Wasn't able to disconnect!\n");
        exit(EXIT_FAILURE);//provavelmente dar nomes signifcativos--LLCLOSEFAILED
    }
}
