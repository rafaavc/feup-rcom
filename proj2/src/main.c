#include "fileManager.h"


void printUsage(char ** argv) {
    fprintf(stderr, "Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
}

        
int main(int argc, char*argv[]){
    if (argc != 2) {
        printUsage(argv);
        exit(EXIT_FAILURE);
    }

    char *user = NULL;
    char *password = NULL; 
    char *host = NULL; 
    char *urlPath = NULL;
    
    if (parseURL(argv[1], &user, &password, &host, &urlPath)){
        fprintf(stderr, "Error parsing the url!\n");
        printUsage(argv);
        exit(EXIT_FAILURE);
    }

    char* ip = getIP(host);
    int commandSocketFD = connectToIP(ip, FTP_COMMAND_PORT);

    char * reply = NULL;
    readReply(commandSocketFD, &reply);

    fileTransfer(commandSocketFD, user, password, host, urlPath);

    //enviar o ficheiro

    free(user);
    free(password);
    free(host);
    free(urlPath);


    return 0;
}