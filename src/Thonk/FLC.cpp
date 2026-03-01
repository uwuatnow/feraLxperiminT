#include "FLC.h"
#include <cmath>
#include <utility>
#include <algorithm>
#include <tuple>
#ifdef _OPENMP
#include <omp.h>
#else
#define omp_get_thread_num() 0
#endif

#define M_PI 3.14159265358979323846

double phi = (1.0 + sqrt(5.0)) / 2.0;

namespace flc {

FLC::FLC() : cached_N(-1), cached_bands_N(-1), window_cached_N(-1) {}

void FLC::reset() {
    // Clear all stateful buffers to ensure clean start
    compress_overlap.clear();
    decompress_overlap.clear();
    temp_block.clear();
}

void FLC::precompute_cosines(int N) {
    if (cached_N == N) return;

    cached_N = N;
    cos_cache.resize(static_cast<size_t>(N) * 2 * N);
    
    // N is 512, 2*N is 1024 (for block size 1024)
    // Precomputing doubles then casting to float for better vectorization later
    for (int k = 0; k < N; k++) {
        for (int n = 0; n < 2 * N; n++) {
            cos_cache[k * (2 * N) + n] = static_cast<float>(std::cos(M_PI / N * (n + 0.5 + N / 2.0) * (k + 0.5)));
        }
    }
}

const std::vector<float>& FLC::get_cached_window(int N) {
    if (window_cached_N == N) return window_cache;
    
    window_cached_N = N;
    window_cache.resize(2 * N);
    for (int n = 0; n < 2 * N; n++) {
        window_cache[n] = static_cast<float>(std::sin(M_PI / (2.0 * N) * (n + 0.5)));
    }
    return window_cache;
}

std::vector<float> FLC::mdct(const std::vector<float>& input) {
    const int N = static_cast<int>(input.size() / 2);
    if (N <= 0) return {};

    precompute_cosines(N);
    const auto& win = get_cached_window(N);
    
    // Pre-windowing optimization: Apply window once
    std::vector<float> windowed_input(2 * N);
    for (int i = 0; i < 2 * N; ++i) {
        windowed_input[i] = input[i] * win[i];
    }
    
    std::vector<float> output(N);

    // Optimized MDCT Loop
    // Inner loop access is strictly sequential for both windowed_input and cos_cache
    // OpenMP removed for VST efficiency (single thread vectorized is better than thread overhead for small N)
    for (int k = 0; k < N; k++) {
        float sum = 0.0f;
        const float* cos_ptr = &cos_cache[k * (2 * N)];
        for (int n = 0; n < 2 * N; n++) {
            sum += windowed_input[n] * cos_ptr[n];
        }
        output[k] = sum;
    }
    return output;
}

std::vector<float> FLC::imdct(const std::vector<float>& input) {
    const int N = static_cast<int>(input.size());
    if (N <= 0) return {};

    precompute_cosines(N);
    const auto& win = get_cached_window(N);
    std::vector<float> output(2 * N, 0.0f);
    const float factor = 2.0f / N;

    // Optimized IMDCT Loop: Reordered for cache locality
    // Outer loop over k (input), inner loop over n (output)
    // Access patterns:
    // input[k]: constant for inner loop
    // output[n]: sequential access
    // cos_cache[k*(2N)+n]: sequential access due to row-major storage
    for (int k = 0; k < N; k++) {
        float val = input[k];
        const float* cos_ptr = &cos_cache[k * (2 * N)];
        
        // This inner loop is highly vectorizable (MAC)
        for (int n = 0; n < 2 * N; n++) {
            output[n] += val * cos_ptr[n];
        }
    }
    
    // Apply window and scaling factor at the end
    for (int n = 0; n < 2 * N; n++) {
        output[n] *= win[n] * factor;
    }
    
    return output;
}

std::vector<int> FLC::fibonacci_frequency_boundaries(int n_coeffs, int n_bands) {
    if (n_bands < 2) return {0, n_coeffs};
    std::vector<double> fibs;
    double a = 1.0, b = 2.0;
    while (fibs.size() < static_cast<size_t>(n_bands)) {
        fibs.push_back(a);
        double temp = a + b;
        a = b;
        b = temp;
    }
    double sum_f = 0.0;
    for (double f : fibs) sum_f += f;
    std::vector<double> w;
    for (double f : fibs) w.push_back(f / (sum_f + 1e-12));
    std::vector<double> cum;
    cum.push_back(0.0);
    for (double ww : w) cum.push_back(cum.back() + ww);
    std::vector<int> bounds;
    for (size_t i = 0; i < cum.size(); ++i) {
        int bound = static_cast<int>(std::round(n_coeffs * cum[i]));
        bounds.push_back(bound);
    }
    bounds.back() = n_coeffs;
    
    std::vector<int> unique_bounds;
    unique_bounds.push_back(0);
    for (size_t i = 1; i < bounds.size(); ++i) {
        if (bounds[i] > unique_bounds.back()) {
            unique_bounds.push_back(bounds[i]);
        }
    }
    return unique_bounds;
}

std::vector<std::pair<int, int>> FLC::get_bands(int N, int n_bands) {
    auto boundaries = fibonacci_frequency_boundaries(N, n_bands);
    std::vector<std::pair<int, int>> bands;
    for (size_t i = 0; i + 1 < boundaries.size(); ++i) {
        bands.emplace_back(boundaries[i], boundaries[i+1]);
    }
    return bands;
}

std::tuple<std::vector<int>, std::vector<std::pair<int, int>>, float> FLC::compress(const std::vector<float>& data, float basestep) {
    const int N = static_cast<int>(data.size());
    if (compress_overlap.size() != static_cast<size_t>(N)) {
        compress_overlap.assign(N, 0.0f);
    }
    
    // Reuse temp_block efficiently
    if (temp_block.size() != static_cast<size_t>(2 * N)) {
        temp_block.resize(2 * N);
    }
    
    // block layout: [overlap | data]
    // copy overlap to head
    std::copy(compress_overlap.begin(), compress_overlap.end(), temp_block.begin());
    // copy new data to tail
    std::copy(data.begin(), data.end(), temp_block.begin() + N);
    // update overlap with new data for next time
    std::copy(data.begin(), data.end(), compress_overlap.begin());

    auto coeffs = mdct(temp_block);

    const float scale = 1.f;

    if (cached_bands_N != N) {
        cached_bands = get_bands(N, 10);
        cached_bands_N = N;
    }
    const auto& bands = cached_bands;

    std::vector<int> quantized(N, 0);

    const size_t numBands = bands.size();
    
    for (int idx = 0; idx < static_cast<int>(numBands); idx++) {
        auto [start, end] = bands[idx];
        double qstep = basestep * scale * phi;
        double inv_qstep = 1.0 / qstep;

        for (int i = start; i < end; i++) {
            double val = coeffs[i] * inv_qstep;
            quantized[i] = static_cast<int>(std::round(val));
        }
    }

    return std::make_tuple(quantized, bands, scale);
}

std::vector<float> FLC::decompress(const std::vector<int>& quantized, const std::vector<std::pair<int, int>>& bands, float basestep, float scale, int N) {
    const int actual_N = (N == -1 ? static_cast<int>(quantized.size()) : N);
    std::vector<float> recon_coeffs(actual_N, 0.0f);

    const size_t numBands = bands.size();

    for (int idx = 0; idx < static_cast<int>(numBands); idx++) {
        auto [start, end] = bands[idx];
        double qstep = basestep * scale * phi;
        // Using float for recon to match implementation types
        float f_qstep = static_cast<float>(qstep);
        
        for (int i = start; i < end && i < actual_N; i++) {
            recon_coeffs[i] = static_cast<float>(quantized[i]) * f_qstep;
        }
    }

    auto full_recon = imdct(recon_coeffs);
    
    if (decompress_overlap.size() != static_cast<size_t>(actual_N)) {
        decompress_overlap.assign(actual_N, 0.0f);
    }

    std::vector<float> output(actual_N);
    for (int i = 0; i < actual_N; i++) {
        output[i] = full_recon[i] + decompress_overlap[i];
        decompress_overlap[i] = full_recon[i + actual_N];
    }

    return output;
}

}