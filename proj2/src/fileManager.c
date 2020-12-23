#include "fileManager.h"

int receiveFile(int dataSocketFD, char * filename) {
    unsigned prevFileBufferSize = REPLY_SIZE*sizeof(char);
    char * file = malloc(prevFileBufferSize);
    int n;
    
    while((n = read(dataSocketFD, file + prevFileBufferSize - (REPLY_SIZE*sizeof(char)), REPLY_SIZE)) != 0) {
        if (n == -1) {
            perror("receiveFile > read()");
            return 1;
        }
        file = realloc(file, prevFileBufferSize + n);
        prevFileBufferSize += n*sizeof(char);
    }

    FILE * output = fopen(filename, "w");

    fwrite(file, sizeof(char), prevFileBufferSize-REPLY_SIZE, output);

    fclose(output);

    close(dataSocketFD);

    return 0;
}



int fileTransfer(int socketFD, char * user, char * password, char * host, char * urlPath, char * filename){

    if (loginHost(socketFD, user, password)) return EXIT_FAILURE;

    unsigned int port;
    char * ip = malloc(IP_SIZE*sizeof(char));
    if (passiveMode(socketFD, ip, &port)) return EXIT_FAILURE;

    int dataSocketFD = connectToIP(ip, port);
    if (binaryMode(socketFD, ip, &port)) return EXIT_FAILURE;
    if (retrCommand(socketFD, urlPath)) return EXIT_FAILURE;


    if (receiveFile(dataSocketFD, filename)) return EXIT_FAILURE;

    if (disconnect(socketFD)) return EXIT_FAILURE;

    return 0;
}

