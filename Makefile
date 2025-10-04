CXX = g++-15
CXXFLAGS = -std=c++17 -O3

SRCS = main.cpp Board.cpp
HEADERS = Board.h Constants.h

TARGET = chess_engine

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
