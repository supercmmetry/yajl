#include "entropy_tables.h"

YAJLHuffmanTable::YAJLHuffmanTable(bitio::bitio_stream *bstream) {
    table_class = bstream->read(0x4);
    table_dest = bstream->read(0x4);
    for (int i = 0; i < 16; i++) {
        ncodes[i] = bstream->read(0x8);
    }

    for (int i = 0; i < 16; i++) {
        var_codes[i] = new uint8_t[ncodes[i]];
        for (int j = 0; j < ncodes[i]; j++) {
            var_codes[i][j] = bstream->read(0x8);
        }
    }
}


YAJLArithmeticTable::YAJLArithmeticTable(bitio::bitio_stream *bstream) {
    table_class = bstream->read(0x4);
    table_dest = bstream->read(0x4);
    cs_value = bstream->read(0x8);
}