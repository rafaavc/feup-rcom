#include "protocol.h"


bool stopAndWait(unsigned (*functionToExec)(char*,size_t), char * msgToWrite, size_t msgSize, size_t res);
bool logicConnectionFunction(char * msg, size_t msgSize);
bool establishLogicConnection();
bool disconnectionFunction(char * msg, size_t msgSize);
bool establishDisconnection();
bool informationExchange(char* msg, size_t msgSize, size_t *res );
bool sendMessage(char* msg, size_t msgSize);