#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>

struct Frame {
      char page = 'E';
      std::bitset<8> bits;
    };

auto print_vector(std::vector<Frame> &frame_vector) -> void;
auto shift_bits(std::vector<Frame> &frame_vector) -> void;

#endif
