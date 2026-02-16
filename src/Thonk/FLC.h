#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <utility>
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#include <cstdlib>

namespace flc {

//NOTE: FLC is lossy. ~8% loss on a sine wave
class FLC {
    double phi;
    std::vector<double> cos_cache;
    int cached_N;
    std::vector<int> prev_quantized;
    bool has_prev = false;
    
    // Cached bands and qsteps for repeated calls with same N
    int cached_bands_N = -1;
    std::vector<std::pair<int, int>> cached_bands;
    std::vector<double> cached_qsteps;

    // Fibonacci coding helpers
    struct BitWriter {
        std::vector<uint8_t> buf;
        uint8_t acc = 0;
        int nbits = 0;
        void write_bit(int b);
        std::vector<uint8_t> finish();
    };

    struct BitReader {
        const uint8_t* data;
        size_t i = 0;
        uint8_t acc = 0;
        int nbits = 0;
        BitReader(const std::vector<uint8_t>& d) : data(d.data()) {}
        int read_bit();
    };

    void fib_encode_nonneg(BitWriter& bw, uint64_t n);
    uint64_t fib_decode_nonneg(BitReader& br);
    std::vector<uint8_t> rle_fib_encode_ints(const std::vector<int64_t>& ints);
    std::vector<int64_t> rle_fib_decode_ints(const std::vector<uint8_t>& payload, size_t n_out);

public:
    std::vector<int> generate_fib_widths(int N);

    std::vector<int> fibonacci_frequency_boundaries(int n_coeffs, int n_bands);

    std::vector<std::pair<int, int>> get_bands(int N, int n_bands = 10);

    std::vector<float> dct(const std::vector<float>& x);

    std::vector<float> idct(const std::vector<float>& y);

private:
    void precompute_cosines(int N);
    double get_cos(int k, int n, int N);

public:
    FLC();

    std::pair<std::vector<int>, std::vector<std::pair<int, int>>> flc_compress(const std::vector<float>& data, float basestep = 1.0f);

    std::vector<float> flc_decompress(const std::vector<int>& quantized, const std::vector<std::pair<int, int>>& bands, float basestep = 1.0f, int N = -1);

    std::vector<float> progressive_recon(int k, const std::vector<int>& quantized, const std::vector<std::pair<int, int>>& bands, float basestep = 1.0f, int N = -1);

    std::vector<uint8_t> compress_to_fib_binary(const std::vector<float>& data, float basestep = 1.0f);

    std::vector<float> decompress_from_fib_binary(const std::vector<uint8_t>& bytes);
};

}