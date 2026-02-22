#include "decoder.hpp"
#include "encoder.hpp"
#include <array>
#include <limits>
#include <stdexcept>
#include <string>

namespace pucch_f2 {

Decoder::Decoder(int code_length) : code_length_(code_length), num_codewords_(1 << code_length) {
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

    if (codeword_table_.empty()) {
        BuildCodewordTable();
    }
}

void Decoder::BuildCodewordTable() {
    codeword_table_.reserve(num_codewords_);

    Encoder encoder(code_length_);

    for (int data_idx = 0; data_idx < num_codewords_; ++data_idx) {
        std::vector<uint8_t> data(code_length_);
        for (int i = 0; i < code_length_; ++i) {
            data[i] = (data_idx >> (code_length_ - 1 - i)) & 1;
        }

        codeword_table_.push_back(encoder.Encode(data));
    }
}

std::vector<uint8_t> Decoder::Decode(const std::vector<double>& llr_values) {
    if (static_cast<int>(llr_values.size()) != kCodewordLength) {
        throw std::invalid_argument("LLR size mismatch");
    }

    double max_metric = -std::numeric_limits<double>::infinity();
    int best_idx = 0;

    for (int idx = 0; idx < num_codewords_; ++idx) {
        double metric = ComputeMetric(codeword_table_[idx], llr_values);
        if (metric > max_metric) {
            max_metric = metric;
            best_idx = idx;
        }
    }

    std::vector<uint8_t> decoded(code_length_);
    for (int i = 0; i < code_length_; ++i) {
        decoded[i] = (best_idx >> (code_length_ - 1 - i)) & 1;
    }

    return decoded;
}

double Decoder::ComputeMetric(const std::vector<uint8_t>& codeword,
                              const std::vector<double>& llr) {
    double metric = 0.0;

    for (int i = 0; i < kCodewordLength; ++i) {
        double symbol = (codeword[i] == 1) ? 1.0 : -1.0;
        metric += symbol * llr[i];
    }

    return metric;
}

} // namespace pucch_f2
