/*
MIT License

Copyright (c) 2020 Vishaal Selvaraj

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <cassert>
#include "entropy_tables.h"

YAJLHuffmanTable::YAJLHuffmanTable(bitio::bitio_stream *bstream) {
    this->bstream = bstream;
    table_class = bstream->read(0x4);
    table_dest = bstream->read(0x4);

    for (int i = 0; i < 16; i++) {
        ncodes[i] = bstream->read(0x8);
    }

    decode_table();
}

void YAJLHuffmanTable::decode_table() {
    tree = new YAJLHuffmanTable::HFTree(this, bstream);
}

u16 YAJLHuffmanTable::decode() {
    assert(tree != nullptr);
    return tree->decode(bstream);
}


YAJLArithmeticTable::YAJLArithmeticTable(bitio::bitio_stream *bstream) {
    table_class = bstream->read(0x4);
    table_dest = bstream->read(0x4);
    cs_value = bstream->read(0x8);
}

YAJLHuffmanTable::HFTree::HFTree(YAJLHuffmanTable *table, bitio::bitio_stream *bstream) {
    if (table == nullptr) {
        return;
    }



    auto ncodes = table->ncodes;
    u8 *var_codes[16];


    for (int i = 0; i < 16; i++) {
        var_codes[i] = new u8[ncodes[i]];
        for (int j = 0; j < ncodes[i]; j++) {
            var_codes[i][j] = bstream->read(0x8);
        }
    }

    u16 code = 0;
    u32 count = 0;
    root = new HFNode;

    // construct huffman-tree
    for (int i = 0; i < 16; i++) {
        auto in_count = 0;
        for (int j = 0; j < ncodes[i]; j++) {
            insert(code++, i, var_codes[i][in_count]);
            in_count++;
            count++;
        }
        code <<= 1;
    }

    for (int i = 0; i < 0x10; i++) {
        free(var_codes[i]);
    }
}

u16 YAJLHuffmanTable::HFTree::decode(bitio::bitio_stream *bstream) {
    auto *curr = root;
    while (curr->left != nullptr && curr->right != nullptr) {
        if (bstream->peek(0x8) == 0xFF) {
            auto pkval = bstream->peek(0x10);
            if (pkval == 0xFF00) {
                count_bstuff = true;
                bstuff_count = 0;
            }
        }

        if (count_bstuff) {
            bstuff_count++;
        }
        if (bstuff_count == 8) {
            bstuff_count = 0;
            count_bstuff = false;
            bstream->skip(0x8);
        }

        bool msb = bstream->read(0x1);
        if (msb) {
            curr = curr->right;
        } else {
            curr = curr->left;
        }
    }
    return curr->symbol;
}

void YAJLHuffmanTable::HFTree::free_hfnode(YAJLHuffmanTable::HFTree::HFNode *node) {
    if (node == nullptr) {
        return;
    }
    free_hfnode(node->left);
    free_hfnode(node->right);
    free(node);
}

inline void YAJLHuffmanTable::HFTree::insert(u16 code, u8 code_lensh, u8 symbol) {
    int j = code_lensh;
    HFNode *curr = root;
    while (j >= 0) {
        bool msb = (code & (1 << j)) >> j;
        if (msb) {
            if (curr->right == nullptr) {
                curr->right = new HFNode;
            }
            curr = curr->right;
        } else {
            if (curr->left == nullptr) {
                curr->left = new HFNode;
            }
            curr = curr->left;
        }
        j--;
    }
    if (code_lensh != 0) {
        curr->symbol = symbol;
    }
}


void YAJLEntropyTable::set_marker(u16 _marker) {
    marker = _marker;
    if (marker == markers::DAC && huffman_table != nullptr) {
        free(huffman_table);
    }
    if (marker == markers::DHT && arithmetic_table != nullptr) {
        free(arithmetic_table);
    }
}

u16 YAJLEntropyTable::decode() {
    if (marker == markers::DHT && huffman_table != nullptr) {
        return huffman_table->decode();
    } else if (marker == markers::DAC && arithmetic_table != nullptr) {
        //todo: implement decode_table for arithmetic tables.
    }
}

