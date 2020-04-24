#include <string>
#include <bitio/bitio.h>
#include <yajl.h>
#include "test_jpeg.h"
#include <bitio/bitio.h>

int main() {
    std::string filename = "/home/supercmmetry/Pictures/supercmmetry.jpg";

    YAJLImage image(false);
    image.set_src(filename);
    image.scan_markers();
    std::cout << std::endl;
    std::cout << "Actual DCT-coefficients: " << std::endl;
    auto cofs = JPEGCodec(filename).read_coefficients();
    // print dc-coeffs

    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0) {
            std::cout << std::endl;
        }
        std::cout << cofs.coeff_arrays[1][i][0][0] << " ";
    }
    return 0;
}
