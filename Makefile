# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++17 -O3

# Source and Header Files
SRCS = main.cpp Board.cpp
HEADERS = Board.h Constants.h

# Output Executable
TARGET = chess_engine

# Build Target
$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# Clean Build Artifacts
clean:
	rm -f $(TARGET)

# Run the Program
run: $(TARGET)
	./$(TARGET)
