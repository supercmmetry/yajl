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

#include "misc.h"
#include "types.h"
#include <cassert>

void YAJLMiscData::fetch_app_data(bitio::bitio_stream *bstream) {
    if (!ignore) {
        // param-length followed by application data.
        u16 length = bstream->read(0x10);
        // exclude Lp from length
        length -= 2;
        auto *_app_data = new char[length];
        for (int i = 0; i < length; i++) {
            _app_data[i] = bstream->read(0x8);
        }
        app_data.push_back(_app_data);
    } else {
        u16 length = bstream->read(0x10);
        length -= 2;
        for (int i = 0; i < length; i++) {
            bstream->read(0x8);
        }
    }
}

void YAJLMiscData::fetch_comment(bitio::bitio_stream *bstream) {
    if (!ignore) {
        // param-length followed by comment-bytes.
        u16 length = bstream->read(0x10);
        length -= 2;
        auto *com_data = new char[length];
        for (int i = 0; i < length; i++) {
            com_data[i] = bstream->read(0x8);
        }
        comments.push_back(com_data);
    } else {
        u16 length = bstream->read(0x10);
        length -= 2;
        for (int i = 0; i < length; i++) {
            bstream->read(0x8);
        }
    }
}

void YAJLTables::fetch_qtable(bitio::bitio_stream *bstream) {
    u32 seg_len = bstream->read(0x10);
    // exclude size of Lq which is 2 bytes
    seg_len -= 2;
    while (seg_len > 0) {
        YAJLQTable table(bstream);
        u8 qk_size = table.precision == 0 ? 8 : 16;
        qtables[table.dest_id] = table;
        seg_len -= 1 + (qk_size << 3);
    }
}

void YAJLTables::fetch_etable(u16 marker, bitio::bitio_stream *bstream) {
    u16 length = bstream->read(0x10);
    length -= 2;
    if (marker == markers::DHT) {
        while (length > 0) {
            YAJLEntropyTable etable{};
            etable.set_marker(marker);
            etable.huffman_table = new YAJLHuffmanTable(bstream);

            length -= 17;
            for (int i = 0; i < 16; i++) {
                length -= etable.huffman_table->ncodes[i];
            }

            assert(etable.huffman_table->table_dest < 4);
            etables[etable.huffman_table->table_dest] = etable;
        }
    } else if (marker == markers::DAC) {
        while (length > 0) {
            YAJLEntropyTable etable{};
            etable.set_marker(marker);
            etable.arithmetic_table = new YAJLArithmeticTable(bstream);

            assert(etable.arithmetic_table->table_dest < 4);
            etables[etable.arithmetic_table->table_dest] = etable;
            length -= 2;
        }
    } else {
        fprintf(stderr, "Unhandled marker at: %s\n", __FILE__);
        exit(1);
    }
}
