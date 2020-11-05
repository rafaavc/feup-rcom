#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include "utilities.h"

/**
 * Draws a loading progress bar on the screen
 * @param goal maximum value
 * @param current current loading value
 */
void printProgressBar(size_t goal, size_t current);
