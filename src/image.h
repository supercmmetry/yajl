#ifndef YAJL_IMAGE_H
#define YAJL_IMAGE_H

#include <string>
#include <bitio/bitio.h>
#include "descriptors.h"


class YAJLImage : public YAJLImageDescriptor {
private:
    bitio::bitio_stream *bstream;
    bool use_minimal_scan;
public:
    YAJLImage(bool _use_minimal_scan = true);
    void set_src(std::string filename);
    void scan_markers();
};


#endif