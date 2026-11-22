CXX      = g++
CXXFLAGS = -g -Wall -Wextra -std=c++17
TARGET   = main

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run valgrind clean