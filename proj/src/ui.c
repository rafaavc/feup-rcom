#include "ui.h"

void printProgressBar(size_t goal, size_t current) {
    #ifndef LOADING_UI
    return;
    #endif
    struct winsize size;
    ioctl(1, TIOCGWINSZ, &size);

    /* size.ws_row is the number of rows, size.ws_col is the number of columns. */
    int numCol = size.ws_col;

    double ratio = (double)current / (double) goal;

    int blockAmount = numCol - 7;

    int filledBlocks = ratio * blockAmount;

    printf("%03i%% [", (int)(ratio*100));
    for (int i = 0; i < filledBlocks; i++) {
        printf("█");
    }
    for (int i = 0; i < blockAmount - filledBlocks; i++) {
        printf(" ");
    }
    printf("]\r");

}

