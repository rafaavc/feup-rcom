#include "transmitter.h"

/**
 * 
 * @param ports - 
 */
void printUsage(int * ports);
/**
 * 
 * @param argc 
 * @param argv 
 */

void checkCmdArgs(int argc, char ** argv);
/**
 * Constructs the control packet for the application
 * @param ret array of characters where the packet will be stored
 * @param ctrl  controlo byte for the packet
 * @param fileName name of the file to include in the packet
 * @param fileSize size of the file to include in the packet
 */
void constructControlPacket(char * ret,char ctrl, char* fileName, size_t fileSize);
/**
 * Constructs the data packet for the application
 * @param ret array of characters where the packet will be stored
 * @param data data to include in the packet
 * @param dataSize number of bytes of data
 * @param msgNr number of sequence of this data packet
 */
void constructDataPacket(char * ret, char* data, size_t dataSize, int msgNr);