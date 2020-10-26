#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "protocol/iPA.h"
#include "defines.h"
#include "ui.h"
/**
 * Deals with the receiver part of the application
 * @param serialPort 
 */

void receiver(int serialPort);

enum checkReceptionState checkStateReception(char * buffer, int bufferSize, size_t * fileSize, char ** fileName, int * dataAmount);
