
// CSE421 Summer 2016           Team name: MikeJoelFrank
// University at Buffalo        July 8, 2016 
// 
//                    Team Members:
//
// Name:               Email:                   UB#
// Franklin Pinnock    fgpinnoc@buffalo.edu     36398302
// Joel Little         joellitt@buffalo.edu     36183498          
//-------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <map>
#include <functional>
#include <chrono>
#include <policy.h>
#include <exception>

auto debug_output(bool, std::string) -> void;
auto get_ref_sequence(bool, std::string) -> std::string;
auto show_help() -> void;
auto show_program_info(bool, int, std::string, std::string) -> void;
auto create_policy_mapping() -> std::map<std::string, std::function<int (std::string)>>;

int main(int argc, char* argv[]){   
   
    bool debug_flag = true;
    bool input_file_was_given = false;
    std::string ref_string;

    //Set default frames, replacement policy, and input file.
    extern int available_frames;
    available_frames = 5;
    std::string replacement_policy = "FIFO";
    std::string input_file = "STDIN";

    char option;
    while((option = getopt(argc,argv,"hf:r:i:"))!=-1){
      switch(option){
        case 'h':
          show_help();
          return 0;
          break;
        case 'f':
          available_frames = std::stoi(optarg, nullptr, 10);
          break;
        case 'r':
          replacement_policy = optarg;
          break;
        case 'i':   
          input_file_was_given = true;
          input_file = optarg;
          break;
        default:
        break;
      }
    }   
    
    ref_string = get_ref_sequence(input_file_was_given, input_file); 

    //show_program_info(debug_flag, available_frames, replacement_policy, input_file);     
    
    //Run function associated with replacement policy
    std::map<std::string, std::function<int (std::string)>> policy_map;
    policy_map = create_policy_mapping();  
    auto& do_policy = policy_map[replacement_policy];
    
    //Calculate elapsed time of replacement policy algorithm
    auto policy_start_time = std::chrono::high_resolution_clock::now();
    int policy_page_replacements = do_policy(ref_string);
    auto policy_finish_time = std::chrono::high_resolution_clock::now();
    auto policy_duration = std::chrono::duration_cast<std::chrono::milliseconds>(policy_finish_time - policy_start_time);
    int policy_duration_ms = policy_duration.count(); //Elapsed time of selected policy algorithm
    
    //Calculate elapsed time for optional algorithm  
    auto optional_start_time = std::chrono::high_resolution_clock::now();
    int optional_page_replacements = do_optional(ref_string);
    auto optional_finish_time = std::chrono::high_resolution_clock::now();
    auto optional_duration = std::chrono::duration_cast<std::chrono::milliseconds>(optional_finish_time - optional_start_time);
    int optional_duration_ms = optional_duration.count(); //Elapsed time of optional algorithm
    
    std::cout << "# of page replacements with " << replacement_policy << "\t: " << policy_page_replacements << std::endl;
    std::cout << "# of page replacements with Optimal\t: " << optional_page_replacements << std::endl;
    
    double replacement_penalty = (((double)policy_page_replacements / (double)optional_page_replacements) - 1) * 100;
    std::cout << "% page replacement penalty using LFU\t: " << replacement_penalty << "%\n\n";
    
    std::cout << "Total time to run " << replacement_policy << " algorithm\t: " << policy_duration_ms << " msec\n";
    std::cout << "Total time to run Optimal algorithm\t :" << optional_duration_ms << " msec\n";
  
    try {
        if (optional_duration_ms == 0){
          throw std::overflow_error("Divide by zero error");
        }
        double effective_rate = (optional_duration_ms - policy_duration_ms) / optional_duration_ms;
        std::cout << replacement_policy << "is " << effective_rate << "% faster than Optimal algorithm.\n";
    }
    catch (std::overflow_error e){
      std::cout << "Could not compare performance. Try a larger reference sequence." << std::endl;
    }
    return 0;
}

auto create_policy_mapping() -> std::map<std::string, std::function<int (std::string)>>{

  std::map<std::string, std::function<int (std::string)>> policy_map;
  
  policy_map["FIFO"]      = &fifo; 
  policy_map["LFU"]       = &lfu;
  policy_map["LRU-STACK"] = &lru_stack;
  policy_map["LRU-CLOCK"] = &lru_clock;
  policy_map["LRU-REF8"]  = &lru_ref8;
  
  return policy_map;
}

auto debug_output(bool debug_flag, std::string message) -> void{
  
  if(debug_flag = true){
    std::cout << message << std::endl;
  }
  
}

auto show_help() -> void {

  std::cout << "\nSynopsis: virtualmem [-h] [-f available-frames] [-r replacement policy] [-i input_file]" << std::endl; 
  std::cout << "\n\t-f: Set the number of available frames. By default it should be 5.\n" << std::endl;
  std::cout << "\t-r Set the page replacement policy. It can be either:\n\t\t FIFO (First-in-first-out)" << std::endl;
  std::cout << "\t\t LFU (Least-frequently-used)\n\t\t LRU-STACK (Least-recently-used stack implementation)" << std::endl;
  std::cout << "\t\t LRU-CLOCK (Least-recently-used clock implementation second chance algorithm)." << std::endl;
  std::cout << "\t\t LRU-REF8 (Least-recently-used Reference-bit implementation, using 8 reference bits)" << std::endl;
  std::cout << "\t\t The default will be FIFO\n" << std::endl;
  std::cout << "\t-i: Read the page reference sequence from a specified file. If not given, the sequence" << std::endl;
  std::cout << "\t    should be read from STDIN (ended with ENTER)" << std::endl;     
}

 // Read reference sequence from input file or STDIN  
auto get_ref_sequence(bool input_file_was_given, std::string input_file) -> std::string {
  
  bool file_read_error = false;
  std::string ref_string;
      
  if (input_file_was_given == true){
      std::ifstream file (input_file);

      if (file.is_open()){
        getline(file, ref_string);
        file.close();
      }
      else {
        file_read_error = true;
      }
  } 
  
  if (input_file_was_given == false || file_read_error == true){
    std::cout << "Please type a reference sequence and press ENTER" << std::endl;
    std::cin >> ref_string;  
  }
  //std::cout << "Reference sequence is: " << ref_string << std::endl;    

  return ref_string;
}

auto show_program_info(bool debug_flag, int available_frames, std::string replacement_policy, std::string input_file) -> void{

  if(debug_flag == true){
  
    std::cout << "\n===========\n";
    std::cout << "Virtual Mem\n";  
    std::cout << "-----------\n";
   
    std::cout << "Available frames: " << available_frames << std::endl;
    std::cout << "Replacement policy: " << replacement_policy << std::endl;
    std::cout << "Input file: " << input_file << std::endl;
    std::cout << "-----------\n";
  }
}

