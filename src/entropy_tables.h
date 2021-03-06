#ifndef YAJL_ENTROPY_TABLES_H
#define YAJL_ENTROPY_TABLES_H

#include <cstdlib>
#include <bitio/bitio.h>
#include "markers.h"
#include "types.h"

class YAJLHuffmanTable {
private:
    bitio::bitio_stream *bstream;

    void decode_table();

public:
    class HFTree {
    private:
        struct HFNode {
            HFNode *left = nullptr;
            HFNode *right = nullptr;
            u8 symbol{};
        };

        HFNode *root = nullptr;
        bool marker_found = false;
        bool count_bstuff = false;
        uint8_t bstuff_count = 0;

        void free_hfnode(HFNode *node);
        inline void insert(u16 code, u8 code_lensh, u8 symbol);

    public:
        HFTree() {
            // empty-constructor
        }

        ~HFTree() {
            // de-allocate huffman tree.
            free_hfnode(root);
        }

        HFTree(YAJLHuffmanTable *table, bitio::bitio_stream *bstream);

        u16 decode(bitio::bitio_stream *bstream);
    };

    u8 table_class;
    u8 table_dest;
    u8 ncodes[16];
    HFTree *tree = nullptr;

    YAJLHuffmanTable() {
        // empty-constructor
    }

    YAJLHuffmanTable(bitio::bitio_stream *bstream);

    u16 decode();
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

    void set_marker(u16 _marker);

    u16 decode();
};


#endif
