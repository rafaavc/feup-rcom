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
    timer->started = false;
    timer->timerName = name;
}

void startTimer(struct myTimer * timer) {
    timer->started = true;
    clock_gettime(CLOCK_REALTIME, &(timer->startTime));
}

double stopTimer(struct myTimer * timer, bool verbose) {
    if (!timer->started) return -1;

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);


    time_t sElapsed  = endTime.tv_sec - timer->startTime.tv_sec;
    time_t nsElapsed = endTime.tv_nsec - timer->startTime.tv_nsec;

    double timeInSeconds = sElapsed + nsElapsed/1.0e9;

    if (verbose) {
        printf("Timer %s took %lf seconds\n", timer->timerName, timeInSeconds);
    }

    return timeInSeconds;
}


void addIntToFilename(char * fileNameNext, char * fileName, int tries) {
    unsigned numberOfDigits = floor(log10(abs(tries)) + 1);
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


