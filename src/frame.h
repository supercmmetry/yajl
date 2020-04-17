#ifndef YAJL_FRAME_H
#define YAJL_FRAME_H

#include <cstdint>
#include <vector>
#include <bitio/bitio.h>
#include "qtable.h"
#include "entropy_tables.h"



struct YAJLFrameHeaderSpec {
    uint8_t component_id;
    uint8_t horizontal_sampling_factor;
    uint8_t vertical_sampling_factor;
    uint8_t qtable_dest;

    YAJLFrameHeaderSpec() {
        // empty-constructor
    }

    YAJLFrameHeaderSpec(bitio::bitio_stream *bstream);
};

struct YAJLFrameHeaderData {
    uint16_t sof;
    uint8_t precision;
    uint16_t nlines;
    uint16_t nsamples_per_line;
    uint8_t ncomponents;

    YAJLFrameHeaderData() {
        // empty-constructor
    }

    YAJLFrameHeaderData(uint16_t marker, bitio::bitio_stream *bstream);
};

struct YAJLFrameHeader {
    YAJLFrameHeaderData data;
    std::vector<YAJLFrameHeaderSpec> specs;
};

struct YAJLScanHeaderSpec {
    uint8_t component_selector;
    uint8_t dc_selector;
    uint8_t ac_selector;

    YAJLScanHeaderSpec() {
        // empty-constructor
    }

    YAJLScanHeaderSpec(bitio::bitio_stream *bstream);
};

struct YAJLScanHeader {
    uint8_t ncomponents;
    std::vector<YAJLScanHeaderSpec> specs;
    uint8_t select_start;
    uint8_t select_end;
    uint8_t ah;
    uint8_t al;

    YAJLScanHeader() {
        // empty-constructor
    }

    YAJLScanHeader(bitio::bitio_stream *bstream);
};

#endif
