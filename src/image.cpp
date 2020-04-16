#include <cassert>
#include "image.h"
#include "qtable.h"
#include "frame.h"
#include "htables.h"
#include "markers.h"


void YAJLImage::set_src(std::string filename) {
    bstream = new bitio::bitio_stream(filename, bitio::READ);
}

void YAJLImage::scan_markers() {
    // First check for SOI marker
    if (bstream->read(0x10) != markers::SOI) {
        std::cout << "SOI not found" << std::endl;
        return;
    }
    bool continue_scan = true;
    while (continue_scan && !bstream->is_eof()) {
        uint16_t marker = bstream->read(0x10);
        assert(markers::is_marker(marker));
        printf("marker found: %X\n", marker);

        // we handle SOF markers separately.
        if (markers::is_sof_marker(marker)) {
            uint16_t length = bstream->read(0x10);
            // remove non-recurring lengths.
            length -= 8;

            header.frame_header_data.sof = marker;
            header.frame_header_data.precision = bstream->read(0x8);
            header.frame_header_data.nlines = bstream->read(0x10);
            header.frame_header_data.nsamples_per_line = bstream->read(0x10);
            header.frame_header_data.ncomponents = bstream->read(0x8);

            while (length > 0) {
                YAJLFrameHeaderSpec spec{};
                spec.component_id = bstream->read(0x8);
                spec.horizontal_sampling_factor = bstream->read(0x4);
                spec.vertical_sampling_factor = bstream->read(0x4);
                spec.qtable_dest = bstream->read(0x8);

                header.frame_header_specs.push_back(spec);
                length -= 3;
            }

            continue;
        }

        // channel APPn to APP0
        marker = markers::is_app_marker(marker) ? markers::APP0 : marker;

        switch (marker) {
            case markers::APP0: {
                if (!use_minimal_scan) {
                    // param-length followed by application data.
                    uint16_t length = bstream->read(0x10);
                    // exlude Lp from length
                    length -= 2;
                    auto *app_data = new char[length];
                    for (int i = 0; i < length; i++) {
                        app_data[i] = bstream->read(0x8);
                    }
                    push_app_data(app_data);
                } else {
                    uint16_t length = bstream->read(0x10);
                    length -= 2;
                    for (int i = 0; i < length; i++) {
                        bstream->read(0x8);
                    }
                }
                break;
            }
            case markers::COM: {
                if (!use_minimal_scan) {
                    // param-length followed by comment-bytes.
                    uint16_t length = bstream->read(0x10);
                    length -= 2;
                    auto *com_data = new char[length];
                    for (int i = 0; i < length; i++) {
                        com_data[i] = bstream->read(0x8);
                    }
                    push_com_data(com_data);
                } else {
                    uint16_t length = bstream->read(0x10);
                    length -= 2;
                    for (int i = 0; i < length; i++) {
                        bstream->read(0x8);
                    }
                }
                break;
            }
            case markers::DQT: {
                // first we read 16-bit Lq
                uint32_t seg_len = bstream->read(0x10);
                // exclude size of Lq which is 2 bytes
                seg_len -= 2;
                while (seg_len > 0) {
                    YAJLQTable table{};
                    table.precision = bstream->read(0x4);
                    table.dest_id = bstream->read(0x4);
                    uint8_t qk_size = table.precision == 0 ? 8 : 16;
                    for (int i : scanorders::ZZ) {
                        table.coeffs[i] = bstream->read(qk_size);
                    }
                    add_qtable(table, table.dest_id);
                    seg_len -= 1 + (qk_size << 3);
                }
                break;
            }
            case markers::DHT: {
                uint16_t length = bstream->read(0x10);
                length -= 2;
                while (length > 0) {
                    YAJLEntropyTable etable{};
                    etable.set_marker(marker);
                    etable.huffman_table = new YAJLHuffmanTable;
                    etable.huffman_table->table_class = bstream->read(0x4);
                    etable.huffman_table->table_dest = bstream->read(0x4);
                    for (int i = 0; i < 16; i++) {
                        etable.huffman_table->ncodes[i] = bstream->read(0x8);
                    }

                    length -= 17;

                    for (int i = 0; i < 16; i++) {
                        etable.huffman_table->var_codes[i] = new uint8_t[etable.huffman_table->ncodes[i]];
                        for (int j = 0; j < etable.huffman_table->ncodes[i]; j++) {
                            etable.huffman_table->var_codes[i][j] = bstream->read(0x8);
                        }
                        length -= etable.huffman_table->ncodes[i];
                    }
                    add_etable(etable, etable.huffman_table->table_dest);
                }
                break;

            }
            case markers::DAC: {
                uint16_t length = bstream->read(0x10);
                length -= 2;
                while (length > 0) {
                    YAJLEntropyTable etable{};
                    etable.set_marker(marker);
                    etable.arithmetic_table = new YAJLArithmeticTable;
                    etable.arithmetic_table->table_class = bstream->read(0x4);
                    etable.arithmetic_table->table_dest = bstream->read(0x4);
                    etable.arithmetic_table->cs_value = bstream->read(0x8);
                    add_etable(etable, etable.huffman_table->table_dest);
                    length -= 2;
                }
                default: {
                    continue_scan = false;
#ifdef DEBUG
                    printf("Unhandled marker: %X\n", marker);
#endif
                }
            }

        }
    }


    YAJLImage::YAJLImage(bool
    _use_minimal_scan) {
        bstream = nullptr;
        use_minimal_scan = _use_minimal_scan;
    }


    char *YAJLImageDescriptor::get_app_data(uint32_t index) {
        if (index < ds_app_data.size()) {
            return ds_app_data[index];
        } else {
            return nullptr;
        }
    }

    void YAJLImageDescriptor::push_app_data(char *app_data) {
        ds_app_data.push_back(app_data);
    }

    void YAJLImageDescriptor::push_com_data(char *com_data) {
        ds_comments.push_back(com_data);
    }

    char *YAJLImageDescriptor::get_comment(uint32_t index) {
        if (index < ds_comments.size()) {
            return ds_comments[index];
        } else {
            return nullptr;
        }
    }

    void YAJLImageDescriptor::add_qtable(YAJLQTable table, uint8_t index) {
        assert(index < 4);
        qtables[index] = table;
    }

    YAJLQTable YAJLImageDescriptor::get_qtable(uint8_t index) {
        assert(index < 4);
        return qtables[index];
    }

    void YAJLImageDescriptor::add_etable(YAJLEntropyTable table, uint8_t index) {
        assert(index < 4);
        etables[index] = table;
    }
