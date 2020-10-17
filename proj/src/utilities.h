#include <string.h>
#include <stdio.h>
#include <time.h>
#include "defines.h"

void debugMessage(char * msg);

struct myTimer {
    clock_t startTime;
    char * timerName;
};

void printCharArray(char * arr, size_t arrSize);

void initTimer(struct myTimer * timer, char * time);

void startTimer(struct myTimer * timer);

double stopTimer(struct myTimer * timer, unsigned verbose);
