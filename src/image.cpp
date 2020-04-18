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
    // parse one frame
    frames.emplace_back(&misc, bstream);
#ifdef DEBUG
    printf("Run completed at: %s for 'Image'\n", __FILE__);
#endif
}


YAJLImage::YAJLImage(bool _use_minimal_scan) {
    bstream = nullptr;
    misc.ignore = _use_minimal_scan;
}