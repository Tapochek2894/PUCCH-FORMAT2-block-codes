#ifndef PUCCH_F2_MODULATOR_HPP
#define PUCCH_F2_MODULATOR_HPP

#include <complex>
#include <cstdint>
#include <vector>

namespace pucch_f2 {

class QpskModulator {
public:
    std::vector<std::complex<double>> Modulate(const std::vector<uint8_t>& codebits);

private:
    std::complex<double> MapBitsToSymbol(uint8_t msb, uint8_t lsb);
};

} // namespace pucch_f2

#endif // PUCCH_F2_MODULATOR_HPP