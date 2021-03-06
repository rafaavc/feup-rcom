#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "../protocol/iPA.h"
#include "defines.h"
#include "../utilities/ui.h"


enum checkReceptionState{CTRL, T, L, V, RECEIVING_DATA, DATA_N, DATA_L1, DATA_L2, DATA_FINISHED, END_RECEIVED, START_RECEIVED, ERROR};

/**
 * Checks the state of valid data the currently read packet gets
 * @param buffer buffer where the received packet is stored
 * @param bufferSize size of the buffer
 * @param fileSize variable where the fileSize will be stored once parsed
 * @param fileName variable where the fileName will be stored once parsed
 * @param dataAmount amount of data read in the current packet
 * @return the final state of the packet read
 */
enum checkReceptionState checkStateReception(char * buffer, int bufferSize, size_t * fileSize, char ** fileName, int * dataAmount);
