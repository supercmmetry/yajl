#ifndef YAJL_DESCRIPTORS_H
#define YAJL_DESCRIPTORS_H

#include <vector>
#include "types.h"
#include "frame.h"
#include "misc.h"

class YAJLImageDescriptor {
private:

protected:
    YAJLMiscData misc;
    std::vector<YAJLFrame> frames;
};
#endif
