#include "transmitterFuncs.h"

/**
 * Reads information frames from the SP and acts accordingly to the frame received,like confirming the reception or asking for a resend
 * @param buffer array of received chars 
 * @return Number of characters read, negative value in case of error
 */
size_t receiverRead(char * buffer);
/**
 * Conects to the transmitter
 * @return true if it received a SET message and sent an UA message, false otherwise
 */
bool receiverConnect();
/**
 * Disconnects from the transmitter
 * @return true if it received a DISC message, sent an DISC message and received an UA message, false otherwise
 */
size_t receiverDisconnect();
