#ifndef YAJL_IMAGE_H
#define YAJL_IMAGE_H

#include <string>
#include <bitio/bitio.h>
#include "qtable.h"


class YAJLImageDescriptor {
private:
    std::vector<char*> ds_app_data;
    std::vector<char*> ds_comments;
    YAJLQTable tables[4];
protected:
    void push_app_data(char* app_data);
    void push_com_data(char* com_data);
    void add_qtable(YAJLQTable table, int index);
public:
    char* get_app_data(uint32_t index = 0);
    char* get_comment(uint32_t index = 0);
    YAJLQTable get_qtable(uint8_t index);
};


class YAJLImage : public YAJLImageDescriptor {
    bitio::bitio_stream *bstream;
    bool use_minimal_scan;
public:
    YAJLImage(bool _use_minimal_scan = true);
    void set_src(std::string filename);
    void scan_markers();
};


#endif