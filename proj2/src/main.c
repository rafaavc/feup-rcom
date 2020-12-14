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
    char *filename = NULL;
    if (parseURL(argv[1], &user, &password, &host, &urlPath, &filename)){
        fprintf(stderr, "Error parsing the url!\n");
        printUsage(argv);
        exit(EXIT_FAILURE);
    }

    if (user != NULL) {
        printf("Retrieving '%s' from '%s', using username '%s' and password '%s'.\n", urlPath, host, user, password);
    } else {
        printf("Retrieving '%s' from '%s' anonymously.\n", urlPath, host);
    }

    char* ip = getIP(host);
    int commandSocketFD = connectToIP(ip, FTP_COMMAND_PORT);

    unsigned reply = 0;
    readReply(commandSocketFD, &reply, NULL);

    fileTransfer(commandSocketFD, user, password, host, urlPath, filename); //download of the file 

    free(user);
    free(password);
    free(host);
    free(urlPath);


    return 0;
}