/* INTERFACE PROTOCOL-APPLICATION */

/**
 * 
 * @param porta - serial port
 * @param role -TRANSMITTER / RECEIVER
 * @return data link identifier, negative value in case of error
 */
int llopen(int porta, char * role);

/**
 * 
 * @param fd - data link identifier
 * @param buffer - array of chars to be transmitted
 * @param length - char array length
 * @return number of characters written, negative value in case of error
 */
int llwrite(int fd, char * buffer, int length);

/**
 * 
 * @param fd - data link identifier
 * @param buffer - array of received chars 
 * @return number of characters read, negative value in case of error
 */
int llread(int fd, char * buffer);
  
/**
 * 
 * @param fd - data link identifier
 * @return positive number in case of success, negative value in case of error
 */
int llclose(int fd);


/**
 * Sets the baudrate and the frameSize; done to help evaluate eficiency
 * @param baudrateArg the baudrate
 * @param frameSizeArg the frame size
 */
void llset(int baudrateArg, int frameSizeArg);
