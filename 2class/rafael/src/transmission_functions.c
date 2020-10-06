#include <unistd.h>
#include "defines.h"
#include <stdlib.h>
#include <stdio.h>

struct message constructSupervisionMessage(char  addr,char ctrl){
    struct message aux;
    aux.msg = malloc(sizeof(char)*SUPERVISION_MESSAGE_SIZE);
    aux.msg[0] = MSG_FLAG;
    aux.msg[1] = addr;
    aux.msg[2] = ctrl;
    aux.msg[3] ='1' ;//nao sei  é um xor qq 
    aux.msg[4]= MSG_FLAG;

    return aux;
}

