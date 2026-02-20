CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Werror -Wextra -Wpedantic -Iinclude

TARGET = pucch_codes_modeling
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:src/%.cpp=build/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) build

run: $(TARGET)
	./$(TARGET) $(filter-out $@,$(MAKECMDGOALS))

%:
	@:

help:
	@echo "Использование:"
	@echo "  make — сборка"
	@echo "  make run — запуск"
	@echo "  make clean — очистка"