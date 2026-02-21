#include "channel.hpp"
#include "decoder.hpp"
#include "demodulator.hpp"
#include "encoder.hpp"
#include "modulator.hpp"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

const uint32_t RANDOM_SEED = 5090U;

std::complex<double> ParseComplex(const std::string& string) {
    double re, im;
    char sign, j_char;
    std::stringstream ss(string);

    ss >> re >> sign;
    ss.putback(sign);
    ss >> im >> j_char;

    return std::complex<double>(re, im);
}

std::string FormatComplex(const std::complex<double>& c) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << c.real() << std::showpos << c.imag()
       << std::noshowpos << "j";

    return ss.str();
}

void ValidateCodingInput(const json& input) {
    if (!input.contains("num_of_pucch_f2_bits")) {
        throw std::invalid_argument("Missing field: 'num_of_pucch_f2_bits'");
    }
    if (!input.contains("pucch_f2_bits")) {
        throw std::invalid_argument("Missing field: 'pucch_f2_bits'");
    }

    int code_length = input["num_of_pucch_f2_bits"].get<int>();
    auto bits = input["pucch_f2_bits"].get<std::vector<uint8_t>>();

    if (static_cast<int>(bits.size()) != code_length) {
        throw std::invalid_argument("Bit count mismatch: expected " + std::to_string(code_length) +
                                    ", got " + std::to_string(bits.size()));
    }

    for (std::size_t i = 0; i < bits.size(); ++i) {
        if (bits[i] != 0 && bits[i] != 1) {
            throw std::invalid_argument("Invalid bit at position " + std::to_string(i) +
                                        ": must be 0 or 1, got " + std::to_string(bits[i]));
        }
    }

    constexpr std::array<int, 5> valid_lengths = {2, 4, 6, 8, 11};
    bool valid = false;
    for (int len : valid_lengths) {
        if (code_length == len) {
            valid = true;
            break;
        }
    }

    if (!valid) {
        throw std::invalid_argument("Invalid code_length: " + std::to_string(code_length) +
                                    ". Must be one of {2, 4, 6, 8, 11}");
    }
}

void ValidateDecodingInput(const json& input) {
    if (!input.contains("num_of_pucch_f2_bits")) {
        throw std::invalid_argument("Missing field: 'num_of_pucch_f2_bits'");
    }
    if (!input.contains("qpsk_symbols")) {
        throw std::invalid_argument("Missing field: 'qpsk_symbols'");
    }

    [[maybe_unused]] int code_length = input["num_of_pucch_f2_bits"].get<int>();
    auto symbols = input["qpsk_symbols"].get<std::vector<std::string>>();

    const int expected_symbols = 10;

    if (static_cast<int>(symbols.size()) != expected_symbols) {
        throw std::invalid_argument("Symbol count mismatch: expected " +
                                    std::to_string(expected_symbols) + ", got " +
                                    std::to_string(symbols.size()));
    }

    for (std::size_t i = 0; i < symbols.size(); ++i) {
        const std::string& sym = symbols[i];
        if (sym.empty() || sym.back() != 'j') {
            throw std::invalid_argument("Invalid symbol format at index " + std::to_string(i) +
                                        ": '" + sym + "' (expected format: '0.707+0.707j')");
        }
    }
}

void ValidateChannelSimulationInput(const json& input) {
    if (!input.contains("num_of_pucch_f2_bits")) {
        throw std::invalid_argument("Missing field: 'num_of_pucch_f2_bits'");
    }
    if (!input.contains("iterations")) {
        throw std::invalid_argument("Missing field: 'iterations'");
    }
    if (!input.contains("snr_db")) {
        throw std::invalid_argument("Missing field: 'snr_db'");
    }

    int code_length = input["num_of_pucch_f2_bits"].get<int>();
    int iterations = input["iterations"].get<int>();
    double snr_db = input["snr_db"].get<double>();

    constexpr std::array<int, 5> valid_lengths = {2, 4, 6, 8, 11};
    bool valid = false;
    for (int len : valid_lengths) {
        if (code_length == len) {
            valid = true;
            break;
        }
    }
    if (!valid) {
        throw std::invalid_argument("Invalid code_length: " + std::to_string(code_length) +
                                    ". Must be one of {2, 4, 6, 8, 11}");
    }

    if (iterations <= 0) {
        throw std::invalid_argument("iterations must be positive, got " +
                                    std::to_string(iterations));
    }

    if (snr_db < -20.0 || snr_db > 30.0) {
        std::cerr << "Warning: snr_db=" << snr_db << " is outside typical range [-20, 30]\n";
    }
}

json RunCoding(const json& input) {
    ValidateCodingInput(input);

    int code_length = input["num_of_pucch_f2_bits"].get<int>();
    std::vector<uint8_t> data = input["pucch_f2_bits"].get<std::vector<uint8_t>>();

    pucch_f2::Encoder encoder(code_length);
    auto codeword = encoder.Encode(data);

    pucch_f2::QpskModulator modulator;
    auto symbols = modulator.Modulate(codeword);

    json output;
    output["mode"] = "coding";
    output["num_of_pucch_f2_bits"] = code_length;

    std::vector<std::string> symbols_str;
    for (const auto& sym : symbols) {
        symbols_str.push_back(FormatComplex(sym));
    }
    output["qpsk_symbols"] = symbols_str;

    return output;
}

