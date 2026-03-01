#include "modulator.hpp"
#include <cmath>
#include <gtest/gtest.h>

constexpr double EPSILON = 1e-4;

TEST(ModulatorTest, EvenBitsRequired) {
    pucch_f2::QpskModulator modulator;

    EXPECT_THROW(modulator.Modulate({0}), std::invalid_argument);
    EXPECT_THROW(modulator.Modulate({0, 0, 0}), std::invalid_argument);

    EXPECT_NO_THROW(modulator.Modulate({0, 0}));
    EXPECT_NO_THROW(modulator.Modulate({0, 0, 0, 0}));
}

TEST(ModulatorTest, QpskMapping) {
    pucch_f2::QpskModulator modulator;

    auto symbols = modulator.Modulate({0, 0});
    EXPECT_NEAR(symbols[0].real(), 1.0 / std::sqrt(2.0), EPSILON);
    EXPECT_NEAR(symbols[0].imag(), 1.0 / std::sqrt(2.0), EPSILON);

    symbols = modulator.Modulate({1, 1});
    EXPECT_NEAR(symbols[0].real(), -1.0 / std::sqrt(2.0), EPSILON);
    EXPECT_NEAR(symbols[0].imag(), -1.0 / std::sqrt(2.0), EPSILON);

    symbols = modulator.Modulate({0, 1});
    EXPECT_NEAR(symbols[0].real(), 1.0 / std::sqrt(2.0), EPSILON);
    EXPECT_NEAR(symbols[0].imag(), -1.0 / std::sqrt(2.0), EPSILON);
}

TEST(ModulatorTest, OutputCount) {
    pucch_f2::QpskModulator modulator;

    std::vector<uint8_t> bits(20, 0);
    auto symbols = modulator.Modulate(bits);

    EXPECT_EQ(symbols.size(), 10);
}

TEST(ModulatorTest, UnitPower) {
    pucch_f2::QpskModulator modulator;

    for (uint8_t b0 : {0, 1}) {
        for (uint8_t b1 : {0, 1}) {
            auto symbols = modulator.Modulate({b0, b1});
            double power = std::norm(symbols[0]);
            EXPECT_NEAR(power, 1.0, EPSILON);
        }
    }
}