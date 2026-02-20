#include "channel.hpp"

namespace pucch_f2 {

AwgnChannel::AwgnChannel(double snr_db, uint32_t seed) : random_generator_(seed) {
    double snr_linear = std::pow(10.0, snr_db / 10.0);
    sigma_ = std::sqrt(1.0 / (2.0 * snr_linear));
}

std::vector<std::complex<double>>
AwgnChannel::Transmit(const std::vector<std::complex<double>>& symbols) {
    std::vector<std::complex<double>> noisy_symbols;
    noisy_symbols.reserve(symbols.size());

    std::normal_distribution<double> noise(0.0, sigma_);

    for (const auto& symbol : symbols) {
        double noise_re = noise(random_generator_);
        double noise_im = noise(random_generator_);
        noisy_symbols.emplace_back(symbol.real() + noise_re, symbol.imag() + noise_im);
    }

    return noisy_symbols;
}

} // namespace pucch_f2
