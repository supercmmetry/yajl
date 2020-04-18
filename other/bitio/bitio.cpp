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


#include "bitio.h"

using namespace bitio;

bitio_stream::bitio_stream(std::string filename, access_enum op, uint64_t buffer_size) {
    file = nullptr;
    this->filename = filename;

    if ((op == READ) && !(file = fopen(filename.c_str(), "rb"))) {
        return;
    } else if (op == WRITE) {
        file = fopen(filename.c_str(), "wb");
    } else if (op == APPEND) {
        file = fopen(filename.c_str(), "a");
    }

    this->buffer_size = buffer_size;
    byte_buffer = (unsigned char *) malloc(sizeof(unsigned char) * buffer_size);
    bit_count = 0;
    eof = false;
    bit_buffer = 0;
    current_buffer_length = 0;
    byte_index = 0;

}


void bitio_stream::close() {
    free(byte_buffer);
    if (file != nullptr) fclose(file);
}

inline void bitio_stream::load_buffer() {
    current_buffer_length = fread(byte_buffer, 1, buffer_size, file);
    eof = current_buffer_length == 0;
    byte_index = 0;
}

inline void bitio_stream::load_byte() {
    if (eof)
        bit_buffer = 0;
    if (byte_index == current_buffer_length)
        load_buffer();
    bit_buffer = byte_buffer[byte_index++];
}

inline void bitio_stream::wflush() {
    fwrite(byte_buffer, 1, buffer_size, file);
}

uint64_t bitio_stream::read(uint64_t n) {
    uint64_t value = 0;
    if (bit_count == 0) {
        load_byte();
        bit_count = 8;
    }

    if (bit_count >= n) {
        value += (bit_buffer & reverse_bit_masks[n]) >> (8 - n);
        bit_buffer <<= n;
        bit_count -= n;
    } else {
        value += bit_buffer >> (8 - bit_count);
        char target_bits = n - bit_count;
        char nbytes = target_bits >> 3;
        bit_buffer = 0;
        bit_count = 0;

        while (nbytes--) {
            load_byte();
            value <<= 8;
            value += bit_buffer;
        }
        load_byte();
        bit_count = 8;

        char rembits = target_bits & bit_masks[3];

        value <<= rembits;
        value += (bit_buffer & reverse_bit_masks[rembits]) >> (8 - rembits);
        bit_buffer <<= rembits;
        bit_count -= rembits;
    }
    return value & ui64_masks[n];
}

void bitio_stream::skip(uint64_t n) {
    while (n > 0) {
        if (n > 0x40) {
            read(0x40);
            n -= 0x40;
        } else {
            read(n);
            n = 0;
        }
    }
}

void bitio_stream::write(uint64_t obj, uint64_t n) {
    uint64_t i = 0;
    obj <<= 0x40 - n;

    unsigned char mask_index = 0;
    while (i++ < n) {
        bit_buffer <<= 1;
        bit_buffer += (obj & ui64_single_bit_masks[0x3f - mask_index++]) != 0;;
        bit_count++;
        if (bit_count == 8) {
            byte_buffer[byte_index++] = bit_buffer;
            if (byte_index == buffer_size) {
                wflush();
                byte_index = 0;
            }
            bit_buffer = 0;
            bit_count = 0;
        }

    }
}

// aligns to next-byte, meaningful only for read operations.
void bitio_stream::align() {
    read(bit_count);
    //load_byte();
}

// write residues and align to next-byte.
void bitio_stream::flush() {
    if (bit_count != 0) {
        bit_buffer <<= 8 - bit_count;
        byte_buffer[byte_index++] = bit_buffer;
    }
    fwrite(byte_buffer, 1, byte_index, file);
}

bool bitio_stream::is_eof() {
    if (eof) return true;
    if (byte_index == current_buffer_length) {
        auto *tmp_ptr = new char[1];
        eof = fread(tmp_ptr, 1, 1, file) == 0;
        free(tmp_ptr);

        if (!eof) fseek(file, -1, SEEK_CUR);
    }
    return eof;
}

uint64_t bitio_stream::get_file_size() {
    FILE *tmp = fopen(filename.c_str(), "rb");
    uint64_t count = 0;
    char *tmp_ptr = new char[1];
    while (fread(tmp_ptr, 1, 1, tmp) != 0) {
        count++;
    }
    free(tmp_ptr);
    fclose(tmp);
    return count;
}



