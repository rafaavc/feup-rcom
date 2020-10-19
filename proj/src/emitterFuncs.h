#include "protocol.h"


bool stopAndWait(unsigned (*functionToExec)(char*,size_t), char * msgToWrite, size_t msgSize);
bool logicConnectionFunction(char * msg, size_t msgSize);
bool establishLogicConnection();
bool disconnectionFunction(char * msg, size_t msgSize);
bool establishDisconnection();
bool informationExchange(char *msg, size_t msgSize);
bool sendMessage(char* msg, size_t msgSize);