#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include "../protocol/linkLayer.h"
#include "../protocol/defines.h"

#define DELAY               0
#define PROBABILITY_HEAD    0
#define PROBABILITY_DATA    0

/**
 * Introduces a delay in the frame processing of each frame received, variating Tprop
 */
void delayGenerator();
/**
 * Introduces an error on the data field of the frame with a predefined probability, acting like a real error
 * @param frame - current frame
 * @param frameSize - size of the frame
 */
void generateDataError(char *frame, size_t frameSize);
/**
 * Introduces an error on the head of the frame with a predefined probability, acting like a real error
 * @param buffer - buffer that contains the bcc of the header
 */
void generateHeadError(char *buffer);
/**
 * Update the variable needed for the efficiency calculations
 * @param packeteSize - size of the packet in bytes
 * @param time - time taken to send the packet
 */
void rateValuesUpdate(size_t packetSize, double time);
/**
 * Calculates the average rate of the application 
 */
double getAverageRate();


double calculateEfficiency();
