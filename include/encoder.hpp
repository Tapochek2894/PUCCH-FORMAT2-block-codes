#ifndef PUCCH_F2_ENCODER_HPP
#define PUCCH_F2_ENCODER_HPP

#include <array>
#include <cstdint>
#include <vector>

namespace pucch_f2 {

class Encoder {
public:
    explicit Encoder(int code_length);

    std::vector<uint8_t> Encode(const std::vector<uint8_t>& data);

private:
    static constexpr int kCodewordLength = 20;
    static constexpr int kMaxCodeLength = 13;
    static const std::array<std::array<uint8_t, kMaxCodeLength>, kCodewordLength> kGeneratorMatrix;

    int code_length_;
};

} // namespace pucch_f2

#endif // PUCCH_F2_ENCODER_HPP