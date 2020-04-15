#ifndef YAJL_QTABLE_H
#define YAJL_QTABLE_H

#include <cstdint>

struct YAJLQTable {
    uint16_t coeffs[64];
    uint8_t precision;
    uint8_t dest_id;
};

#endif
