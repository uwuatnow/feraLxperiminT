#include "Thonk/FLC.h"
#include <cstring>

#define M_PI 3.14159265358979323846

namespace flc {

const char MAGIC[8] = {'F','L','C','1',0,0,0,0};
const uint16_t VER_V2 = 2;

// Pre-computed Fibonacci sequence (covers all uint64_t values)
static const uint64_t FIB_TABLE[] = {
    1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946,
    17711, 28657, 46368, 75025, 121393, 196418, 317811, 514229, 832040, 1346269, 2178309, 3524578,
    5702887, 9227465, 14930352, 24157817, 39088169, 63245986, 102334155, 165580141, 267914296,
    433494437, 701408733, 1134903170, 1836311903, 2971215073ULL, 4807526976ULL, 7778742049ULL,
    12586269025ULL, 20365011074ULL, 32951280099ULL, 53316291173ULL, 86267571272ULL, 139583862445ULL,
    225851433717ULL, 365435296162ULL, 591286729879ULL, 956722026041ULL, 1548008755920ULL,
    2504730781961ULL, 4052739537881ULL, 6557470319842ULL, 10610209857723ULL, 17167680177565ULL,
    27777890035288ULL, 44945570212853ULL, 72723460248141ULL, 117669030460994ULL, 190392490709135ULL,
    308061521170129ULL, 498454011879264ULL, 806515533049393ULL, 1304969544928657ULL, 2111485077978050ULL,
    3416454622906707ULL, 5527939700884757ULL, 8944394323791464ULL, 14472334024676221ULL,
    23416728348467685ULL, 37889062373143906ULL, 61305790721611591ULL, 99194853094755497ULL,
    160500643816367088ULL, 259695496911122585ULL, 420196140727489673ULL, 679891637638612258ULL,
    1100087778366101931ULL, 1779979416004714189ULL, 2880067194370816120ULL, 4660046610375530309ULL,
    7540113804746346429ULL, 12200160415121876738ULL
};
static const int FIB_TABLE_SIZE = sizeof(FIB_TABLE) / sizeof(FIB_TABLE[0]);

void FLC::BitWriter::write_bit(int b) {
    acc = (acc << 1) | (b & 1);
    nbits++;
    if (nbits == 8) {
        buf.push_back(acc);
        acc = 0;
        nbits = 0;
    }
}

std::vector<uint8_t> FLC::BitWriter::finish() {
    if (nbits) {
        buf.push_back(acc << (8 - nbits));
    }
    return buf;
}

int FLC::BitReader::read_bit() {
    if (nbits == 0) {
        acc = data[i++];
        nbits = 8;
    }
    int b = (acc >> (nbits - 1)) & 1;
    nbits--;
    return b;
}

void FLC::fib_encode_nonneg(BitWriter& bw, uint64_t n) {
    uint64_t m = n + 1;
    
    // Find how many fibonacci numbers we need
    int numFibs = 0;
    for (int i = 0; i < FIB_TABLE_SIZE && FIB_TABLE[i] <= m; ++i) {
        numFibs = i + 1;
    }
    if (numFibs == 0) numFibs = 1;
    
    // Greedy decomposition using lookup table
    uint8_t bits[92] = {0};  // Stack buffer, no allocation
    for (int i = numFibs - 1; i >= 0 && m > 0; --i) {
        if (FIB_TABLE[i] <= m) {
            bits[i] = 1;
            m -= FIB_TABLE[i];
        }
    }
    
    for (int i = 0; i < numFibs; ++i) {
        bw.write_bit(bits[i]);
    }
    bw.write_bit(1);  // Terminator
}

uint64_t FLC::fib_decode_nonneg(BitReader& br) {
    int bitIdx = 0;
    uint8_t bits[92] = {0};
    int prev = 0;
    
    while (true) {
        int b = br.read_bit();
        bits[bitIdx++] = b;
        if (prev == 1 && b == 1) break;
        prev = b;
    }
    
    uint64_t m = 0;
    for (int i = 0; i < bitIdx - 1; ++i) {
        if (bits[i]) m += FIB_TABLE[i];
    }
    return m - 1;
}

std::vector<uint8_t> FLC::rle_fib_encode_ints(const std::vector<int64_t>& ints) {
    BitWriter bw;
    bw.buf.reserve(ints.size() / 2);  // Rough estimate to reduce reallocations
    int64_t zrun = 0;
    for (size_t idx = 0; idx < ints.size(); ++idx) {
        int64_t v = ints[idx];
        if (v == 0) {
            zrun++;
            continue;
        }
        if (zrun) {
            bw.write_bit(0);
            fib_encode_nonneg(bw, zrun);
            zrun = 0;
        }
        bw.write_bit(1);
        uint64_t uv = (v << 1) ^ (v >> 63);
        fib_encode_nonneg(bw, uv);
    }
    if (zrun) {
        bw.write_bit(0);
        fib_encode_nonneg(bw, zrun);
    }
    return bw.finish();
}

std::vector<int64_t> FLC::rle_fib_decode_ints(const std::vector<uint8_t>& payload, size_t n_out) {
    BitReader br(payload);
    std::vector<int64_t> out(n_out, 0);
    size_t i = 0;
    while (i < n_out) {
        if (br.read_bit() == 0) {
            uint64_t zrun = fib_decode_nonneg(br);
            i += zrun;
            if (i > n_out) i = n_out;
        } else {
            uint64_t u = fib_decode_nonneg(br);
            int64_t v = (u >> 1) ^ (-(u & 1));
            if (i < n_out) out[i] = v;
            i++;
        }
    }
    return out;
}

FLC::FLC()
    :phi(1.618033988749895f / 2.f)
    ,cached_N(-1)
{
}

void FLC::precompute_cosines(int N) {
    if (cached_N == N) return;

    cached_N = N;
    cos_cache.resize(static_cast<size_t>(N) * N);

    #pragma omp parallel for collapse(2)
    for (int k = 0; k < N; k++) {
        for (int n = 0; n < N; n++) {
            cos_cache[k * N + n] = std::cos(M_PI * k * (2.0 * n + 1.0) / (2.0 * N));
        }
    }
}

double FLC::get_cos(int k, int n, int N) {
    if (cached_N == N) {
        return cos_cache[k * N + n];
    }
    return std::cos(M_PI * k * (2.0 * n + 1.0) / (2.0 * N));
}

std::vector<int> FLC::generate_fib_widths(int N) {
    std::vector<int> widths;
    int a = 1, b = 1;
    int sum_w = 0;
    while (sum_w < N) {
        widths.push_back(a);
        sum_w += a;
        int temp = a + b;
        a = b;
        b = temp;
        if (a > N - sum_w) a = N - sum_w;
    }

    if (sum_w > N) {
        widths.back() -= (sum_w - N);
    }

    return widths;
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
    bounds.push_back(0);
    for (size_t i = 0; i < w.size(); ++i) {
        int bound = static_cast<int>(std::round(n_coeffs * cum[i]));
        bounds.push_back(bound);
    }
    bounds.push_back(n_coeffs);
    // Ensure monotonic
    for (size_t i = 1; i < bounds.size(); ++i) {
        if (bounds[i] <= bounds[i-1]) bounds[i] = bounds[i-1] + 1;
    }
    return bounds;
}

std::vector<std::pair<int, int>> FLC::get_bands(int N, int n_bands) {
    auto boundaries = fibonacci_frequency_boundaries(N, n_bands);
    std::vector<std::pair<int, int>> bands;
    bands.reserve(boundaries.size());
    for (size_t i = 0; i + 1 < boundaries.size(); ++i) {
        bands.emplace_back(boundaries[i], boundaries[i+1]);
    }
    return bands;
}

std::vector<float> FLC::dct(const std::vector<float>& x) {
    const int N = static_cast<int>(x.size());
    std::vector<float> y(N, 0.0f);

    if (N <= 1) return y;

    precompute_cosines(N);

    const double sqrt1N = std::sqrt(1.0 / N);
    const double sqrt2N = std::sqrt(2.0 / N);
    const float* xPtr = x.data();
    const double* cosPtr = cos_cache.data();

    #pragma omp parallel for
    for (int k = 0; k < N; k++) {
        double sum = 0.0;
        const double* rowPtr = cosPtr + k * N;
        for (int n = 0; n < N; n++) {
            sum += xPtr[n] * rowPtr[n];
        }
        y[k] = static_cast<float>((k == 0 ? sqrt1N : sqrt2N) * sum);
    }

    return y;
}

std::vector<float> FLC::idct(const std::vector<float>& y) {
    const int N = static_cast<int>(y.size());
    std::vector<float> x(N, 0.0f);

    if (N <= 1) return x;

    precompute_cosines(N);

    const double sqrt1N = std::sqrt(1.0 / N);
    const double sqrt2N = std::sqrt(2.0 / N);
    
    // Pre-scale coefficients to avoid repeated multiplications
    std::vector<double> scaledY(N);
    scaledY[0] = y[0] * sqrt1N;
    for (int k = 1; k < N; k++) {
        scaledY[k] = y[k] * sqrt2N;
    }
    
    const double* scaledPtr = scaledY.data();
    const double* cosPtr = cos_cache.data();

    #pragma omp parallel for
    for (int n = 0; n < N; n++) {
        double sum = scaledPtr[0];  // cos(0) = 1 for k=0
        for (int k = 1; k < N; k++) {
            sum += scaledPtr[k] * cosPtr[k * N + n];
        }
        x[n] = static_cast<float>(sum);
    }

    return x;
}

std::pair<std::vector<int>, std::vector<std::pair<int, int>>> FLC::flc_compress(const std::vector<float>& data, float basestep) {
    auto coeffs = dct(data);
    const int N = static_cast<int>(coeffs.size());
    
    // Use cached bands if N matches
    if (cached_bands_N != N) {
        cached_bands = get_bands(N, 10);
        cached_bands_N = N;
    }
    const auto& bands = cached_bands;
    
    std::vector<int> quantized(N, 0);

    // Precompute quantization steps for each band
    const size_t numBands = bands.size();
    double qsteps[16];  // Stack allocation, max 16 bands
    for (size_t idx = 0; idx < numBands; idx++) {
        qsteps[idx] = basestep * std::pow(phi, static_cast<double>(idx));
        if (qsteps[idx] == 0.0) qsteps[idx] = 1e-6;
    }

    #pragma omp parallel for
    for (int idx = 0; idx < static_cast<int>(numBands); idx++) {
        auto [start, end] = bands[idx];
        double qstep = qsteps[idx];
        double inv_qstep = 1.0 / qstep;
        for (int i = start; i < end; i++) {
            quantized[i] = static_cast<int>(std::round(coeffs[i] * inv_qstep));
        }
    }

    return {quantized, bands};
}

std::vector<float> FLC::flc_decompress(const std::vector<int>& quantized, const std::vector<std::pair<int, int>>& bands, float basestep, int N) {
    const int actual_N = (N == -1 ? static_cast<int>(quantized.size()) : N);
    std::vector<float> recon_coeffs(actual_N, 0.0f);

    const size_t numBands = bands.size();
    double qsteps[16];  // Stack allocation
    for (size_t idx = 0; idx < numBands; idx++) {
        qsteps[idx] = basestep * std::pow(phi, static_cast<double>(idx));
    }

    #pragma omp parallel for
    for (int idx = 0; idx < static_cast<int>(numBands); idx++) {
        auto [start, end] = bands[idx];
        double qstep = qsteps[idx];
        for (int i = start; i < end && i < actual_N; i++) {
            recon_coeffs[i] = static_cast<float>(quantized[i] * qstep);
        }
    }

    return idct(recon_coeffs);
}

std::vector<float> FLC::progressive_recon(int k, const std::vector<int>& quantized, const std::vector<std::pair<int, int>>& bands, float basestep, int N) {
    size_t actual_N = (N == -1 ? quantized.size() : static_cast<size_t>(N));
    auto full_bands = get_bands(static_cast<int>(actual_N), 10);
    std::vector<float> recon_coeffs(actual_N, 0.0f);

    size_t max_bands = std::min(static_cast<size_t>(k), full_bands.size());

    std::vector<double> qsteps(max_bands);
    for (size_t idx = 0; idx < max_bands; idx++) {
        qsteps[idx] = static_cast<double>(basestep) * std::pow(phi, static_cast<double>(idx));
    }

    #pragma omp parallel for
    for (int idx = 0; idx < static_cast<int>(max_bands); idx++) {
        auto [start, end] = full_bands[idx];
        double qstep = qsteps[idx];
        for (int i = start; i < end && i < static_cast<int>(actual_N); i++) {
            recon_coeffs[i] = static_cast<float>(static_cast<double>(quantized[i]) * qstep);
        }
    }

    return idct(recon_coeffs);
}

std::vector<uint8_t> FLC::compress_to_fib_binary(const std::vector<float>& data, float basestep) {
    auto [quantized, bands] = flc_compress(data, basestep);
    size_t N = data.size();
    size_t n_bands = bands.size(); // should be 10

    std::vector<uint32_t> bnds;
    bnds.push_back(0);
    for (auto& p : bands) {
        bnds.push_back(p.second);
    }

    std::vector<int64_t> diffs(N);
    for (size_t i = 0; i < N; ++i) {
        diffs[i] = quantized[i] - (i > 0 ? quantized[i - 1] : 0);
    }

    std::vector<uint8_t> payload = rle_fib_encode_ints(diffs);

    size_t h_sz = 8 + 2 + 8 + 4 + 4 + 2 + 8 + 8 + 2;
    std::vector<uint8_t> result;
    result.reserve(h_sz + bnds.size() * 4 + 4 + payload.size());

    result.insert(result.end(), MAGIC, MAGIC + 8);
    uint16_t ver = VER_V2;
    result.insert(result.end(), reinterpret_cast<uint8_t*>(&ver), reinterpret_cast<uint8_t*>(&ver) + 2);
    uint64_t n_bytes = N * sizeof(float);
    result.insert(result.end(), reinterpret_cast<uint8_t*>(&n_bytes), reinterpret_cast<uint8_t*>(&n_bytes) + 8);
    uint32_t block_len = N;
    result.insert(result.end(), reinterpret_cast<uint8_t*>(&block_len), reinterpret_cast<uint8_t*>(&block_len) + 4);
    uint32_t n_blks = 1;
    result.insert(result.end(), reinterpret_cast<uint8_t*>(&n_blks), reinterpret_cast<uint8_t*>(&n_blks) + 4);
    uint16_t nb = n_bands;
    result.insert(result.end(), reinterpret_cast<uint8_t*>(&nb), reinterpret_cast<uint8_t*>(&nb) + 2);
    double step = basestep;
    result.insert(result.end(), reinterpret_cast<uint8_t*>(&step), reinterpret_cast<uint8_t*>(&step) + 8);
    double mu = 0.0;
    result.insert(result.end(), reinterpret_cast<uint8_t*>(&mu), reinterpret_cast<uint8_t*>(&mu) + 8);
    uint16_t bnd_l = bnds.size();
    result.insert(result.end(), reinterpret_cast<uint8_t*>(&bnd_l), reinterpret_cast<uint8_t*>(&bnd_l) + 2);

    for (uint32_t b : bnds) {
        result.insert(result.end(), reinterpret_cast<uint8_t*>(&b), reinterpret_cast<uint8_t*>(&b) + 4);
    }

    uint32_t p_len = payload.size();
    result.insert(result.end(), reinterpret_cast<uint8_t*>(&p_len), reinterpret_cast<uint8_t*>(&p_len) + 4);
    result.insert(result.end(), payload.begin(), payload.end());

    return result;
}

std::vector<float> FLC::decompress_from_fib_binary(const std::vector<uint8_t>& bytes) {
    if (bytes.size() < 8 + 2 + 8 + 4 + 4 + 2 + 8 + 8 + 2) return {};

    size_t off = 0;

    if (std::memcmp(&bytes[off], MAGIC, 8) != 0) return {};
    off += 8;

    uint16_t ver;
    std::memcpy(&ver, &bytes[off], 2);
    off += 2;
    if (ver != VER_V2) return {};

    uint64_t n_bytes;
    std::memcpy(&n_bytes, &bytes[off], 8);
    off += 8;
    size_t n_floats = n_bytes / sizeof(float);

    uint32_t block_len;
    std::memcpy(&block_len, &bytes[off], 4);
    off += 4;

    uint32_t n_blks;
    std::memcpy(&n_blks, &bytes[off], 4);
    off += 4;

    uint16_t n_bands;
    std::memcpy(&n_bands, &bytes[off], 2);
    off += 2;

    double step;
    std::memcpy(&step, &bytes[off], 8);
    off += 8;

    double mu;
    std::memcpy(&mu, &bytes[off], 8);
    off += 8;

    uint16_t bnd_l;
    std::memcpy(&bnd_l, &bytes[off], 2);
    off += 2;

    std::vector<uint32_t> bnds(bnd_l);
    for (uint16_t i = 0; i < bnd_l; ++i) {
        std::memcpy(&bnds[i], &bytes[off], 4);
        off += 4;
    }

    std::vector<std::pair<int, int>> bands;
    for (size_t i = 0; i + 1 < bnds.size(); ++i) {
        bands.emplace_back(bnds[i], bnds[i + 1]);
    }

    uint32_t p_len;
    std::memcpy(&p_len, &bytes[off], 4);
    off += 4;

    std::vector<uint8_t> payload(bytes.begin() + off, bytes.begin() + off + p_len);

    std::vector<int64_t> diffs = rle_fib_decode_ints(payload, n_floats);

    std::vector<int> quantized(n_floats);
    quantized[0] = diffs[0];
    for (size_t i = 1; i < n_floats; ++i) {
        quantized[i] = quantized[i - 1] + diffs[i];
    }

    return flc_decompress(quantized, bands, step, n_floats);
}

}