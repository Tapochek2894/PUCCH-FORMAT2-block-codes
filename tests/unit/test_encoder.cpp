#include "encoder.hpp"
#include <gtest/gtest.h>

TEST(EncoderTest, ValidCodeLengths) {
    for (int len : pucch_f2::kValidCodeLengths) {
        EXPECT_NO_THROW(pucch_f2::Encoder encoder(len));
    }
}

TEST(EncoderTest, InvalidCodeLength) {
    EXPECT_THROW(pucch_f2::Encoder(1), std::invalid_argument);
    EXPECT_THROW(pucch_f2::Encoder(3), std::invalid_argument);
    EXPECT_THROW(pucch_f2::Encoder(5), std::invalid_argument);
    EXPECT_THROW(pucch_f2::Encoder(12), std::invalid_argument);
}

TEST(EncoderTest, EncodeZeroBits) {
    pucch_f2::Encoder encoder(2);
    auto codeword = encoder.Encode({0, 0});

    EXPECT_EQ(codeword.size(), pucch_f2::kCodewordLength);
    for (uint8_t bit : codeword) {
        EXPECT_EQ(bit, 0);
    }
}

TEST(EncoderTest, EncodeDataSizeMismatch) {
    pucch_f2::Encoder encoder(4);
    EXPECT_THROW(encoder.Encode({0, 0, 0}), std::invalid_argument);
    EXPECT_THROW(encoder.Encode({0, 0, 0, 0, 0}), std::invalid_argument);
}

TEST(EncoderTest, EncodeAllOnes) {
    pucch_f2::Encoder encoder(2);
    auto codeword = encoder.Encode({1, 1});

    EXPECT_EQ(codeword.size(), pucch_f2::kCodewordLength);
    for (uint8_t bit : codeword) {
        EXPECT_TRUE(bit == 0 || bit == 1);
    }
}

TEST(EncoderTest, DeterministicEncoding) {
    pucch_f2::Encoder encoder(4);
    std::vector<uint8_t> data = {1, 0, 1, 0};

    auto cw1 = encoder.Encode(data);
    auto cw2 = encoder.Encode(data);

    EXPECT_EQ(cw1, cw2);
}