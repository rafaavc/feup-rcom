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
    int hasUser = user == NULL ? 0 : 1;

    //user command
    unsigned userCommandSize = hasUser ? strlen(user)+5 : 12;
    char *userCommand =  malloc(sizeof(char)*userCommandSize);
    userCommand[0] = '\0';
    strcat(userCommand, "user ");

    if(hasUser) strcat(userCommand, user);
    else strcat(userCommand, "anonymous");


    unsigned passwordCommandSize = hasUser ? strlen(password)+5 : 11;
    char * passwordCommand = malloc(sizeof(char)*passwordCommandSize);
    passwordCommand[0] = '\0';
    strcat(passwordCommand, "pass ");

    if (hasUser) strcat(passwordCommand, password);
    else strcat(passwordCommand, "random");


    //send user command 
    if(sendCommand(socketFD, userCommand) != 0){
        fprintf(stderr,"Error sending user command: %s\n", userCommand);
        free(userCommand);      
        return EXIT_FAILURE;
    }

    printf("User command: %s\n", userCommand);
    free(userCommand);

    //read reply 
    unsigned replyCode = 0;
    readReply(socketFD, &replyCode, NULL);
    if(replyCode != USER_OK){
        fprintf(stderr,"Error reading user command reply\n");
        return EXIT_FAILURE;
    }
     
    if(sendCommand(socketFD, passwordCommand) != 0){ //send password command
        fprintf(stderr,"Error sending password command: %s\n", passwordCommand);
        free(passwordCommand);  
        return EXIT_FAILURE;
    }

    printf("Password command: %s \n", passwordCommand);
    free(passwordCommand);

    //read command answer
    replyCode = 0;
    readReply(socketFD, &replyCode, NULL);

    if (replyCode != SUCESSFULL_LOGIN){ 
        fprintf(stderr,"Error reading password command reply\n");   
        return EXIT_FAILURE;
    }

    return 0;   
}



int passiveMode(int socketFD, char *ip, unsigned int *porta){
    char *pasvCommand = malloc(sizeof(char)* (strlen("pasv")+2));
    pasvCommand[0] = '\0';
    sprintf(pasvCommand, "pasv \n");
    

    //send pasv command 
    if(sendCommand(socketFD,pasvCommand) != 0){
        fprintf(stderr,"Error sending pasv command\n");
        free(pasvCommand);       
        return EXIT_FAILURE;
    }

    unsigned replyCode = 0;
    char * reply = malloc(REPLY_SIZE);
    //read response < 227 Entering Passive Mode (193,136,28,12,19,91)
    readReply(socketFD, &replyCode, reply);

    if(replyCode != PASSIVE_MODE){
        fprintf(stderr,"Error reading pasv command reply\n");   
        return EXIT_FAILURE;
    }

    if(parsePsv(reply, ip, porta) != 0){
        return EXIT_FAILURE;
    }

    return 0;
}

int binaryMode(int socketFD, char *ip, unsigned int *porta){
    char *bynaryCommand = malloc(sizeof(char)* (strlen("pasv")+2));
    bynaryCommand[0] = '\0';
    sprintf(bynaryCommand, "type I\n");
    

    //send pasv command 
    if(sendCommand(socketFD,bynaryCommand) != 0){
        fprintf(stderr,"Error sending binary mode command\n");
        free(bynaryCommand);       
        return EXIT_FAILURE;
    }
    printf("Binary command: %s \n", bynaryCommand);
    free(bynaryCommand);


    unsigned replyCode = 0;
    char * reply = malloc(REPLY_SIZE);
    readReply(socketFD, &replyCode, reply);

    if(replyCode != BINARY_MODE){
        fprintf(stderr,"Error reading binary command reply\n");   
        return EXIT_FAILURE;
    }



    return 0;
}

int retrCommand(int socketFD, char*urlPath){
    char *retrCommand = malloc(sizeof(char)*(5+strlen(urlPath)));
    retrCommand[0] = '\0';
    strcat(retrCommand, "retr ");
    strcat(retrCommand,urlPath);

    if(sendCommand(socketFD, retrCommand) != 0){
        fprintf(stderr,"Error sending retr command: %s\n", retrCommand);
        free(retrCommand);      
        return EXIT_FAILURE;
    }

    printf("RetrCommand: %s\n", retrCommand);
    free(retrCommand);

    // Reads the 150 response
    unsigned replyCode = 0;
    if(readReply(socketFD, &replyCode, NULL) != 0){
        fprintf(stderr,"Error reading retr command reply\n");  
        return EXIT_FAILURE;
    }

    return 0;
}




int disconnect(int socketFD) {
    char *quitCommand = malloc(sizeof(char)* (strlen("quit")+2));
    quitCommand[0] = '\0';
    sprintf(quitCommand, "quit \n");

    // reads retr success
    unsigned replyCode = 0;
    if(readReply(socketFD, &replyCode, NULL) != 0){
        fprintf(stderr,"Error reading retr command reply\n");  
        return EXIT_FAILURE;
    }

    if(sendCommand(socketFD, quitCommand) != 0){
        fprintf(stderr,"Error sending quit command: %s\n", quitCommand);
        free(quitCommand);      
        return EXIT_FAILURE;
    }
    printf("QuitCommand: %s\n", quitCommand);
    free(quitCommand);

    replyCode = 0;
    if(readReply(socketFD, &replyCode, NULL) != 0){
        fprintf(stderr,"Error reading quit command reply\n");  
        return EXIT_FAILURE;
    }

    if(replyCode != QUIT_SUCESS){
        printf("%d\n", replyCode);
        fprintf(stderr,"Error reading quit command reply\n");   
        return EXIT_FAILURE;
    }

   return 0;
}