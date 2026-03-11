#include "demodulator.hpp"
#include "modulator.hpp"
#include <cmath>
#include <gtest/gtest.h>

constexpr double EPSILON = 1e-4;

TEST(DemodulatorTest, LlrSignConvention) {
    pucch_f2::QpskModulator modulator;
    pucch_f2::QpskDemodulator demodulator;

    auto symbols = modulator.Modulate({0, 0});
    double snr_db = 100.0;

    auto demodulated = demodulator.Demodulate(symbols, snr_db);

    EXPECT_GT(demodulated[0], 0);
    EXPECT_GT(demodulated[1], 0);
}

TEST(DemodulatorTest, LlrMagnitude) {
    pucch_f2::QpskDemodulator demodulator;

    double snr_db = 100.0;

    std::complex<double> symbol(-1.0 / std::sqrt(2.0), 0);

    auto demodulated = demodulator.Demodulate({symbol}, snr_db);

    EXPECT_LT(demodulated[0], 0);
}

TEST(DemodulatorTest, DemodulateVector) {
    pucch_f2::QpskModulator modulator;
    pucch_f2::QpskDemodulator demodulator;

    double snr_db = 100.0;

    std::vector<uint8_t> bits = {0, 1, 1, 0, 0, 0};
    auto symbols = modulator.Modulate(bits);

    auto llrs = demodulator.Demodulate(symbols, snr_db);

    EXPECT_EQ(llrs.size(), bits.size());

    EXPECT_GT(llrs[0], 0);
    EXPECT_LT(llrs[1], 0);
}