#ifndef YAJL_MARKERS_H
#define YAJL_MARKERS_H

#include <cstdint>

namespace MARKERS {
    inline bool is_marker(uint16_t val) {
        return (val >> 8) == 0xFF;
    }

    enum MARKER {
        SOF0 = 0xFFC0,
        SOF1 = 0xFFC1,
        SOF2 = 0xFFC2,
        SOF3 = 0xFFC3,
        SOF5 = 0xFFC5,
        SOF6 = 0xFFC6,
        SOF7 = 0xFFC7,
        JPG = 0xFFC8,
        SOF9 = 0xFFC9,
        SOF10 = 0xFFCA,
        SOF11 = 0xFFCB,
        SOF13 = 0xFFCD,
        SOF14 = 0xFFCE,
        SOF15 = 0xFFCF,
        DHT = 0xFFC4,
        DAC = 0xFFCC,
        RST0 = 0xFFD0,
        RST1 = 0xFFD1,
        RST2 = 0xFFD2,
        RST3 = 0xFFD3,
        RST4 = 0xFFD4,
        RST5 = 0xFFD5,
        RST6 = 0xFFD6,
        RST7 = 0xFFD7,
        SOI = 0xFFD8,
        EOI = 0xFFD9,
        SOS = 0xFFDA,
        DQT = 0xFFDB,
        DNL = 0xFFDC,
        DRI = 0xFFDD,
        DHP = 0xFFDE,
        EXP = 0xFFDF,
        APP0 = 0xFFE0,
        APP1 = 0xFFE1,
        APP2 = 0xFFE2,
        APP3 = 0xFFE3,
        APP4 = 0xFFE4,
        APP5 = 0xFFE5,
        APP6 = 0xFFE6,
        APP7 = 0xFFE7,
        APP8 = 0xFFE8,
        APP9 = 0xFFE9,
        APP10 = 0xFFEA,
        APP11 = 0xFFEB,
        APP12 = 0xFFEC,
        APP13 = 0xFFED,
        APP14 = 0xFFEE,
        APP15 = 0xFFEF,
        JPG0 = 0xFFF0,
        JPG1 = 0xFFF1,
        JPG2 = 0xFFF2,
        JPG3 = 0xFFF3,
        JPG4 = 0xFFF4,
        JPG5 = 0xFFF5,
        JPG6 = 0xFFF6,
        JPG7 = 0xFFF7,
        JPG8 = 0xFFF8,
        JPG9 = 0xFFF9,
        JPG10 = 0xFFFA,
        JPG11 = 0xFFFB,
        JPG12 = 0xFFFC,
        JPG13 = 0xFFFD,
        COM = 0xFFFE,
        TEM = 0xFF01,
    };
}

#endif
