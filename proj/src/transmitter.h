#include <sys/stat.h>
#include "receiver.h"
/**
 * Deals with the transmitter part of the application
 * @param serialPort 
 * @param fileToSend the file to send (NULL if not specified)
 * @param destFile the name of the destination file (NULL if not specified)
 */

void transmitter(int serialPort, char * fileToSend, char * destFile);


/**
 * Constructs the control packet for the application
 * @param ret array of characters where the packet will be stored
 * @param ctrl  controlo byte for the packet
 * @param fileName name of the file to include in the packet
 * @param fileSize size of the file to include in the packet
 * @return the size of the control packet
 */
int constructControlPacket(char * ret,char ctrl, char* fileName, size_t fileSize);

/**
 * Constructs the data packet for the application
 * @param ret array of characters where the packet will be stored
 * @param data data to include in the packet
 * @param dataSize number of bytes of data
 * @param msgNr number of sequence of this data packet
 */
void constructDataPacket(char * ret, char* data, size_t dataSize, int msgNr);