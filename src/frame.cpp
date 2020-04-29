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

#include "frame.h"
#include <cmath>

YAJLFrameHeaderSpec::YAJLFrameHeaderSpec(bitio::bitio_stream *bstream) {
    component_id = bstream->read(0x8);
    horizontal_sampling_factor = bstream->read(0x4);
    vertical_sampling_factor = bstream->read(0x4);
    qtable_dest = bstream->read(0x8);
}

YAJLFrameHeader::YAJLFrameHeader(u16 marker, bitio::bitio_stream *bstream) {
    u16 length = bstream->read(0x10);
    // remove non-recurring lengths.
    length -= 8;

    sof = marker;
    precision = bstream->read(0x8);
    nlines = bstream->read(0x10);
    nsamples_per_line = bstream->read(0x10);
    ncomponents = bstream->read(0x8);

    while (length > 0) {
        YAJLFrameHeaderSpec spec(bstream);
        specs.push_back(spec);
        length -= 3;
    }
}

YAJLScanHeaderSpec::YAJLScanHeaderSpec(bitio::bitio_stream *bstream) {
    component_selector = bstream->read(0x8);
    dc_selector = bstream->read(0x4);
    ac_selector = bstream->read(0x4);
}

YAJLScanHeader::YAJLScanHeader(bitio::bitio_stream *bstream) {
    u16 length = bstream->read(0x10);
    ncomponents = bstream->read(0x8);
    length -= 6;
    while (length > 0) {
        YAJLScanHeaderSpec spec(bstream);
        specs.push_back(spec);
        length -= 2;
    }
    select_start = bstream->read(0x8);
    select_end = bstream->read(0x8);
    ah = bstream->read(0x4);
    al = bstream->read(0x4);
}

YAJLScan::YAJLScan(YAJLMiscData *misc, YAJLFrameHeader *fheader, bitio::bitio_stream *bstream) {
    // we fill YAJLScan with data fetch directly over bitio streams.
    bool run = true;
    while (run) {
        u16 marker = bstream->read(0x10);
        // since we are inside a Scan, we don't need to handle SOF markers
        if (markers::is_app_marker(marker)) {
            misc->fetch_app_data(bstream);
        } else if (marker == markers::COM) {
            misc->fetch_comment(bstream);
        } else if (marker == markers::DQT) {
            tables.fetch_qtable(bstream);
        } else if (marker == markers::DHT || marker == markers::DAC) {
            tables.fetch_etable(marker, bstream);
        } else if (marker == markers::SOS) {
            header = YAJLScanHeader(bstream);
            // scan for MCUs here
            // if Ns > 1, then the MCUs are interleaved. refer B.2.3 from ISO 10918
            // the interleaved components are ordered in this fashion: Cs1, Cs2 ... Csj

            int index = 0;
            u32 mcu_count = 0;
            u32 mcu_max_count = 0;
            u8 max_sampling_factor = 0;
            for (auto spec : fheader->specs) {
                auto sfactor = spec.horizontal_sampling_factor * spec.vertical_sampling_factor;
                if (max_sampling_factor < sfactor) {
                    max_sampling_factor = sfactor;
                }
            }

            for (int i = 0; i < header.ncomponents; i++) {
                auto sampling_factor = fheader->specs[i].horizontal_sampling_factor * fheader->specs[i].vertical_sampling_factor;
                mcu_max_count += ceil((float) (fheader->nlines * fheader->nsamples_per_line) * sampling_factor / (64 * max_sampling_factor));
            }

            while (mcu_count < mcu_max_count) {

                auto cmp_id = header.specs[index].component_selector - 1;
                auto fspec = fheader->specs[cmp_id];
                for (int r = 0; r < fspec.vertical_sampling_factor * fspec.horizontal_sampling_factor; r++) {
                    mcus.emplace_back(bstream, &tables, &header.specs[cmp_id]);
                    mcu_count++;
                }
                index = (index + 1) % header.ncomponents;
            }

            bstream->force_align();
            printf("%x\n", bstream->read(0x10));

#ifdef DEBUG
            printf("Run completed at: %s for 'Scan'\n", __FILE__);
#endif
            run = false;
        } else {
            fprintf(stderr, "Unhandled marker at: %s\n", __FILE__);
            exit(1);
        }
    }
}

YAJLFrame::YAJLFrame(YAJLMiscData *misc, bitio::bitio_stream *bstream) {
    bool run = true;
    while (run) {
        u16 marker = bstream->read(0x10);
        // since we are inside a Scan, we don't need to handle SOF markers
        if (markers::is_app_marker(marker)) {
            misc->fetch_app_data(bstream);
        } else if (marker == markers::COM) {
            misc->fetch_comment(bstream);
        } else if (marker == markers::DQT) {
            tables.fetch_qtable(bstream);
        } else if (marker == markers::DHT || marker == markers::DAC) {
            tables.fetch_etable(marker, bstream);
        } else if (marker == markers::DNL) {
            // skip the length parameter as we know that Nl is a u16.
            bstream->skip(0x10);
            header.nlines = bstream->read(0x10);
        } else if (markers::is_sof_marker(marker)) {
            header = YAJLFrameHeader(marker, bstream);
            // now we parse one Scan

            scans.emplace_back(misc, &header, bstream);
#ifdef DEBUG
            printf("Run completed at: %s for 'Frame'\n", __FILE__);
#endif
            run = false;
        } else {
            fprintf(stderr, "Unhandled marker at: %s\n", __FILE__);
            exit(1);
        }
    }
}


inline i16 YAJLScan::MCU::extract_xv(bitio::bitio_stream *bstream, u8 ebits) {
    if (ebits == 0) {
        return 0;
    }

    i8 sign = (bstream->read(0x1) == 1) ? 1 : -1;
    i16 absv = bstream->read(ebits - 1);

    if (ebits == 1) {
        return sign;
    } else {
        if (sign == 1) {
            return (1 << (ebits - 1)) + absv;
        } else {
            return -(1 << (ebits)) + absv + 1;
        }
    }
}

inline i16 YAJLScan::MCU::fill_xac(bitio::bitio_stream *bstream, YAJLEntropyTable *table) {
    int i = 1;
    while (i < 0x40) {
        u8 val = table->decode();
        if (val == 0) {
            break;
        }

        u8 zrl = val >> 0x4;
        u8 ebits = val & 0xF;

        i += zrl;
        xcof[scanorders::ZZ[i++]] = extract_xv(bstream, ebits);
    }
}

YAJLScan::MCU::MCU(bitio::bitio_stream *bstream, YAJLTables *tables, YAJLScanHeaderSpec *spec) : MCU() {
    component_id = spec->component_selector - 1;
    auto dc_table = tables->etables[spec->dc_selector][0];
    auto ac_table = tables->etables[spec->ac_selector][1];
    u8 ebits = dc_table.decode();
    xcof[0] = extract_xv(bstream, ebits);
    fill_xac(bstream, &ac_table);
}

