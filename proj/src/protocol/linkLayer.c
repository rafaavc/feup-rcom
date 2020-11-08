#include "defines.h"  // incluidos aqui de forma à aplicação não ter acesso aos defines do protocolo (porque o linkLayer é incluído pelo iPA.h)
#include "linkLayer.h"

static bool createdLayer = false;

struct linkLayer { // put here so other files don't know anything about this struct
    speed_t BAUDRATE;
    unsigned FRAME_SIZE, 
        MAX_DATA_PACKET_SIZE, 
        MAX_FRAME_BUFFER_SIZE, 
        TIMEOUT, 
        NUM_TRANSMISSIONS,
        nextSequenceNumber;
    int fd;  // serial port file descriptor
    struct termios oldtio;
};

static struct linkLayer llayer;


speed_t getBaudrate() {
    return llayer.BAUDRATE;
}
unsigned getFrameSize() {
    return llayer.FRAME_SIZE;
}
unsigned getMaxDataPacketSize() {
    return llayer.MAX_DATA_PACKET_SIZE;
}
unsigned getMaxFrameBufferSize() {
    return llayer.MAX_FRAME_BUFFER_SIZE;
}
unsigned getTimeout() {
    return llayer.TIMEOUT;
}
unsigned getNumTransmissions() {
    return llayer.NUM_TRANSMISSIONS;
}
struct termios * getOldTIO() {
    return &llayer.oldtio;
}
unsigned getNextSequenceNumber() {
    return llayer.nextSequenceNumber;
} 
int getFD() {
    return llayer.fd;
} 


void incrementNextSequenceNumber() {
    llayer.nextSequenceNumber = (llayer.nextSequenceNumber+1)%2;
} 

void setFD(int fd) {
    llayer.fd = fd;
} 

void setTimeout(unsigned timeout) {
    llayer.TIMEOUT = timeout;
    printf("Set timeout = %u\n", timeout);
}

void setNumTransmissions(unsigned numTransmissions) {
    llayer.NUM_TRANSMISSIONS = numTransmissions;
    printf("Set num transmissions = %u\n", numTransmissions);
}

void setBaudrate(speed_t baudrate) {
    llayer.BAUDRATE = baudrate;
    printf("Set baudrate = %u\n", baudrate);
}

void setFrameSize(unsigned frameSize) {
    llayer.FRAME_SIZE = frameSize;
    llayer.MAX_DATA_PACKET_SIZE = llayer.FRAME_SIZE - SUPERVISION_MSG_SIZE - 1;
    llayer.MAX_FRAME_BUFFER_SIZE = llayer.FRAME_SIZE * 2;
    printf("Set frame size = %u\n", frameSize);
}

void createLinkLayer() {
    if (createdLayer) return;
    createdLayer = true;
    setFrameSize(500);
    setBaudrate(B38400);
    setNumTransmissions(5);
    setTimeout(3);
    setFD(-1);
    llayer.nextSequenceNumber = 0;
}