#include "protocol.h"

bool dealWithReceivedMessage(enum stateMachine state, char * msg, size_t msgSize,enum readFromSPRet res);

void receiverLoop();
