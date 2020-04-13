#include <string>
#include <bitio/bitio.h>
#include <yajl.h>

int main() {
    std::string filename = "/home/supercmmetry/Pictures/supercmmetry.jpg";
    YAJLImage image;
    image.set_src(filename);
    image.scan_markers();
    std::cout << image.app_data << std::endl;

    return 0;
}
