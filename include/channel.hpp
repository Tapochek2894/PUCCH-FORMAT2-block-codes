#ifndef PUCCH_F2_AWGN_HPP
#define PUCCH_F2_AWGN_HPP

#include <complex>
#include <cstdint>
#include <random>
#include <vector>

namespace pucch_f2 {

class AwgnChannel {
public:
    explicit AwgnChannel(double snr_db, uint32_t seed = 5489u);
    std::vector<std::complex<double>> Transmit(const std::vector<std::complex<double>>& symbols);

private:
    std::mt19937 random_generator_;
    double sigma_;
};

} // namespace pucch_f2

#endif // PUCCH_F2_AWGN_HPP