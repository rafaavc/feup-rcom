#include "utilities.h"

void debugMessage(char * msg) {
    #ifndef DEBUG
    return;
    #endif
    printf("- %s\n", msg);
}

bool isUnsignedNumber(char * str) {
    for(int i = 0; str[i] != '\0'; i++) {
        if(!(str[i] >= '0' && str[i] <= '9')) {
            return false;
        }
    }
    return true;
}

void printCharArray(char * arr, size_t arrSize) {
    int count = 0;
    for (size_t i = 0; i < arrSize; i++) {
        count++;
        printf("%x ", (unsigned char)arr[i]);
    }
    printf("\ntotal: %d, dados: %d\n", count, count-6);
    printf("\n");
}

void initTimer(struct myTimer * timer, char * name) {
    timer->startTime = -1;
    timer->timerName = name;
}

void startTimer(struct myTimer * timer) {
    timer->startTime = clock();
}

double stopTimer(struct myTimer * timer, unsigned verbose) {
    if (timer->startTime == -1) return -1;

    clock_t endTime = clock();
    clock_t timeItTook = endTime - timer->startTime;

    double timeInSeconds = ((double)timeItTook)/CLOCKS_PER_SEC;

    if (verbose) {
        printf("Timer %s took %lf seconds\n", timer->timerName, timeInSeconds);
    }

    return timeInSeconds;
}


