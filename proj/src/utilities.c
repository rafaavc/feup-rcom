#include "utilities.h"

void debugMessage(char * msg) {
    #ifndef DEBUG
    return;
    #endif
    printf("- %s\n", msg);
}

void printCharArray(char * arr, size_t arrSize) {
    for (size_t i = 0; i < arrSize; i++) {
        printf("%x ", arr[i]);
    }
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




