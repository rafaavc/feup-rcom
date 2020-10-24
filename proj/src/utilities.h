#include <string.h>
#include <stdio.h>
#include <time.h>
#include "defines.h"

struct myTimer {
    clock_t startTime;
    char * timerName;
};

/**
 * 
 * @param msg
 */
void debugMessage(char * msg);
/**
 * 
 * @param arr
 * @param arrSize
 */
void printCharArray(char * arr, size_t arrSize);
/**
 * 
 * @param timer
 * @param time
 */
void initTimer(struct myTimer * timer, char * time);
/**
 * 
 * @param timer
 */
void startTimer(struct myTimer * timer);
/**
 * 
 * @param timer
 * @param verbose
 * @return 
 */
double stopTimer(struct myTimer * timer, unsigned verbose);

