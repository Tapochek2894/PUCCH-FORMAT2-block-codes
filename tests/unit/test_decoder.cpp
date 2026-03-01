#include "decoder.hpp"
#include "demodulator.hpp"
#include "encoder.hpp"
#include "modulator.hpp"
#include <gtest/gtest.h>

TEST(DecoderTest, ValidCodeLengths) {
    for (int len : pucch_f2::kValidCodeLengths) {
        EXPECT_NO_THROW(pucch_f2::Decoder decoder(len));
    }
}

TEST(DecoderTest, InvalidCodeLength) {
    EXPECT_THROW(pucch_f2::Decoder(3), std::invalid_argument);
    EXPECT_THROW(pucch_f2::Decoder(7), std::invalid_argument);
}

TEST(DecoderTest, DecodeNoNoise) {
    for (int code_len : pucch_f2::kValidCodeLengths) {
        pucch_f2::Encoder encoder(code_len);
        pucch_f2::QpskModulator modulator;
        pucch_f2::QpskDemodulator demodulator;
        pucch_f2::Decoder decoder(code_len);

        std::vector<uint8_t> data(code_len);
        for (int i = 0; i < code_len; ++i) {
            data[i] = i % 2;
        }

        auto codeword = encoder.Encode(data);
        auto symbols = modulator.Modulate(codeword);
        auto llr = demodulator.Demodulate(symbols, 20.0);
        auto decoded = decoder.Decode(llr);

        EXPECT_EQ(data, decoded) << "Failed for code length " << code_len;
    }
}

TEST(DecoderTest, LlrSizeMismatch) {
    pucch_f2::Decoder decoder(2);
    EXPECT_THROW(decoder.Decode({1.0, 2.0}), std::invalid_argument);
    EXPECT_THROW(decoder.Decode(std::vector<double>(19, 0.0)), std::invalid_argument);
    EXPECT_THROW(decoder.Decode(std::vector<double>(21, 0.0)), std::invalid_argument);
}

TEST(DecoderTest, DecodeDeterministic) {
    pucch_f2::Encoder encoder(2);
    pucch_f2::QpskModulator modulator;
    pucch_f2::QpskDemodulator demodulator;
    pucch_f2::Decoder decoder(2);

    std::vector<uint8_t> data = {1, 0};
    auto codeword = encoder.Encode(data);
    auto symbols = modulator.Modulate(codeword);
    auto llr = demodulator.Demodulate(symbols, 50.0);

    auto dec1 = decoder.Decode(llr);
    auto dec2 = decoder.Decode(llr);

    EXPECT_EQ(dec1, dec2);
}
