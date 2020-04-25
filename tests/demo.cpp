#include <string>
#include <bitio/bitio.h>
#include <yajl.h>
#include "test_jpeg.h"

int main() {
    std::string filename = "/home/supercmmetry/Pictures/supercmmetry.jpg";
    bitio::bitio_stream bstream(filename, bitio::READ, 1);


    std::cout << bstream.read(0x10) << std::endl;

    bstream.seek(0x1);
    bstream.seek(-0x8);
    bstream.seek(0x8);
    bstream.seek(0x123);
    bstream.seek(-0x123);
    bstream.seek(-0x11);



    std::cout << bstream.read(0x10) << std::endl;

//    YAJLImage image(false);
//    image.set_src(filename);
//    image.scan_markers();
//    std::cout << std::endl;
//    std::cout << "Actual DCT-coefficients: " << std::endl;
//    auto cofs = JPEGCodec(filename).read_coefficients();
//    // print dc-coeffs
//
//    for (int i = 0; i < 64; i++) {
//        if (i % 8 == 0) {
//            std::cout << std::endl;
//        }
//        std::cout << cofs.coeff_arrays[1][i][0][0] << " ";
//    }
    return 0;
}
