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