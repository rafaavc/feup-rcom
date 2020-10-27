#include "protocol.h"

/**
 * 
 * @param functionToExec function pointer to the function that will be timed out (the function returns true when everything goes right, false when it doesn't get a response or an error occurs)
 * @param msgToWrite buffer with the message (that will be sent to the functionToExec)
 * @param msgSize buffer with the message (that will be sent to the functionToExec)
 * @param res amount of bytes written (by the functionToExec)
 * @return true if is able to get a response, exits the program otherwise
 */
bool stopAndWait(bool (*functionToExec)(char*,size_t, size_t*), char * msgToWrite, size_t msgSize, size_t *res);
/**
 * Sends the connection messages and waits for the answer 
 * @param msg message to write in the connect establishment
 * @param msgSize size of the message   
 * @param res number of bytes written
 * @return  true if it was able to connect, false otherwise
 */

bool logicConnectionFunction(char * msg, size_t msgSize,size_t *res );
/**
 * Writes the supervision message and makes sure the all process of connection went well
 * @return true if it was able to connect, false otherwise
 */
bool establishLogicConnection();
/**
 * Sends the disconnection message and waits for the answer
 * @param msg message to write
 * @param msgSize message size
 * @param res bytes written
 * @return true if all went well, false otherwise
 */
bool disconnectionFunction(char * msg, size_t msgSize, size_t *res);
/**
 * 
 * @return  true if it was able to disconnect, false otherwise
 */
bool establishDisconnection();
/**
 * Reads the messages from the SP and sends the ACK/NACK messages accordingly
 * @param msg message to send
 * @param msgSize size of the message 
 * @param res 
 * @return true if no error occurred in the message transmission and the data is valid, false otherwise
 */
bool informationExchange(char* msg, size_t msgSize, size_t *res );
/**
 * Calls the function that will deal with the stop&wait process of reading
 * @param msg Message to be written
 * @param msgSize message size
 * @return number of bytes written
 */
bool sendMessage(char* msg, size_t msgSize);