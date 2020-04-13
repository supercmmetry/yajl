#include "image.h"
#include "markers.h"


void YAJLImage::set_src(std::string filename) {
    bstream = new bitio::bitio_stream(filename, bitio::READ);
}

void YAJLImage::scan_markers() {
    // First check for SOI marker
    if (bstream->read(0x10) != MARKERS::SOI) {
        std::cout << "SOI not found" << std::endl;
        return;
    }
    while (true) {
        uint16_t marker = bstream->read(0x10);
        switch (marker) {
            case MARKERS::APP0: {
                if (!use_minimal_scan) {
                    // param-length followed by application data.
                    uint16_t length = bstream->read(0x10);
                    auto *app_data = new char[length];
                    for (int i = 0; i < length; i++) {
                        app_data[i] = bstream->read(0x8);
                    }
                    push_app_data(app_data);
                } else {
                    uint16_t length = bstream->read(0x10);
                    for (int i = 0; i < length; i++) {
                        bstream->read(0x8);
                    }
                }
                break;
            }
            case MARKERS::COM: {
                if (!use_minimal_scan) {
                    // param-length followed by comment-bytes.
                    uint16_t length = bstream->read(0x10);
                    auto *com_data = new char[length];
                    for (int i = 0; i < length; i++) {
                        com_data[i] = bstream->read(0x8);
                    }
                    push_com_data(com_data);
                } else {
                    uint16_t length = bstream->read(0x10);
                    for (int i = 0; i < length; i++) {
                        bstream->read(0x8);
                    }
                }
                break;
            }

        }
        break;
    }

}

YAJLImage::YAJLImage(bool _use_minimal_scan) {
    bstream = nullptr;
    use_minimal_scan = _use_minimal_scan;
}


char *YAJLImageDescriptor::get_app_data(uint32_t index) {
    if (index < ds_app_data.size()) {
        return ds_app_data[index];
    } else {
        return nullptr;
    }
}

void YAJLImageDescriptor::push_app_data(char *app_data) {
    ds_app_data.push_back(app_data);
}

void YAJLImageDescriptor::push_com_data(char *com_data) {
    ds_comments.push_back(com_data);
}

char *YAJLImageDescriptor::get_com_data(uint32_t index) {
    if (index < ds_comments.size()) {
        return ds_comments[index];
    } else {
        return nullptr;
    }
}
