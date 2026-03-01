#include "demodulator.hpp"
#include "modulator.hpp"
#include <cmath>
#include <gtest/gtest.h>

constexpr double EPSILON = 1e-4;

TEST(DemodulatorTest, LlrSignConvention) {
    pucch_f2::QpskModulator modulator;
    pucch_f2::QpskDemodulator demodulator;

    auto symbols = modulator.Modulate({0, 0});
    double snr_linear = 10.0;

    auto demodulated = demodulator.Demodulate(symbols, snr_linear);

    EXPECT_LT(demodulated[0], 0);
    EXPECT_LT(demodulated[1], 0);
}

TEST(DemodulatorTest, LlrMagnitude) {
    pucch_f2::QpskDemodulator demodulator;

    double snr_db = 10.0 * std::log10(5.0);

    std::complex<double> symbol(1.0 / std::sqrt(2.0), 0);

    auto demodulated = demodulator.Demodulate({symbol}, snr_db);

    double expected = 2.0 * std::sqrt(2.0) * 5.0 * symbol.real();
    EXPECT_NEAR(std::abs(demodulated[0]), std::abs(expected), EPSILON);
}

TEST(DemodulatorTest, DemodulateVector) {
    pucch_f2::QpskModulator modulator;
    pucch_f2::QpskDemodulator demodulator;

    std::vector<uint8_t> bits = {0, 1, 1, 0, 0, 0};
    auto symbols = modulator.Modulate(bits);

    auto llrs = demodulator.Demodulate(symbols, 20.0);

    EXPECT_EQ(llrs.size(), 6);

    EXPECT_LT(llrs[0], 0);
    EXPECT_GT(llrs[1], 0);
}