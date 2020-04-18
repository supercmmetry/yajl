#ifndef YAJL_ENTROPY_TABLES_H
#define YAJL_ENTROPY_TABLES_H


#include <cstdlib>
#include <bitio/bitio.h>
#include "markers.h"
#include "types.h"

struct YAJLHuffmanTable {
    u8 table_class;
    u8 table_dest;
    u8 ncodes[16];
    u8 *var_codes[16];

    YAJLHuffmanTable() {
        // empty-constructor
    }

    YAJLHuffmanTable(bitio::bitio_stream *bstream);
};

struct YAJLArithmeticTable {
    u8 table_class;
    u8 table_dest;
    u8 cs_value;

    YAJLArithmeticTable() {
        // empty-constructor
    }

    YAJLArithmeticTable(bitio::bitio_stream *bstream);
};

struct YAJLEntropyTable {
    u16 marker;
    YAJLHuffmanTable *huffman_table = nullptr;
    YAJLArithmeticTable *arithmetic_table = nullptr;

    void set_marker(u16 _marker) {
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
