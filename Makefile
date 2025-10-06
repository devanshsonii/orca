CXX = g++-15
CXXFLAGS = -std=c++17 -O3

SRCS = main.cpp Board.cpp uci.cpp
HEADERS = Board.h Constants.h uci.h

TARGET = chess_engine

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
