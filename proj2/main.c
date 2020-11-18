#include "connection.h"


void printUsage(char ** argv) {
    fprintf(stderr, "Usage: %s ftp://[<user>:<password>@]<host>/<url-path>", argv[0]);
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
        printUsage(argv);
        exit(EXIT_FAILURE);
    }

    //char* ip = getIP(host);

    free(user);
    free(password);
    //free(*host);
    //free(*urlPath);


    return 0;
}