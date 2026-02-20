#include "modulator.hpp"

namespace pucch_f2 {

std::vector<std::complex<double>> QpskModulator::Modulate(const std::vector<uint8_t>& codebits) {
    if (codebits.size() % 2 != 0) {
        throw std::invalid_argument("Codebits size must be even");
    }

    std::vector<std::complex<double>> modulated_symbols;
    modulated_symbols.reserve(codebits.size() / 2);

    for (std::size_t i = 0; i < codebits.size(); i += 2) {
        modulated_symbols.push_back(MapBitsToSymbol(codebits[i], codebits[i + 1]));
    }

    return modulated_symbols;
}

std::complex<double> QpskModulator::MapBitsToSymbol(uint8_t msb, uint8_t lsb) {
    double re = (msb == 0) ? 1.0 : -1.0;
    double im = (lsb == 0) ? 1.0 : -1.0;

    return std::complex<double>(re, im) / std::sqrt(2.0);
}

} // namespace pucch_f2
