#include "encoder.hpp"
#include <stdexcept>
#include <string>

namespace pucch_f2 {

constexpr std::array<std::array<uint8_t, Encoder::kMaxCodeLength>, Encoder::kCodewordLength>
    Encoder::kGeneratorMatrix = {{

        {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
        {1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
        {1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1},
        {1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1},
        {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1},
        {1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1},
        {1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1},
        {1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1},
        {1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1},
        {1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1},
        {1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1},
        {1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1},
        {1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1},
        {1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1},
        {1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1},
        {1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0}

    }};

constexpr std::array<int, 5> kValidCodeLengths = {2, 4, 6, 8, 11};

Encoder::Encoder(int code_length) : code_length_(code_length) {
    bool is_valid = false;
    for (int valid_len : kValidCodeLengths) {
        if (code_length_ == valid_len) {
            is_valid = true;
            break;
        }
    }

    if (!is_valid) {
        throw std::invalid_argument("Invalid code_length: " + std::to_string(code_length_) +
                                    ". Must be one of {2, 4, 6, 8, 11} for PUCCH Format 2");
    }
}

std::vector<uint8_t> Encoder::Encode(const std::vector<uint8_t>& data) {
    if (static_cast<int>(data.size()) != code_length_) {
        throw std::invalid_argument("Data size mismatch: expected " + std::to_string(code_length_) +
                                    " bits, got " + std::to_string(data.size()));
    }

    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] != 0 && data[i] != 1) {
            throw std::invalid_argument("Invalid bit value at position " + std::to_string(i) +
                                        ": must be 0 or 1, got " + std::to_string(data[i]));
        }
    }

    const int start_col = kMaxCodeLength - code_length_;

    std::vector<uint8_t> codeword(kCodewordLength, 0);

    for (int row = 0; row < kCodewordLength; ++row) {
        int sum = 0;
        for (int col = 0; col < code_length_; ++col) {
            sum ^= kGeneratorMatrix[row][start_col + col] & data[col];
        }
        codeword[row] = static_cast<uint8_t>(sum);
    }

    return codeword;
}

} // namespace pucch_f2
