#include "channel.hpp"
#include "modulator.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <random>

constexpr double EPSILON = 1e-2;

TEST(ChannelTest, AwgnNoiseStatistics) {
    pucch_f2::AwgnChannel channel(100.0, 42);

    std::complex<double> symbol(1.0, 0);
    auto received = channel.Transmit({symbol})[0];

    EXPECT_NEAR(received.real(), symbol.real(), EPSILON);
    EXPECT_NEAR(received.imag(), symbol.imag(), EPSILON);
}

TEST(ChannelTest, AwgnNoiseVariance) {
    pucch_f2::AwgnChannel channel(0.0, 12345);

    std::vector<double> noise_i, noise_q;
    std::complex<double> zero(0, 0);

    for (int i = 0; i < 10000; ++i) {
        auto r = channel.Transmit({zero})[0];
        noise_i.push_back(r.real());
        noise_q.push_back(r.imag());
    }

    auto variance = [](const std::vector<double>& v) {
        double mean = std::accumulate(v.begin(), v.end(), 0.0) / v.size();
        double var = 0;
        for (double x : v)
            var += (x - mean) * (x - mean);
        return var / v.size();
    };

    double expected_var = 0.5;
    EXPECT_NEAR(variance(noise_i), expected_var, 0.1);
    EXPECT_NEAR(variance(noise_q), expected_var, 0.1);
}

TEST(ChannelTest, DeterministicWithSeed) {
    std::complex<double> symbol(0.707, 0.707);

    pucch_f2::AwgnChannel ch1(5.0, 999);
    pucch_f2::AwgnChannel ch2(5.0, 999);

    auto r1 = ch1.Transmit({symbol})[0];
    auto r2 = ch2.Transmit({symbol})[0];

    EXPECT_EQ(r1, r2);
}

TEST(ChannelTest, VectorTransmission) {
    pucch_f2::AwgnChannel channel(10.0, 42);
    pucch_f2::QpskModulator modulator;

    auto symbols = modulator.Modulate({0, 1, 0, 1, 0, 1});
    auto received = channel.Transmit(symbols);

    EXPECT_EQ(received.size(), symbols.size());

    for (size_t i = 0; i < symbols.size(); ++i) {
        EXPECT_NEAR(std::abs(received[i] - symbols[i]), 0, 0.5);
    }
}