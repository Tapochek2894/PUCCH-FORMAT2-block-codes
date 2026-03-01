CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Werror -Wextra -Wpedantic -Iinclude

TARGET = pucch_codes_modeling.elf
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

unit-test: $(TARGET)
	rm -fr tests/unit/build
	mkdir tests/unit/build
	cp -r $(OBJS) tests/unit/build
	@echo "=== Running Unit Tests ==="
	@cd tests/unit && $(MAKE) run

integration-test: $(TARGET)
	@echo "=== Running Integration Tests ==="
	@cd tests/integration && ./run_integration_tests.sh

test: $(TARGET)
	@$(MAKE) unit-test
	@$(MAKE) integration-test

snr-modeling: $(TARGET)
	@chmod +x scripts/snr_modeling.sh scripts/plot_full_modeling.py
	@./scripts/snr_modeling.sh $(filter-out $@,$(MAKECMDGOALS))

snr-modeling-fast: $(TARGET)
	@chmod +x scripts/snr_modeling.sh scripts/plot_full_modeling.py
	@./scripts/snr_modeling.sh 100 -10 6 4

snr-modeling-no-plot: $(TARGET)
	@chmod +x scripts/snr_modeling.sh
	@./scripts/snr_modeling.sh 10000 -10 6 2 1

help:
	@echo "Usage:"
	@echo "  make              	       — сборка"
	@echo "  make run                  — запуск"
	@echo "  make test                 — тесты"
	@echo "  make snr-smodeling-fast   — быстрая симуляция (100 итераций)"
	@echo "  make snr-modeling         — полная симуляция SNR (10000 итераций)"
	@echo "  make snr-modeling-no-plot — полная симуляция без построения графиков"
	@echo "  make clean                — очистка"
