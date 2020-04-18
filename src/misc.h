#ifndef YAJL_MISC_H
#define YAJL_MISC_H

#include <vector>
#include <bitio/bitio.h>
#include "qtable.h"
#include "entropy_tables.h"
#include "types.h"

struct YAJLMiscData {
    bool ignore;
    std::vector<char*> app_data;
    std::vector<char*> comments;

    void fetch_app_data(bitio::bitio_stream *bstream);
    void fetch_comment(bitio::bitio_stream *bstream);
};

struct YAJLTables {
    YAJLQTable qtables[4];
    YAJLEntropyTable etables[4];

    void fetch_qtable(bitio::bitio_stream *bstream);
    void fetch_etable(u16 marker, bitio::bitio_stream *bstream);
};


#endif
