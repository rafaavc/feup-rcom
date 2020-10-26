#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include "globalDefines.h"

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
 * Checks if a string represents an unsigned number
 * @param str the string to check
 * @return true or false
 */
bool isUnsignedNumber(char * str);
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

