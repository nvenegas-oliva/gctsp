CPP = g++
OBJDIR = obj
OBJ = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
TARGET = gctsp
DEFINES  = -DIL_STD -DNDEBUG
CPPFLAGS = -g3 -ggdb -Wall -O2 -pipe -march=native -Wno-deprecated -Wno-sign-compare -fopenmp -lm -std=c++11 -fPIC -fno-strict-aliasing $(DEFINES)
#CPPFLAGS =  -Wall -Wextra -Wno-sign-compare -O2 -pipe -fopenmp -lm -std=c++11
.PHONY: all clean

all: $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir $(OBJDIR)
	cd LKH-2.0.7 && $(MAKE)

$(OBJDIR)/%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CPP) $(CPPFLAGS) -o $@ $^ -LLKH-2.0.7 -llkh

clean:
	@rm -f $(TARGET) $(wildcard *.o)
	@rm -rf $(OBJDIR)
	cd LKH-2.0.7 && $(MAKE) clean
