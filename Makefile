CXX = g++
INC_DIR = include
CXXFLAGS = -std=c++14 -I $(INC_DIR) 
LIB = 
PROG = virtualmem

SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)
DEP := $(OBJ:.o=.d)

# preprocessor flags
CPPFLAGS := -MMD -MP 

$(PROG): $(OBJ)
	$(CXX) -o $@ $^ $(LIB)
    
-include $(DEP)

clean:
	$(RM) $(PROG) $(OBJ) $(DEP)
