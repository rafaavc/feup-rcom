#include "tests.h"

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

void test1(int fd) {
    
}



