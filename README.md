# PUCCH-FORMAT2-block-codes

Моделирование работы блочных кодов в **PUCCH FORMAT2** (Physical Uplink Control Channel). Включает кодирование, модуляцию QPSK, передачу через канал AWGN, демодуляцию и декодирование

---

## 📋 Требования

| Компонент | Версия |
| ----------- | -------- |
| Компилятор | g++ ≥ 11.4 или clang++ ≥ 14 |
| Стандарт C++ | C++17 |
| Библиотеки | STL, GTest, nlohmann/json |

---

## 🚀 Быстрый старт

```bash
# 1. Сборка проекта
make

# 2. Запуск с входным JSON-файлом
make run input.json

# 3. Запуск интеграционных и unit-тестов
make test

# 4. Полное моделирование BLER vs SNR (10000 итераций, все длины кода)
make snr-modeling

# 5. Полное моделирование BLER vs SNR без построения графиков
make snr-modeling-no-plot

# 6. Быстрый тест моделирования (100 итераций, шаг 4дБ)
make snr-modeling-fast

# 7. Очистка артефактов сборки
make clean
```

---

## 📁 Структура проекта

``` bash
PUCCH-FORMAT2-block-codes/
├── include/                  # Заголовочные файлы библиотеки
│   ├── channel.hpp
│   ├── decoder.hpp
│   ├── demodulator.hpp
│   ├── encoder.hpp
│   ├── modulator.hpp
├── src/                      # Исходный код
│   ├── channel.cpp
│   ├── decoder.cpp
│   ├── demodulator.cpp
│   ├── encoder.cpp
│   ├── main.cpp              # Точка входа + CLI логика
│   └── modulator.cpp
├── tests/                    # Тесты
│   ├── integration/          # Интеграционные тесты (JSON-сценарии)
│   │   ├── *.json
│   │   └── run_all_tests.sh
│   └── unit/                 # Юнит-тесты (GTest)
│       ├── test_*.cpp
│       └── Makefile
├── scripts/                  # Автоматизация
│   ├── plot_full_modeling.py # Построение графиков BLER
│   └── snr_modeling.sh       # Скрипт моделирования
├── build/                    # Артефакты компиляции (игнорируется в Git)
├── results/                  # Результаты симуляций (игнорируется в Git)
├── Makefile                  # Основная система сборки
├── LICENSE                   # Лицензия MIT
└── README.md
```

---

## 🔧 Режимы работы

Программа управляется через JSON-вход. Поле **mode** определяет выполняемое действие

### 1. Кодирование

Кодирует информационные биты в кодовое слово (20 бит) и модулирует в QPSK-символы

**Вход:**

```json
{
    "mode": "coding",
    "num_of_pucch_f2_bits": 11,
    "pucch_f2_bits": [1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0]
}
```

**Выход:**

```json
{
    "mode": "coding",
    "num_of_pucch_f2_bits": 11,
    "qpsk_symbols": ["0.707+0.707j", "-0.707+0.707j", ...]
}
```

---

### 2. Декодирование

Принимает вектор комплексных символов, вычисляет LLR и восстанавливает информационные биты.

**Вход:**

```json
{
    "mode": "decoding",
    "num_of_pucch_f2_bits": 11,
    "qpsk_symbols": ["0.707+0.707j", "-0.707+0.707j", ...]
}
```

**Выход:**

```json
{
    "mode": "decoding",
    "num_of_pucch_f2_bits": 11,
    "pucch_f2_bits": [1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0]
}
```

---

### 3. Симуляция работы системы

Симуляция с помощью  метода Монте-Карло при фиксированном SNR для вычисления BLER

**Вход:**

```json
{
    "mode": "channel simulation",
    "num_of_pucch_f2_bits": 11,
    "snr_db": 5.0,
    "iterations": 1000
}
```

**Выход:**

```json
{
    "mode": "channel simulation",
    "num_of_pucch_f2_bits": 11,
    "snr_db": 5.0,
    "iterations": 1000,
    "bler": 0.1500,
    "success": 850,
    "failed": 150
}
```

---

### 4. Моделирование

Автоматический прогон симуляции для всех длин кода {2, 4, 6, 8, 11} в диапазоне SNR. Запускается через make **snr-modeling**

Результат: Файл **results/full_snr_modeling.json** с метаданными и массивом результатов, а также график **results/full_snr_modeling.png**

**Вход:**

```json
{
    "mode": "channel simulation",
    "num_of_pucch_f2_bits": 11,
    "snr_db": 5.0,
    "iterations": 1000
}
```

**Результат:**

`results/full_snr_modeling.json` — массив результатов с метаданными
`results/full_snr_modeling.png` — график BLER vs SNR

