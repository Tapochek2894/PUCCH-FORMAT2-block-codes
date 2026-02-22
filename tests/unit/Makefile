CXX ?= g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -I../../include
LDFLAGS = -lgtest -lgtest_main -lpthread

TARGET = test_runner
OBJ_DIR = obj

TEST_SRCS = $(wildcard *.cpp)

SRC_SRCS = ../../src/encoder.cpp \
           ../../src/decoder.cpp \
           ../../src/modulator.cpp \
           ../../src/demodulator.cpp \
           ../../src/channel.cpp

TEST_OBJS = $(TEST_SRCS:%.cpp=$(OBJ_DIR)/%.o)
SRC_OBJS = $(SRC_SRCS:../../src/%.cpp=$(OBJ_DIR)/%.o)
ALL_OBJS = $(TEST_OBJS) $(SRC_OBJS)

.PHONY: all run clean

all: $(TARGET)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJ_DIR) $(ALL_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(ALL_OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: ../../src/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	@echo "Running Unit-tests..."
	@./$(TARGET); \
	EXIT_CODE=$$?; \
	echo ""; \
	echo "Cleaning up object files..."; \
	rm -rf $(OBJ_DIR) $(TARGET); \
	echo "✓ Cleanup complete"; \
	exit $$EXIT_CODE

clean:
	rm -rf $(OBJ_DIR) $(TARGET)