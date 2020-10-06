#include <stdint.h>

struct ProtocolSU {
    uint8_t flag, addressField, controllField, blockCheckCharacter;
};

struct ProtocolI {
    struct ProtocolSU control;
    uint8_t blockCheckCharacter1;
    char * data;
};


/**
 *
 */
