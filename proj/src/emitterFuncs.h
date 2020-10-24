#include "protocol.h"

/**
 * 
 * @param functionToExec - 
 * @param msgToWrite -
 * @param msgSize -
 * @param res -
 * @return 
 */
bool stopAndWait(unsigned (*functionToExec)(char*,size_t, size_t*), char * msgToWrite, size_t msgSize, size_t *res);
/**
 * 
 * @param msg -
 * @param msgSize -
 * @param res -
 * @return 
 */

bool logicConnectionFunction(char * msg, size_t msgSize,size_t *res );
/**
 * 
 * @return 
 */
bool establishLogicConnection();
/**
 * 
 * @param msg -
 * @param msgSize -
 * @param res -
 * @return 
 */
bool disconnectionFunction(char * msg, size_t msgSize, size_t *res);
/**
 * 
 * @return 
 */
bool establishDisconnection();
/**
 * 
 * @param msg -
 * @param msgSize -
 * @param res -
 * @return 
 */
bool informationExchange(char* msg, size_t msgSize, size_t *res );
/**
 * 
 * @param msg -
 * @param msgSize -
 * @return 
 */
bool sendMessage(char* msg, size_t msgSize);