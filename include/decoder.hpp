#ifndef PUCCH_F2_DECODER_HPP
#define PUCCH_F2_DECODER_HPP

#include <cstdint>
#include <vector>

namespace pucch_f2 {

class Decoder {
public:
    explicit Decoder(int code_length);

    std::vector<uint8_t> Decode(const std::vector<double>& llr_values);

private:
    static constexpr int kCodewordLength = 20;
    static constexpr int kMaxCodeLength = 13;

    int code_length_;
    int num_codewords_;

    std::vector<std::vector<uint8_t>> codeword_table_;

    void BuildCodewordTable();

    double ComputeMetric(const std::vector<uint8_t>& codeword, const std::vector<double>& llr);
};

} // namespace pucch_f2

#endif // PUCCH_F2_DECODER_HPP