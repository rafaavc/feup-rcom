#include "receiverFuncs.h"
#include "iPA.h"

char role;
static bool usedLLSet = false;

int llopen(int porta, char * r){
    if (strcmp(r, "RECEIVER") == 0) role = RECEIVER;
    else if (strcmp(r, "TRANSMITTER") == 0) role = TRANSMITTER;
    else {
        printError("Invalid role given to llopen\n");
        return -1;
    }
    
    srand(time(NULL));

    if (!usedLLSet) createLinkLayer();

    char portString[12];

    sprintf(portString, "/dev/ttyS%d", porta);
    
    int fd = openConfigureSP(portString); 

    if (fd == -1) return -1;
    
    if(role == RECEIVER){
        if (!receiverConnect()) return -1; //establishing connection with TRANSMITTER
    }
    else if (role == TRANSMITTER){
        if (signal(SIGALRM, alarmHandler) < 0) {  // Installs the handler for the alarm interruption
            perror("Alarm handler wasn't installed"); 
            exit(EXIT_FAILURE);
        }
        siginterrupt(SIGALRM, true);  // SIGALARM interrupts system calls (returns -1 and errno is se to EINTR)
        if (!transmitterConnect()) return -1; //establishing connection with RECEIVER
    } 
    else return -1;

    return fd;
}

int llwrite(int fd, char * buffer, int length){
    char * msg = myMalloc(getMaxFrameBufferSize());
    bzero(msg, getMaxFrameBufferSize());
    size_t s = length;
    
    constructInformationMessage(msg, buffer, &s);

    int res = transmitterWrite(msg,s);
    free(msg);
    if (res < 0) return -1;// in case of error
    
    return res;
}

int llread(int fd, char * buffer){
    return receiverRead(buffer);
} 

int llclose(int fd){
    if(role == TRANSMITTER){
        if(!transmitterDisconnect()) //disconnecting in the TRANSMITTER side 
            return -1;
    }
    else if(role == RECEIVER){ //disconnecting in the RECEIVER side 
       if(receiverDisconnect() == -1)//if there was an error sending/receiving the disconnection messages 
            return -1;;
    }
    else return -1;

    sleep(1);
    
    if(closeSP() != 0){ 
        return -1;
    }

    return 0;
}

void llset(int baudrateArg, int frameSizeArg) {
    usedLLSet = true;
    createLinkLayer();
    if (baudrateArg != -1) {
        speed_t BAUDRATE;
        switch(baudrateArg) {
            case 4800:
                BAUDRATE = B4800;
                break;
            case 9600:
                BAUDRATE = B9600;
                break;
            case 19200:
                BAUDRATE = B19200;
                break;
            case 38400:
                BAUDRATE = B38400;
                break;
            case 57600:
                BAUDRATE = B57600;
                break;
            case 115200:
                BAUDRATE = B115200;
                break;
            case 230400:
                BAUDRATE = B230400;
                break;
            default:
                printError("The specified baudrate isn't valid. Using default. Available:\n4800\n9600\n19200\n38400\n57600\n115200\n230400\n");
                break;
        }
        setBaudrate(BAUDRATE);
    }

    if (frameSizeArg != -1) {
        setFrameSize(frameSizeArg);
    }
}
