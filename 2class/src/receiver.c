#include "protocol.h"

volatile int STOP=FALSE;

int main(int argc, char** argv){
    int fd, sum=0, speed=0;
    struct termios oldtio;

    checkCmdArgs(argc, argv);

    fd = openConfigureSP(argv[1], &oldtio);

    char buf[700], reading;
    int res;
    ssize_t string_size;

    //reads from the serial port
    while(1){
        res = read(fd,&reading,1);

        buf[string_size++]=reading;

        if(reading =='\0') break;
    }

    printf("Read: %s with %ld bytes from the serial port\n", buf, string_size);

    //resends the message back to the emitter by serial port
    res = write(fd,buf,string_size);

    printf("%d bytes written\n", res);


    /*
        O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar
        o indicado no gui�o
    */


    sleep(1);
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }


    close(fd);
    return 0;
}
/*
Nota: falta tratamentos de erro

*/
