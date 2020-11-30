#include "fileManager.h"



int receiveFile(int dataSocketFD) {
    unsigned prevFileBufferSize = REPLY_SIZE*sizeof(char);
    char * file = malloc(prevFileBufferSize);
    int n;
    printf("receiving file!\n");
    while((n = read(dataSocketFD, file, REPLY_SIZE)) != 0) {
        printf("hey\n");
        if (n == -1) {
            perror("receiveFile > read()");
            return 1;
        }
        file = realloc(file, prevFileBufferSize + n);
        prevFileBufferSize += n*sizeof(char);
    }

    FILE * output = fopen("testfile", "w");

    fwrite(file, sizeof(char), prevFileBufferSize, output);

    fclose(output);

    close(dataSocketFD);

    return 0;
}



int fileTransfer(int socketFD, char * user, char * password, char * host, char * urlPath){
    /*
        aqui vao ser chamadas todas as funçoes onde se executam os comandos 

    */

    loginHost(socketFD, user, password);

    unsigned int port;
    char * ip = malloc(IP_SIZE*sizeof(char));
    passiveMode(socketFD, ip, &port);
    int dataSocketFD = connectToIP(ip, port);
    retrCommand(socketFD, urlPath);
    receiveFile(dataSocketFD);

    return 0;
}


//se calhar mais a funçao de dowload do ficheiro