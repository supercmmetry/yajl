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