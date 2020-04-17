#include "qtable.h"

YAJLQTable::YAJLQTable(bitio::bitio_stream *bstream) {
    precision = bstream->read(0x4);
    dest_id = bstream->read(0x4);
    uint8_t qk_size = precision == 0 ? 8 : 16;
    for (int i : scanorders::ZZ) {
        coeffs[i] = bstream->read(qk_size);
    }
}
