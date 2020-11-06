#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define DELAY               1
#define PROBABILITY_HEAD    30
#define PROBABILITY_DATA    40

/**
 * Introduces a delay in the frame processing of each frame received, variating Tprop
 */
void delayGenerator();
/**
 * Introduces an error on the data field of the frame with a predefined probability, acting like a real error
 */
void generateDataError(char *frame, size_t frameSize);
/**
 * Introduces an error on the head of the frame with a predefined probability, acting like a real error
 */
void generateHeadError(char *buffer);