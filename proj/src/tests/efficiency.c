#include "efficiency.h"

double n_Packets = 0;
double averageSum = 0;

extern speed_t BAUDRATE;


void rateValuesUpdate(size_t packetSize, double time){
    n_Packets++;
    averageSum += (packetSize * sizeof(char)) / time;
}

double getAverageRate(){
    return averageSum / n_Packets;
}

double calculateEfficiency(){
    return getAverageRate()/(double) BAUDRATE;
}

void delayGenerator(){
    sleep(DELAY);
}


void generateDataError(char *frame, size_t frameSize){
    srand(time(NULL));
    int probability = (rand() % 100) + 1;
    
    if(probability <= PROBABILITY_DATA){
        int randomByte = (rand()% (frameSize - 5)) + 4;
        char randomByteValue = (rand()%177);

        frame[randomByte] = randomByteValue;
        printf("\nGenerate BCC2 with errors\n");

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

    }
}
