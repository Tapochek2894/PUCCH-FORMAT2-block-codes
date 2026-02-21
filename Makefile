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
	rm -rf $(TARGET) build results

run: $(TARGET)
	./$(TARGET) $(filter-out $@,$(MAKECMDGOALS))

test: $(TARGET)
	@cd tests/integration && ./run_all_tests.sh

snr-sweep: $(TARGET)
	@chmod +x scripts/snr_sweep.sh scripts/plot_full_sweep.py
	@./scripts/snr_sweep.sh $(filter-out $@,$(MAKECMDGOALS))

snr-sweep-fast: $(TARGET)
	@chmod +x scripts/snr_sweep.sh scripts/plot_full_sweep.py
	@./scripts/snr_sweep.sh 100 -10 4 4

snr-sweep-no-plot: $(TARGET)
	@chmod +x scripts/snr_sweep.sh
	@./scripts/snr_sweep.sh 20000 -10 4 2 1

%:
	@:

help:
	@echo "Usage:"
	@echo "  make              	 — сборка"
	@echo "  make run            — запуск"
	@echo "  make test           — тесты"
	@echo "  make snr-sweep-fast — быстрая симуляция (100 итераций)"
	@echo "  make snr-sweep      — полная симуляция SNR (20000 итераций)"
	@echo "  make snr-no-plot    — полная симуляция без построения графиков"
	@echo "  make clean          — очистка"
