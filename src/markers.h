#ifndef YAJL_MARKERS_H
#define YAJL_MARKERS_H

#include <cstdint>

namespace markers {
    inline bool is_marker(uint16_t val) {
        return (val >> 8) == 0xFF;
    }

    inline bool is_app_marker(uint16_t val) {
        return (val >> 4) == 0xFFE;
    }

    inline bool is_sof_marker(uint16_t val) {
        return ((val >> 4) == 0xFFC) && (val != 0xFFC8);
    }


    enum MARKER {
        SOF0 = 0xFFC0,  // Baseline DCT (Huffman)
        SOF1 = 0xFFC1,  // Extended sequential DCT (Huffman)
        SOF2 = 0xFFC2,  // Progressive DCT (Huffman)
        SOF3 = 0xFFC3,  // Lossless (sequential) (Huffman)
        SOF5 = 0xFFC5,  // Differential sequential DCT (Huffman)
        SOF6 = 0xFFC6,  // Differential progressive DCT (Huffman)
        SOF7 = 0xFFC7,  // Differential lossless (sequential) (Huffman)
        JPG = 0xFFC8,   // Reserved for JPEG extensions (Arithmetic)
        SOF9 = 0xFFC9,  // Extended sequential DCT (Arithmetic)
        SOF10 = 0xFFCA, // Progressive DCT (Arithmetic)
        SOF11 = 0xFFCB, // Lossless (sequential) (Arithmetic)
        SOF13 = 0xFFCD, // Differential sequential DCT (Arithmetic)
        SOF14 = 0xFFCE, // Differential progressive DCT (Arithmetic)
        SOF15 = 0xFFCF, // Differential lossless (sequential) (Arithmetic)
        DHT = 0xFFC4,   // Define Huffman table(s)
        DAC = 0xFFCC,   // Define arithmetic coding conditioning(s)
        RST0 = 0xFFD0,  // Restart with modulo 8 count “m”
        RST1 = 0xFFD1,
        RST2 = 0xFFD2,
        RST3 = 0xFFD3,
        RST4 = 0xFFD4,
        RST5 = 0xFFD5,
        RST6 = 0xFFD6,
        RST7 = 0xFFD7,
        SOI = 0xFFD8,   // Start of image
        EOI = 0xFFD9,   // End of image
        SOS = 0xFFDA,   // Start of scan
        DQT = 0xFFDB,   // Define quantization table(s)
        DNL = 0xFFDC,   // Define number of lines
        DRI = 0xFFDD,   // Define restart interval
        DHP = 0xFFDE,   // Define hierarchical progression
        EXP = 0xFFDF,   // Expand reference component(s)
        APP0 = 0xFFE0,  // Reserved for application segments
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
        JPG0 = 0xFFF0,  // Reserved for JPEG extensions
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
        COM = 0xFFFE, // Comment
        TEM = 0xFF01, // For temporary private use in arithmetic coding
    };
}

#endif
