#ifndef YAJL_ENTROPY_TABLES_H
#define YAJL_ENTROPY_TABLES_H

#include <cstdint>
#include <cstdlib>
#include <bitio/bitio.h>
#include "markers.h"

struct YAJLHuffmanTable {
    uint8_t table_class;
    uint8_t table_dest;
    uint8_t ncodes[16];
    uint8_t *var_codes[16];

    YAJLHuffmanTable() {
        // empty-constructor
    }

    YAJLHuffmanTable(bitio::bitio_stream *bstream);
};

struct YAJLArithmeticTable {
    uint8_t table_class;
    uint8_t table_dest;
    uint8_t cs_value;

    YAJLArithmeticTable() {
        // empty-constructor
    }

    YAJLArithmeticTable(bitio::bitio_stream *bstream);
};

struct YAJLEntropyTable {
    uint16_t marker;
    YAJLHuffmanTable *huffman_table = nullptr;
    YAJLArithmeticTable *arithmetic_table = nullptr;

    void set_marker(uint16_t _marker) {
        marker = _marker;
        if (marker == markers::DAC && huffman_table != nullptr) {
            free(huffman_table);
        }
        if (marker == markers::DHT && arithmetic_table != nullptr) {
            free(arithmetic_table);
        }
    }
};



#endif
