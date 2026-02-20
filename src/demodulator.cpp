#include "demodulator.hpp"

namespace pucch_f2 {

std::vector<double> QpskDemodulator::Demodulate(const std::vector<std::complex<double>>& symbols,
                                                double snr_db) {
    std::vector<double> llr_values;
    llr_values.reserve(symbols.size() * 2);
    double snr_linear = std::pow(10.0, snr_db / 10.0);

    for (const auto& symbol : symbols) {
        auto [llr_msb, llr_lsb] = ComputeLlr(symbol, snr_linear);
        llr_values.push_back(llr_msb);
        llr_values.push_back(llr_lsb);
    }

    return llr_values;
}

std::pair<double, double> QpskDemodulator::ComputeLlr(const std::complex<double>& symbol,
                                                      double snr_linear) {
    double scale = 2.0 * std::sqrt(2.0) * snr_linear;

    double llr_msb = -scale * symbol.real();
    double llr_lsb = -scale * symbol.imag();

    return {llr_msb, llr_lsb};
}

} // namespace pucch_f2
