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
    class YAJLHuffmanTree {
    private:
        struct HFNode {
            HFNode *left;
            HFNode *right;
            u8 symbol;
        };

        HFNode *root;

        void free_hfnode(HFNode *node);

    public:
        YAJLHuffmanTree() {
            // empty-constructor
        }

        ~YAJLHuffmanTree() {
            // de-allocate huffman tree.
            free_hfnode(root);
        }

        YAJLHuffmanTree(YAJLHuffmanTable *table, bitio::bitio_stream *bstream);

        u8 decode(bitio::bitio_stream *bstream);
    };

    u8 table_class;
    u8 table_dest;
    u8 ncodes[16];
    YAJLHuffmanTree *tree = nullptr;

    YAJLHuffmanTable() {
        // empty-constructor
    }

    YAJLHuffmanTable(bitio::bitio_stream *bstream);

    u8 decode();
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

    u8 decode();
};


#endif
