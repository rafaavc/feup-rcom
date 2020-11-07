#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include "globalDefines.h"

struct myTimer {
    bool started;
    struct timespec startTime;
    char * timerName;
};

/**
 * Prints a debug message if DEBUG is defined
 * @param msg message string
 */
void debugMessage(char * msg);

/**
 * Allocates memory and terminates program if error happens
 * @param size amount of bytes to allocate
 * @return pointer to the memory location
 */
void * myMalloc(size_t size);

/**
 * Prints an array of chars to stdout
 * @param arr the array
 * @param arrSize the amount of chars in the array
 */
void printCharArray(char * arr, size_t arrSize);

/**
 * Checks if a string represents an unsigned number
 * @param str the string to check
 * @return true or false
 */
bool isUnsignedNumber(char * str);

/**
 * Initializes the timer
 * @param timer reference to myTimer struct
 * @param name name of the alarm 
 */
void initTimer(struct myTimer * timer, char * name);

/**
 * Starts a timer
 * @param timer reference to myTimer struct
 */
void startTimer(struct myTimer * timer);

/**
 * Stops the timer
 * @param timer reference to myTimer struct
 * @param verbose whether it prints a message on stdout
 * @return the time elapsed (in seconds)
 */
double stopTimer(struct myTimer * timer, bool verbose);

/**
 * Adds the given integer ('tries') to the end of a filename (right before the extension)
 * @param fileNameNext the output
 * @param fileName the fileName source
 * @param tries the number to insert
 */
void addIntToFilename(char * fileNameNext, char * fileName, int tries);

