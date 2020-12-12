#include "connection.h"


char *getIP(char* hostname){
    struct hostent *h; 

    if ((h = gethostbyname(hostname)) == NULL) {  
        herror("Error getting host by name");
        return NULL;
    }

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

    return inet_ntoa(*((struct in_addr *)h->h_addr));
}


int connectToIP(char * ip, uint16_t port) {
    struct sockaddr_in server_addr;
	
	/*server address handling*/
	bzero((char*)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */
    
    int socketFD; 
	/*open an TCP socket*/
	if ((socketFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("connectToIP > socket()");
        exit(EXIT_FAILURE);
    }
	/*connect to the server*/
    if(connect(socketFD, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("connectToIP > connect()");
        exit(EXIT_FAILURE);
	}

    return socketFD;
}


int loginHost(int socketFD, char *user, char *password){
    //check if there is a user
    int hasUser = 1;    
    if(strncmp(user, "",1) == 0){ // no authentication
        hasUser = 0;
    }

    //user command
    char *userCommand = malloc(sizeof(char)* (strlen(user)+5));
    userCommand[0] = '\0';
    strcat(userCommand, "user ");
    if(hasUser) strcat(userCommand,user);
    else strcat(userCommand, "anonymous");

    char *passwordCommand = NULL;
    if(hasUser){
        //password command
        passwordCommand = malloc(sizeof(char)*(strlen(password)+5));
        passwordCommand[0] = '\0';
        strcat(passwordCommand, "pass ");
        strcat(passwordCommand, password);
        printf("%s\n", passwordCommand);
    }

    //reply
    char *reply = NULL;

    //send user command 
    if(sendCommand(socketFD, userCommand) != 0){
        fprintf(stderr,"Error sending user command: %s\n", userCommand);
        free(userCommand);      
        return EXIT_FAILURE;
    }

    printf("User command: %s\n", userCommand);
    free(userCommand);

    //read command answer 
    if(readReply(socketFD, &reply) != 0){
        fprintf(stderr,"Error reading user command reply\n");
        free(reply);
        return EXIT_FAILURE;
    }

    if(!hasUser){
        free(userCommand);
        free(reply);
        return 0;
    }
    //send password command 
    if(sendCommand(socketFD, passwordCommand) != 0){
        fprintf(stderr,"Error sending password command: %s\n", passwordCommand);
        free(passwordCommand);  
        free(reply);      
        return EXIT_FAILURE;
    }

    printf("Password command: %s \n", passwordCommand);
    free(passwordCommand);

    //read command answer
    char*reply2 = NULL;
    if(readReply(socketFD, &reply2) != 0){
        fprintf(stderr,"Error reading password command reply\n");   
        free(reply2);
        return EXIT_FAILURE;
    }

    free(reply2);
    free(reply);
    return 0;
}



int passiveMode(int socketFD, char *ip, unsigned int *porta){
    char *pasvCommand = malloc(sizeof(char)* (strlen("pasv")+2));
    pasvCommand[0] = '\0';
    sprintf(pasvCommand, "pasv \n");
    

    //send pasv command 
    if(sendCommand(socketFD,pasvCommand) != 0){
        fprintf(stderr,"Error sendi  ng pasv command\n");
        free(pasvCommand);       
        return EXIT_FAILURE;
    }

    char *reply = NULL;

    //read response < 227 Entering Passive Mode (193,136,28,12,19,91)
    if(readReply(socketFD, &reply) != 0){
        fprintf(stderr,"Error reading pasv command reply\n");   
        free(reply);
        return EXIT_FAILURE;
    }

    if(parsePsv(reply, ip, porta) != 0){
        return EXIT_FAILURE;
    }

    return 0;
}


int retrCommand(int socketFD, char*urlPath){
    char *retrCommand = malloc(sizeof(char)*(5+strlen(urlPath)));
    retrCommand[0] = '\0';
    strcat(retrCommand, "retr ");
    strcat(retrCommand,urlPath);

    printf("RetrCommand: %s\n", retrCommand);

    if(sendCommand(socketFD, retrCommand) != 0){
        fprintf(stderr,"Error sending retr command: %s\n", retrCommand);
        free(retrCommand);      
        return EXIT_FAILURE;
    }

    free(retrCommand);

    char *reply = NULL;
    if(readReply(socketFD, &reply) != 0){
        fprintf(stderr,"Error reading retr command reply\n");   
        free(reply);
        return EXIT_FAILURE;
    }

    free(reply);
    return 0;
}