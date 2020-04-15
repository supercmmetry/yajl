#ifndef YAJL_FRAME_H
#define YAJL_FRAME_H

#include <cstdint>
#include <vector>

struct YAJLFrameHeaderSpec {
    uint8_t component_id;
    uint8_t horizontal_sampling_factor;
    uint8_t vertical_sampling_factor;
    uint8_t qtable_dest;
};

struct YAJLFrameHeaderData {
    uint16_t sof;
    uint8_t precision;
    uint16_t nlines;
    uint16_t nsamples_per_line;
    uint8_t ncomponents;
};

class YAJLFrameHeader {
public:
    YAJLFrameHeaderData frame_header_data;
    std::vector<YAJLFrameHeaderSpec> frame_header_specs;

};

#endif
