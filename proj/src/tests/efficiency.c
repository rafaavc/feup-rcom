#include "efficiency.h"

static unsigned n_Packets = 0;
static double totalTime = 0;
static size_t totalSize = 0;

static unsigned dataErrorCount = 0;
static unsigned headErrorCount = 0;


void rateValuesUpdate(size_t packetSize, double time){
    n_Packets++;
    totalTime += time;
    totalSize += packetSize;
}

double getAverageRate(){
    return (totalSize*8) / totalTime;
}

void calculateEfficiency(){
    unsigned baudrateNum = 0;
    switch(getBaudrate()) {
        case B4800:
            baudrateNum = 4800;
            break;
        case B9600:
            baudrateNum = 9600;
            break;
        case B19200:
            baudrateNum = 19200;
            break;
        case B38400:
            baudrateNum = 38400;
            break;
        case B57600:
            baudrateNum = 57600;
            break;
        case B115200:
            baudrateNum = 115200;
            break;
        case B230400:
            baudrateNum = 230400;
            break;
        default:
            printError("The specified baudrate isn't valid. Available:\n4800\n9600\n19200\n38400\n57600\n115200\n230400\n");
            return;
    }
    printf("Probability head error: %u%%\nProbability data error: %u%%\n\n\n# Error generation\nGenerated %u head errors.\nGenerated %u data errors.\n\n",  PROBABILITY_HEAD, PROBABILITY_DATA, headErrorCount, dataErrorCount);
    printf("T_prop: %u\n", DELAY);
    printf("Baudrate: B%u\n", baudrateNum);
    printf("Frame size: %u\n\n", getFrameSize());
    printf("Total time of transfer: %f\n", totalTime);
    printf("Total size of transfer: %lu, %u packets\n", totalSize, n_Packets);
    printf("Average rate: %lf bits/s\n", getAverageRate());
    double efficiency = getAverageRate()/(double)baudrateNum;
    printf("-- Efficiency: %lf\n", efficiency);
}

void errorsGenerated() {
    printf("Probability head error: %u%%\nProbability data error: %u%%\n\n\n# Error generation\nGenerated %u head errors.\nGenerated %u data errors.\n\n",  PROBABILITY_HEAD, PROBABILITY_DATA, headErrorCount, dataErrorCount);
}

void delayGenerator(){
    usleep(DELAY*1e3);  // DELAY in miliseconds
}


void generateDataError(char *frame, size_t frameSize){
    srand(time(NULL));
    int probability = (rand() % 100) + 1;
    
    if(probability <= PROBABILITY_DATA){
        int randomByte = (rand()% (frameSize - 5)) + 4;
        char randomByteValue = (rand()%177);

        frame[randomByte] = randomByteValue;
        printf("\nGenerate BCC2 with errors\n");

        dataErrorCount++;

    }

}

void generateHeadError(char *buffer){
    srand(time(NULL));
    int probability = (rand() % 100) + 1;

    if(probability <= PROBABILITY_HEAD){
        int randomByte = (rand() % 2);
        char randomByteValue = (rand()%177);

        buffer[randomByte] = randomByteValue;
        printf("\nGenerate BCC1 with errors\n");

        headErrorCount++;

    }
}

