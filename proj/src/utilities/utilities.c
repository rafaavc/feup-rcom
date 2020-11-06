#include "utilities.h"
#include <math.h>

void debugMessage(char * msg) {
    #ifndef DEBUG
    return;
    #endif
    printf("- %s\n", msg);
}

void * myMalloc(size_t size) {
    void * ret = malloc(size);
    if (ret == NULL) {
        perror("Error alocating memory.");
        exit(EXIT_FAILURE);
    }
    return ret;
}

bool isUnsignedNumber(char * str) {
    for(int i = 0; str[i] != '\0'; i++) {
        if(!(str[i] >= '0' && str[i] <= '9')) {
            return false;
        }
    }
    return true;
}

void printCharArray(char * arr, size_t arrSize) {
    int count = 0;
    for (size_t i = 0; i < arrSize; i++) {
        count++;
        printf("%x ", (unsigned char)arr[i]);
    }
    printf("\ntotal: %d, dados: %d\n", count, count-6);
    printf("\n");
}

void initTimer(struct myTimer * timer, char * name) {
    timer->startTime = -1;
    timer->timerName = name;
}

void startTimer(struct myTimer * timer) {
    timer->startTime = clock();
}

double stopTimer(struct myTimer * timer, bool verbose) {
    if (timer->startTime == -1) return -1;

    clock_t endTime = clock();
    clock_t timeItTook = endTime - timer->startTime;

    double timeInSeconds = ((double)timeItTook)/CLOCKS_PER_SEC;

    if (verbose) {
        printf("Timer %s took %lf seconds\n", timer->timerName, timeInSeconds);
    }

    return timeInSeconds;
}


void addIntToFilename(char * fileNameNext, char * fileName, int tries) {
    unsigned numberOfDigits = floor(log10(abs((double) tries)) + 1);
    int fileNameSize = strlen(fileName);
    int lastDotIdx = -1;
    for (unsigned i = 0; i < fileNameSize; i++) {
        if (fileName[i] == '.') {
            lastDotIdx = i;
        }
    }
    if (lastDotIdx != -1) {
        char * fileNameHolder = malloc(strlen(fileName)*sizeof(char));
        strcpy(fileNameHolder, fileName);
        fileNameHolder[lastDotIdx] = '\0';
        sprintf(fileNameNext, "%s%d", fileNameHolder, tries);
        for (int i = lastDotIdx; i < fileNameSize; i++) {
            fileNameNext[i + numberOfDigits] = fileName[i];
        }
        fileNameNext[fileNameSize+numberOfDigits] = '\0';
        free(fileNameHolder);
    } else {
        sprintf(fileNameNext, "%s%d", fileName, tries);
    }

}


