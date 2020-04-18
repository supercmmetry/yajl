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
}


YAJLArithmeticTable::YAJLArithmeticTable(bitio::bitio_stream *bstream) {
    table_class = bstream->read(0x4);
    table_dest = bstream->read(0x4);
    cs_value = bstream->read(0x8);
}