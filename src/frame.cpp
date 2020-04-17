#include "frame.h"

YAJLFrameHeaderData::YAJLFrameHeaderData(uint16_t marker, bitio::bitio_stream *bstream) {
    sof = marker;
    precision = bstream->read(0x8);
    nlines = bstream->read(0x10);
    nsamples_per_line = bstream->read(0x10);
    ncomponents = bstream->read(0x8);
}

YAJLFrameHeaderSpec::YAJLFrameHeaderSpec(bitio::bitio_stream *bstream) {
    component_id = bstream->read(0x8);
    horizontal_sampling_factor = bstream->read(0x4);
    vertical_sampling_factor = bstream->read(0x4);
    qtable_dest = bstream->read(0x8);
}

YAJLScanHeaderSpec::YAJLScanHeaderSpec(bitio::bitio_stream *bstream) {
    component_selector = bstream->read(0x8);
    dc_selector = bstream->read(0x4);
    ac_selector = bstream->read(0x4);
}

YAJLScanHeader::YAJLScanHeader(bitio::bitio_stream *bstream) {
    uint16_t length = bstream->read(0x10);
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
