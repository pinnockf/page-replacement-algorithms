#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <algorithm>
#include <bitset>
#include <functions.h>

int available_frames;

//All functions take in the reference string and use the global value for available_frames. Each function will return the number of
//page replacements. Main takes care of everything else.

auto fifo(std::string ref_string) -> int{

  bool page_found = false;
  int page_replacements = 0; 
  std::deque<int> pagesDeque;                

  for (char& character : ref_string){
    char charVal = character;

    if (charVal == ' '){
      break;           
    } 

    int current_num = charVal - 48;                                           // Convert charVal from ASCII number

    for (int i=0; i < pagesDeque.size(); i++){                                // Check to see if page exists in the page table  
      if (pagesDeque[i] == current_num){ 
        page_found = true;
        break;
      }
    }    
         
    if (pagesDeque.size() < available_frames){  
        if (page_found == false){    
            pagesDeque.push_back(current_num);
        }
    }
    else {       
        if (page_found == false){                                      
            pagesDeque.pop_front();                                           // Remove oldest page, add newest page
            pagesDeque.push_back(current_num);
            page_replacements++;
        }              
    }
    page_found = false;
  }
  
  return page_replacements;
}


auto lfu(std::string ref_string) -> int{
  
  bool page_found       = false;
  int page_replacements = 0; 
  int lfu_page          = 0;
  std::deque<int> pagesDeque;
  std::map<int,int> countMap;
  std::deque<int> keys;


  for (char& character : ref_string){
      char charVal = character;

      if (charVal == ' '){
        break;           
      }  

      int current_num = charVal - 48;    
      
      for (int i=0; i < pagesDeque.size(); i++){                            // Check to see if page exists in the page table  
        if (pagesDeque[i] == current_num){ 
          page_found = true;
          break;
        }
      }

      if (pagesDeque.size() < available_frames){                            // While there are empty frames, fill them
          if (page_found == false){    
            pagesDeque.push_back(current_num);
          }              
      }
      else {                                                                  
          if (page_found == false)                                          // If page is not in page table, replace using LFU algorithm 
          { 
              int lowestCount   = countMap[keys[0]];
              int numDuplicates = 0;
              int lfu_page      = keys[0];

              for (int i=0; i < countMap.size(); i++){
                if (countMap[keys[i]] < lowestCount){
                  lowestCount = countMap[keys[i]];
                  lfu_page = keys[i];
                }
              }

              for (int i=0; i < countMap.size(); i++){
                if (countMap[keys[i]] == lowestCount){
                  numDuplicates++;
                }
              }

              if (numDuplicates > 1){                                       // Use FIFO algorithm to resolve ties conflicts
                  //std::cout << "tie on " << current_num << std::endl;
                  pagesDeque.pop_front();                         
              } 
              else {
                  for (int i=0; i < pagesDeque.size(); i++){
                    if (pagesDeque[i] == lfu_page){
                        pagesDeque.erase (pagesDeque.begin() + i);
                        //std::cout << "ref " << current_num << " removing page " << lfu_page << std::endl;
                        break;
                    }
                  }                                                        
              }
              pagesDeque.push_back(current_num);
              page_replacements++;
          }
      } 

      if (countMap[current_num] == 0){                                      // Add key when a new page id is found
          keys.push_back(current_num);
      } 

      countMap[current_num]++;                                              // Update the page count and reset values for next page
      page_found = false;   
  }   
  return page_replacements;
}


auto lru_stack(std::string ref_string) -> int{

  bool page_found       = false;
  int page_replacements = 0;
  int last_page;
  std::deque<int> pagesDeque;  

  for (char& character : ref_string){
    char charVal = character;

    if (charVal == ' '){
      break;           
    } 

    int current_num = charVal - 48; 
    last_page = current_num;   

    for (int i=0; i < pagesDeque.size(); i++){                            // Check to see if page exists in the page table  
      if (pagesDeque[i] == current_num){ 
        page_found = true;
        break;
      }
    }

    if (page_found = false){
      if (pagesDeque.size() < available_frames){
          pagesDeque.push_front(current_num);  
      }
    }
    else {
       for (int i=0; i < pagesDeque.size(); i++){     
         if (pagesDeque[i] == current_num){
             pagesDeque.erase (pagesDeque.begin() + pagesDeque[i]);     // Remove current page
         }
       }
      
      // Find index of last page and push page of top of it 
      for (int i=0; i < pagesDeque.size(); i++){
         if (pagesDeque[i] == last_page){
             pagesDeque.insert(pagesDeque.begin() + pagesDeque[i], current_num);
         }
       }

       page_replacements++;             
    }

    page_found = false;
  }      
  //std::cout << "deque size = " << pagesDeque.size() << std::endl;
  return page_replacements;
}


