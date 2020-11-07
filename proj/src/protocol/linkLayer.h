#ifndef LINK_LAYER_H
#define LINK_LAYER_H

#include <termios.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * @return baudrate
 */
speed_t getBaudrate();

/**
 * @return frame size
 */
unsigned getFrameSize();

/**
 * @return max data packet size
 */
unsigned getMaxDataPacketSize();

/**
 * @return max frame buffer size
 */
unsigned getMaxFrameBufferSize();

/**
 * @return timeout
 */
unsigned getTimeout();

/**
 * @return num of transmissions
 */
unsigned getNumTransmissions();

/**
 * @return oldtio
 */
struct termios * getOldTIO();

/**
 * @return next sequence number
 */
unsigned getNextSequenceNumber();

/**
 * @return serial port fd
 */
int getFD();



/**
 * @param next nest sequence number
 */
void incrementNextSequenceNumber();

/**
 * @param fd serial port fd
 */
void setFD(int fd);

/**
 * @param timeout the timeout to set
 */
void setTimeout(unsigned timeout);

/**
 * @param numTransmissions the number of transmissions to set
 */
void setNumTransmissions(unsigned numTransmissions);

/**
 * @param baudrate the baudrate to set
 */
void setBaudrate(speed_t baudrate);

/**
 * @param baudrate the frame size to set
 */
void setFrameSize(unsigned frameSize);

/**
 * Creates a linkLayer with the default values
 */
void createLinkLayer();

#endif