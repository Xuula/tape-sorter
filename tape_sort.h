#ifndef __TAPE_SORT_H__
#define __TAPE_SORT_H__

#include "tape.h"
#include "config.h"

#include <vector>

class TapeSort{
public:
    template<typename T>
    static void sort(Tape::Tape<T> *in, Tape::Tape<T> *out, const std::vector<Tape::Tape<T>*> &auxillary_tapes);
};

#include "tape_sort.hpp"

#endif