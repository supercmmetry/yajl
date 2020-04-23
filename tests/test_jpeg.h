#ifndef HYBRIDZIP_JPEG_CODEC_H
#define HYBRIDZIP_JPEG_CODEC_H

#include <cstdio>
#include <jpeglib.h>
#include <string>
#include <vector>
#include "types.h"

struct DCTCoeffArrays {
    std::vector<int**> *coeff_arrays;
    uint8_t nchannels;

    DCTCoeffArrays(uint8_t _nchannels) {
        nchannels = _nchannels;
        coeff_arrays = new std::vector<int**>[nchannels];
    }

    void push_array(uint8_t channel, int** arr) {
        coeff_arrays[channel].push_back(arr);
    }
};

class JPEGCodec {
private:
    FILE *jpeg_file;

    void dct_arr_alloc(int** &arr) {
        arr = new int*[8];
        for (int i = 0; i < 8; i++) {
            arr[i] = new int[8];
        }
    }
public:
    JPEGCodec(uint8_t *buf, uint64_t len) {
        jpeg_file = fmemopen(buf, len, "rb");
    }

    JPEGCodec(std::string filename) {
        jpeg_file = fopen(filename.c_str(), "rb");
    }

    DCTCoeffArrays read_coefficients() {
        jpeg_decompress_struct info;
        jpeg_error_mgr err;

        info.err = jpeg_std_error(&err);
        jpeg_create_decompress(&info);
        jpeg_stdio_src(&info, jpeg_file);
        jpeg_read_header(&info, true);

        auto *coeff_arrays = jpeg_read_coefficients(&info);
        auto dct_coeffs = DCTCoeffArrays(info.num_components);

        for (int ci = 0; ci < info.num_components; ci++) {
            jpeg_component_info *compptr = info.comp_info + ci;
            for (int by = 0; by < compptr->height_in_blocks; by++) {
                JBLOCKARRAY buffer = (info.mem->access_virt_barray)((j_common_ptr) &info, coeff_arrays[ci], by,
                                                                    (JDIMENSION) 1, FALSE);
                for (int bx = 0; bx < compptr->width_in_blocks; bx++) {
                    // We are now at a 8x8 dct-block.
                    int** dct_arr;
                    dct_arr_alloc(dct_arr);

                    for (int coeff_index = 0; coeff_index < 0x40; coeff_index++) {
                        dct_arr[coeff_index / 8][coeff_index % 8] = buffer[0][bx][coeff_index];
                    }
                    dct_coeffs.push_array(ci, dct_arr);
                }
            }
        }

        return dct_coeffs;
    }
};

#endif