auto lru_clock(std::string ref_string) -> int{

 int page_replacements = 0;;

  if(ref_string.size() <= available_frames){
    
    page_replacements = 0;

  }
  else{   

    struct Frame {
      char page = 'E';
      int bit = 0;;
    };

    struct find_page{
      char page;
      find_page(char page) : page(page) {}
      bool operator () ( const Frame& f ) const
      {
          return f.page == page;
      }
    };

    std::vector<Frame> frame_vector(available_frames, Frame());
    
    //Initialize clock_iterator 
    auto clock_iterator = frame_vector.begin(); 

    Frame frame;
    //Loop through pages in reference string
    for(auto string_iterator = ref_string.begin(); string_iterator != ref_string.end(); ++string_iterator){

      frame.page = *string_iterator;
      frame.bit = 1;

      //If page is found, set the reference bit of the corresponding frame to 1;
      auto search = std::find_if( frame_vector.begin(), frame_vector.end(), find_page(frame.page));
      if(search != frame_vector.end()){
        search->bit = 1;
        continue;
      }

      //If all available_frames are occupied, then scan vector until an unset reference bit is found
      auto last_frame = *(--frame_vector.end());
      while(last_frame.page != 'E' && clock_iterator->bit == 1){
        clock_iterator->bit = 0;
        clock_iterator++;
        if(clock_iterator == frame_vector.end()) clock_iterator = frame_vector.begin();
      }    

      //Replace page pointed to by clock_iterator with current page from reference string.
      *clock_iterator = frame;
      
      if(last_frame.page != 'E') page_replacements++;
      clock_iterator++;
      if(clock_iterator == frame_vector.end()) clock_iterator = frame_vector.begin();
    }   
  }

  return page_replacements;

}

auto lru_ref8(std::string ref_string) -> int{

 int page_replacements = 0;;

  if(ref_string.size() <= available_frames){
    
    page_replacements = 0;

  }
  else{   

    

    struct find_page{
      char page;
      find_page(char page) : page(page) {}
      bool operator () ( const Frame& f ) const
      {
          return f.page == page;
      }
    };

    std::vector<Frame> frame_vector(available_frames, Frame());
    
    //Initialize clock_iterator 
    auto clock_iterator = frame_vector.begin(); 

    Frame frame;
    //Loop through pages in reference string
    for(auto string_iterator = ref_string.begin(); string_iterator != ref_string.end(); ++string_iterator){
      auto last_frame = *(--frame_vector.end());

      frame.page = *string_iterator;
      frame.bits[7] = 1;

      if(last_frame.page == 'E'){
        *clock_iterator = frame;   
        clock_iterator++;
        shift_bits(frame_vector);
        continue;
      }   
   
      auto search = std::find_if( frame_vector.begin(), frame_vector.end(), find_page(frame.page));
      if(search != frame_vector.end()){
        //Page found
        search->bits[7] = 1;
        continue;
      }
      else{
        //Page not founnd
        Frame min_frame;

        int min_index;
        unsigned long current_min;
        unsigned long min_bits = std::numeric_limits<unsigned long>::max();        

        //Find index of frame with smallest reference bit value 
        for(int i = 0; i < available_frames; i++){
            min_frame = frame_vector[i];
            current_min = min_frame.bits.to_ulong();
            if(current_min < min_bits){
              min_bits = current_min;
              min_index = i;
          }
        }
        
      frame_vector[min_index] = frame;
      shift_bits(frame_vector);
      page_replacements++;     
      }
    }   
  }

  return page_replacements;
}

auto do_optional(std::string ref_string) -> int{

  bool found_page_match = false;
  bool page_found       = false; 
  int page_index        = 0;
  int page_replacements = 0;
  int matched_page_index;
  int matched_page;
  std::deque<int> pagesDeque;       

  for (char& character : ref_string){
      char charVal = character;

      if (charVal == ' '){
        break;           
      } 

      int current_num = charVal - 48;    

      for (int i=0; i < pagesDeque.size(); i++){                            // Check to see if page exists in the page table  
        if (pagesDeque[i] == current_num){ 
          page_found = true;
          break;
        }
      }

      if (pagesDeque.size() < available_frames){                            // While there are empty frames, fill them
          if (page_found == false){    
            pagesDeque.push_back(current_num);
          }              
      }  
      else 
      {                                     
          if (page_found == false)                                          // If page is not in page table, replace using optimal algorithm
          {                           
            for (int i=page_index; i < ref_string.size(); i++){             // Find last page in reference sequence that is currently in page table
              for (int j=0; j < pagesDeque.size(); j++)
              {
                int current_page = ref_string[i] - 48;
                if (current_page == pagesDeque[j]){
                  found_page_match = true;     
                  matched_page = current_page;
                  matched_page_index = j;
                }
              }
            }  

            if (found_page_match == true){
              pagesDeque.erase (pagesDeque.begin() + matched_page_index);   // Delete last matched Frame in reference string 
            }
            else {
              pagesDeque.pop_front();                                       // When no matched page found use FIFO to remove oldest page in deque
            }  
            pagesDeque.push_back(current_num);
            page_replacements++;                                            // Add newest page to back of deque to maintain FIFO ordering  
          }
      }  
    page_found = false;
    found_page_match = false;  
    page_index++;                
  }

  return page_replacements;    
}
