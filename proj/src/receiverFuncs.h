#include "emitterFuncs.h"

bool dealWithReceivedMessage(enum stateMachine state, char * msg, size_t msgSize,enum readFromSPRet res);

size_t receiverLoop(char * buffer);
