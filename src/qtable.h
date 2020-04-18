#ifndef YAJL_QTABLE_H
#define YAJL_QTABLE_H

#include <cstdint>
#include <bitio/bitio.h>
#include "types.h"


struct YAJLQTable {
    u16 *coeffs;
    u8 precision;
    u8 dest_id;

    YAJLQTable() {
        // empty-constructor
    }

    YAJLQTable(bitio::bitio_stream *bstream);
};

#endif