Пример вывода JSON:

```json
{
  "metadata": {
    "code_lengths": [2,4,6,8,11],
    "snr_range": {"start": -10, "end": 4, "step": 2},
    "iterations": 20000,
    "timestamp": "2026-02-21T23:16:50+03:00"
  },
  "results": [
    {
        "bler": 0.3464,
        "failed": 6928,
        "iterations": 20000,
        "mode": "channel simulation",
        "num_of_pucch_f2_bits": 2,
        "snr_db": -10.0,
        "success": 13072
    }
    ,
    ...
  ]
}
```

---

## 🛠 Сборка

| Команда | Описание |
| --------- | ---------- |
| `make` | Сборка release-версии |
| `make clean` | Очистка артефактов сборки |
| `make run [args]` | Запуск программы с аргументами |
| `make test` | Запуск unit-тестов и интеграционных тестов |
| `make unit-test` | Запуск только unit-тестов |
| `make integrarion-test` | Запуск только интеграционных тестов |
| `make snr-modeling [iters] [start] [end] [step]` | Полное моделирование (по умолчанию: 10000 итераций, -10...6 дБ) |
| `make help` | Показать справку |

---

## 📐 Технические детали

### Кодирование

- **Алгоритм:** Код Рида-Маллера (20, n) из 3GPP TS 38.212
- **Длины кода:** 2, 4, 6, 8, 11 бит
- **Кодовое слово:** Фиксированная длина 20 бит
- **Порождающая матрица:** $A₂₀ₓ₁₃$, усечение слева для n < 13

### Модуляция

- **Тип:** QPSK с кодом Грея
- **Нормализация:** Единичная мощность символа (Es = 1)
- **Маппинг:** `0 → +1/√2`, `1 → -1/√2` на каждой оси

### Канал

- **Тип:** С аддитивным белым гаусовским шумом
- **Шум:** Гауссовский, независимый по синфазной и квадратурной составляющим
- **Формула шума:** `σ = √(1 / (2 × SNR_linear))`

### Демодуляция

- **Тип:** Мягкая (вычисление LLR)
- **Формула:** `LLR = -2√2 × SNR_linear × coordinate`
- **Интерпретация:** `LLR > 0` ⇒ вероятнее бит 1, `LLR < 0` ⇒ вероятнее бит 0

### Декодирование

- **Алгоритм:** Максимум правдоподобия (полный перебор)
- **Метрика:** Скалярное произведение `M = ∑(2c - 1)⋅LLR`

- **Оптимизация:** Предрасчитанная таблица кодовых слов
- **Сложность:** O($2^k$) операций на декодирование

---

## 🧪 Тестирование

Проект использует интеграционные и Unit-тесты

### Интеграционные тесты (`tests/integration/`)

Проверяют работу всей системы через JSON-интерфейс

- **Валидные сценарии:** Проверка корректности выхода для всех режимов

- **Невалидные сценарии:** Проверка обработки ошибок (неверный JSON, невалидные поля)

### Unit-тесты (`tests/unit/`)

Проверяют корректность работы **отдельных классов** с использованием GoogleTest.

- **Encoder/Decoder**: проверка кодирования/декодирования без шума
- **Modulator/Demodulator**: проверка маппинга QPSK и вычисления LLR
- **Channel**: проверка статистики шума AWGN
- **Валидация**: проверка обработки некорректных входных данных

```bash
# Запуск всех тестов
make test

# Запуск только unit-тестов
make unit-test

# Запуск только интеграционных тестов
make integration-test
```

---

## 📊 Визуализация результатов

После запуска `make snr-modeling` автоматически генерируется график `results/full_snr_modeling.png`

*(График строится с помощью `scripts/plot_full_modeling.py`)*

**Ручной запуск построения:**

```bash
python3 scripts/plot_full_modeling.py results/full_snr_modeling.json
```

---

## 📦 Установка зависимостей

```bash
# Библиотеки для построения графиков
sudo apt install python3-matplotlib python3-numpy

# Библиотека GTest
sudo apt apt install libgtest-dev

# Библиотека nlohmann/json
sudo apt apt install nlohmann-json3-dev
```

---

### Стандарт

- **3GPP TS 36.212 version 17.1.0 Release 17** — Multiplexing and channel coding (Section 5.2.3.3)

### Зависимости

- **[nlohmann/json](https://github.com/nlohmann/json)** — JSON for Modern C++ (MIT License)
- **[GoogleTest](https://github.com/google/googletest)** — Testing Framework (BSD-3 License)

### Лицензия

Данный проект распространяется под лицензией **MIT**. См. файл [LICENSE](LICENSE) для деталей
