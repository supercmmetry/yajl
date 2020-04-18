#include <string>
#include <bitio/bitio.h>
#include <yajl.h>

int main() {
    std::string filename = "/home/supercmmetry/Pictures/supercmmetry.jpg";
    YAJLImage image(false);
    image.set_src(filename);
    image.scan_markers();

    return 0;
}
