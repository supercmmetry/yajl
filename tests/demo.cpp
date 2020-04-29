#include <string>
#include <bitio/bitio.h>
#include <chrono>
#include <yajl.h>
#include "test_jpeg.h"

int main() {
    std::string filename = "/home/supercmmetry/Pictures/supercmmetry.jpg";
    bitio::bitio_stream bstream(filename, bitio::READ);

    YAJLImage image(false);
    image.set_src(filename);
    image.scan_markers();
    std::cout << std::endl;
    return 0;
}