json RunDecoding(const json& input) {
    ValidateDecodingInput(input);

    int code_length = input["num_of_pucch_f2_bits"].get<int>();
    std::vector<std::string> symbols_str = input["qpsk_symbols"].get<std::vector<std::string>>();

    std::vector<std::complex<double>> symbols;
    symbols.reserve(symbols_str.size());
    for (const auto& str : symbols_str) {
        symbols.push_back(ParseComplex(str));
    }

    pucch_f2::QpskDemodulator demodulator;
    auto llr = demodulator.Demodulate(symbols, 0.0);

    pucch_f2::Decoder decoder(code_length);
    auto decoded = decoder.Decode(llr);

    json output;
    output["mode"] = "decoding";
    output["num_of_pucch_f2_bits"] = code_length;
    output["pucch_f2_bits"] = decoded;

    return output;
}

json RunChannelSimulation(const json& input) {
    ValidateChannelSimulationInput(input);

    int code_length = input["num_of_pucch_f2_bits"].get<int>();
    int iterations = input["iterations"].get<int>();
    double snr_db = input["snr_db"].get<double>();

    pucch_f2::Encoder encoder(code_length);
    pucch_f2::QpskModulator modulator;
    pucch_f2::AwgnChannel channel(snr_db, RANDOM_SEED);
    pucch_f2::QpskDemodulator demodulator;
    pucch_f2::Decoder decoder(code_length);

    std::mt19937 rng(RANDOM_SEED);
    std::uniform_int_distribution<int> bit_dist(0, 1);

    int success_count = 0;
    int failed_count = 0;

    for (int iter = 0; iter < iterations; ++iter) {
        std::vector<uint8_t> data(code_length);
        for (int i = 0; i < code_length; ++i) {
            data[i] = static_cast<uint8_t>(bit_dist(rng));
        }

        auto codeword = encoder.Encode(data);
        auto symbols = modulator.Modulate(codeword);
        auto received = channel.Transmit(symbols);
        auto llr = demodulator.Demodulate(received, snr_db);
        auto decoded = decoder.Decode(llr);

        if (data == decoded) {
            ++success_count;
        } else {
            ++failed_count;
        }
    }

    double bler = static_cast<double>(failed_count) / iterations;

    json output;
    output["mode"] = "channel simulation";
    output["num_of_pucch_f2_bits"] = code_length;
    output["snr_db"] = snr_db;
    output["iterations"] = iterations;
    output["bler"] = bler;
    output["success"] = success_count;
    output["failed"] = failed_count;

    return output;
}

std::string ReadJsonInput(int argc, char* argv[]) {
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg.find('/') != std::string::npos || arg.find('.') != std::string::npos) {
            std::ifstream file(arg);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                return buffer.str();
            }
        }

        return arg;
    }

    std::stringstream buffer;
    buffer << std::cin.rdbuf();

    return buffer.str();
}

int main(int argc, char* argv[]) {
    try {
        std::string json_str = ReadJsonInput(argc, argv);

        if (json_str.empty()) {
            throw std::invalid_argument("Empty JSON input");
        }

        json input;
        try {
            input = json::parse(json_str);
        } catch (const json::parse_error& e) {
            throw std::invalid_argument("Invalid JSON syntax: " + std::string(e.what()));
        }

        if (!input.is_object()) {
            throw std::invalid_argument("JSON root must be an object");
        }

        if (!input.contains("mode") || !input["mode"].is_string()) {
            throw std::invalid_argument("Missing or invalid field: 'mode' (must be string)");
        }
        std::string mode = input["mode"].get<std::string>();

        json output;
        if (mode == "coding") {
            output = RunCoding(input);
        } else if (mode == "decoding") {
            output = RunDecoding(input);
        } else if (mode == "channel simulation") {
            output = RunChannelSimulation(input);
        } else {
            throw std::invalid_argument(
                "Unknown mode: '" + mode +
                "'. Valid modes: 'coding', 'decoding', 'channel simulation'");
        }

        std::string output_str = output.dump(4);

        std::cout << output_str << std::endl;

        const char* disable_file = std::getenv("PUCCH_DISABLE_FILE_OUTPUT");
        if (disable_file == nullptr) {
            try {
                std::ofstream result_file("result.json");
                if (!result_file.is_open()) {
                    throw std::runtime_error("Cannot create result.json");
                }
                result_file << output_str << std::endl;
                result_file.close();
            } catch (const std::exception& e) {
                std::cerr << "Warning: Failed to write result.json: " << e.what() << std::endl;
            }
        }

        return 0;

    } catch (const std::invalid_argument& e) {
        std::cerr << "Validation error: " + std::string(e.what()) << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " + std::string(e.what()) << std::endl;
        return 1;
    }
}