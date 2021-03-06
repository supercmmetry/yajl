#ifndef YAJL_FRAME_H
#define YAJL_FRAME_H

#include <cstdint>
#include <vector>
#include <bitio/bitio.h>
#include "qtable.h"
#include "htables.h"
#include "entropy_tables.h"
#include "types.h"
#include "misc.h"


struct YAJLFrameHeaderSpec {
    u8 component_id;
    u8 horizontal_sampling_factor;
    u8 vertical_sampling_factor;
    u8 qtable_dest;

    YAJLFrameHeaderSpec() {
        // empty-constructor
    }

    YAJLFrameHeaderSpec(bitio::bitio_stream *bstream);
};

struct YAJLFrameHeader {
    u16 sof;
    u8 precision;
    u16 nlines;
    u16 nsamples_per_line;
    u8 ncomponents;
    std::vector<YAJLFrameHeaderSpec> specs;

    YAJLFrameHeader() {
        // empty-constructor
    }

    YAJLFrameHeader(u16 marker, bitio::bitio_stream *bstream);
};

struct YAJLScanHeaderSpec {
    u8 component_selector;
    u8 dc_selector;
    u8 ac_selector;

    YAJLScanHeaderSpec() {
        // empty-constructor
    }

    YAJLScanHeaderSpec(bitio::bitio_stream *bstream);
};

struct YAJLScanHeader {
    u8 ncomponents;
    std::vector<YAJLScanHeaderSpec> specs;
    u8 select_start;
    u8 select_end;
    u8 ah;
    u8 al;

    YAJLScanHeader() {
        // empty-constructor
    }

    YAJLScanHeader(bitio::bitio_stream *bstream);
};

struct YAJLScan {
public:
    struct MCU {
    private:
        inline i16 extract_xv(bitio::bitio_stream *bstream, u8 ebits);

        inline i16 fill_xac(bitio::bitio_stream *bstream, YAJLEntropyTable *table);

    public:
        u8 component_id{};
        i16 xcof[0x40] = {0};

        MCU() {
            // empty-constructor.
        }

        MCU(bitio::bitio_stream *bstream, YAJLTables *tables, YAJLScanHeaderSpec *spec);

    };

    YAJLTables tables;
    YAJLScanHeader header;
    std::vector<MCU> mcus;

    YAJLScan() {
        // empty-constructor
    }

    YAJLScan(YAJLMiscData *misc, YAJLFrameHeader *fheader, bitio::bitio_stream *bstream);
};

struct YAJLFrame {
    YAJLTables tables;
    YAJLFrameHeader header;
    std::vector<YAJLScan> scans;

    YAJLFrame() {
        // empty-constructor
    }

    YAJLFrame(YAJLMiscData *misc, bitio::bitio_stream *bstream);
};


#endif
