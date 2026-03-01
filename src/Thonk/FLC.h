#pragma once
#include <vector>
#include <tuple>

namespace flc {

class FLC {
    std::vector<float> cos_cache;
    int cached_N = -1;

    int cached_bands_N = -1;
    std::vector<std::pair<int, int>> cached_bands;

    std::vector<float> compress_overlap;
    std::vector<float> decompress_overlap;
    
    // Optimizations
    std::vector<float> window_cache;
    int window_cached_N = -1;
    std::vector<float> temp_block; // Reuse in compress

    std::vector<int> fibonacci_frequency_boundaries(int n_coeffs, int n_bands);
    std::vector<std::pair<int, int>> get_bands(int N, int n_bands = 10);
    
    void precompute_cosines(int N);

    // MDCT/IMDCT
    std::vector<float> mdct(const std::vector<float>& input);
    std::vector<float> imdct(const std::vector<float>& input);
    const std::vector<float>& get_cached_window(int N);

public:
    FLC();
    void reset();
    std::tuple<std::vector<int>, std::vector<std::pair<int, int>>, float> compress(const std::vector<float>& data, float basestep = 1.0f);
    std::vector<float> decompress(const std::vector<int>& quantized, const std::vector<std::pair<int, int>>& bands, float basestep, float scale, int N = -1);
};

}