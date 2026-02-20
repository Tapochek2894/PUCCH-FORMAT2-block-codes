#ifndef PUCH_F2_QPSK_DEMODULATOR_HPP
#define PUCH_F2_QPSK_DEMODULATOR_HPP

#include <complex>
#include <cstdint>
#include <vector>

namespace pucch_f2 {

class QpskDemodulator {
public:
    std::vector<double> Demodulate(const std::vector<std::complex<double>>& symbols, double snr_db);

private:
    std::pair<double, double> ComputeLlr(const std::complex<double>& symbol, double snr_linear);
};

} // namespace pucch_f2

#endif // PUCH_F2_QPSK_DEMODULATOR_HPP