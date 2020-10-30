#include "tests.h"

void test1(int fd) {
    llwrite(fd, "testing!!", 9);
    //llwrite(fd, "1234", 4);
}

void test(int fd, int testNo) {
    switch(testNo) {
        case 1:
            test1(fd);
            break;
        default:
            printError("You didn't specify a valid test number!\n");
            break;
    }
}


