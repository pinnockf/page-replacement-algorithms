#include <iostream>
#include <bitset>
#include <vector>
#include <functions.h>


auto print_vector(std::vector<Frame> &frame_vector) -> void{
  for(auto& frame: frame_vector){
    std::cout << frame.page << " " << frame.bits.to_string() << std::endl;
    
  }
  std::cout << "----------------\n";
}

auto shift_bits(std::vector<Frame> &frame_vector) -> void{
   for(auto& frame: frame_vector){
      frame.bits >>= 1;
   }

}