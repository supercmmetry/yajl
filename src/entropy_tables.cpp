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

#include "entropy_tables.h"

YAJLHuffmanTable::YAJLHuffmanTable(bitio::bitio_stream *bstream) {
    table_class = bstream->read(0x4);
    table_dest = bstream->read(0x4);
    for (int i = 0; i < 16; i++) {
        ncodes[i] = bstream->read(0x8);
    }

    for (int i = 0; i < 16; i++) {
        var_codes[i] = new u8[ncodes[i]];
        for (int j = 0; j < ncodes[i]; j++) {
            var_codes[i][j] = bstream->read(0x8);
        }
    }

    auto tree = YAJLHuffmanTree(this);
}


YAJLArithmeticTable::YAJLArithmeticTable(bitio::bitio_stream *bstream) {
    table_class = bstream->read(0x4);
    table_dest = bstream->read(0x4);
    cs_value = bstream->read(0x8);
}

YAJLHuffmanTree::YAJLHuffmanTree(YAJLHuffmanTable *table) {
    // first we create HUFFSIZE. refer Figure C.1 from ISO 10918
    // since dct-coefficients are scaled to 1-byte, max(K) = 255
    u8 *huffsize = new u8[0x101];
    u16 k = 0;
    u8 i = 1;
    u8 j = 1;
    u16 lastk = 0;
    while (true) {
        if (j > table->ncodes[i-1]) {
            i++;
            j = 1;
            if (i > 16) {
                huffsize[k] = 0;
                lastk = k;
                break;
            }
        } else {
            huffsize[k] = i;
            k++;
            j++;
            continue;
        }
    }

    // now we create HUFFCODE
    u16 *huffcode = new u16[0x101];
    k = 0;
    u16 code = 0;
    u8 si = huffsize[0];

    start:
    huffcode[k] = code;
    code++;
    k++;

    if (huffsize[k] == si) {
        goto start;
    } else {
        if (huffsize[k] == 0) {
            goto end;
        } else {
            shift_point:
            code <<= 1;
            si++;

            if (huffsize[k] == si) {
                goto start;
            } else {
                goto shift_point;
            }
        }
    }

    end:;

    // construct huffval list
    u8 *huffval = new u8[0x101];
    k = 0;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < table->ncodes[i]; j++) {
            huffval[k++] = table->var_codes[i][j];
        }
    }

    // now we order the codes according to symbols.
    k = 0;
    u16 *ehufco = new u16[0x101];
    u8 *ehufsi = new u8[0x101];
    while (k < lastk) {
        i = huffval[k];
        ehufco[i] = huffcode[k];
        ehufsi[i] = huffsize[k];
        k++;
    }

    delete [] huffcode;
    delete [] huffsize;



    // now we construct the huffman tree.
    root = new HFNode;

    for (int i = 0; i < 0x100; i++) {
        int j = ehufsi[i] - 1;
        u16 code = ehufco[i];
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
        if (ehufsi[i] != 0) {
            curr->symbol = i;
        }
    }
}
