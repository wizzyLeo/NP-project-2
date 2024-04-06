# Compiler settings
CXX = g++
CXXFLAGS = -Wall -g -std=c++17

# Executable name
TARGET = npshell

# Source files
SRCS = npshell.cpp Parser.cpp IOManager.cpp Shell.cpp CommandExecutor.cpp

HEADERS = Parser.h IOManager.h Shell.h CommandExecutor.h

# Object files (replace .cpp with .o)
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Rule for linking the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule for compiling each source file to an object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# Clean target
clean:
	rm -f $(TARGET) $(OBJS)

# Copy target
copy: all
	mkdir -p ../work_dir
	cp $(SRCS) $(HEADERS) $(TARGET) Makefile ../test/master/110550140

# Phony targets
.PHONY: all clean copy
