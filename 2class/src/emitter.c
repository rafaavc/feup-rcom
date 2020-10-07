#include "protocol.h"

volatile int STOP=FALSE;

int main(int argc, char** argv){
    int fd, res, sum = 0, speed = 0;
    struct termios oldtio;

    checkCmdArgs(argc, argv);

    fd = openConfigureSP(argv[1], &oldtio);

    //writes to the serial port
    char *buf = NULL;
    size_t n;
    ssize_t string_size;
    string_size = getline(&buf,&n,stdin);

    buf[string_size-1]='\0';

    res = write(fd,buf,string_size*sizeof(buf[0]));
    printf("%d bytes written\n", res);

    /*
      O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar
      o indicado no gui�o
    */


    //reads the message sent back  by serial
    char msg[255];
    char reading;
    n=0;

    while(1){
      string_size = read(fd,&reading,1);
      msg[n++]=reading;
      if(reading=='\0') break;
    }

    printf("Returned message: %s with %ld bytes\n", msg, n);

    sleep(1);
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
}

/*
Notas: pus a correr com /dev/ttyS0
Nao fiz tratamento de erros nas leituras e escritas
*/
