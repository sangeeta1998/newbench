#include <iostream>
#include <vector>
#include <complex>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>

// Large static data array (8MB)
static unsigned char big_data[8 * 1024 * 1024] = {42};

// Mandelbrot
int mandelbrot(int width, int height, int max_iter) {
    int sum = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::complex<double> c(
                (x - width/2.0) * 4.0 / width,
                (y - height/2.0) * 4.0 / width
            );
            std::complex<double> z = 0;
            int iter = 0;
            while (abs(z) < 2.0 && iter < max_iter) {
                z = z*z + c;
                ++iter;
            }
            sum += iter;
        }
    }
    return sum;
}

// Matrix multiplication
int matmul(int N) {
    std::vector<std::vector<double>> A(N, std::vector<double>(N, 1.1));
    std::vector<std::vector<double>> B(N, std::vector<double>(N, 2.2));
    std::vector<std::vector<double>> C(N, std::vector<double>(N, 0.0));
    double checksum = 0.0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                C[i][j] += A[i][k] * B[k][j];
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            checksum += C[i][j];
    return static_cast<int>(checksum) % 1000000;
}

// Simple 1D FFT (Cooley-Tukey, radix-2, not optimized)
int fft(int N) {
    std::vector<std::complex<double>> x(N);
    for (int i = 0; i < N; ++i)
        x[i] = std::complex<double>(i % 256, 0);
    // Bit-reversal permutation
    int n = N;
    int j = 0;
    for (int i = 0; i < n; ++i) {
        if (i < j) std::swap(x[i], x[j]);
        int m = n / 2;
        while (m >= 1 && j >= m) {
            j -= m;
            m /= 2;
        }
        j += m;
    }
    // FFT
    for (int s = 1; s <= __builtin_ctz(N); ++s) {
        int m = 1 << s;
        std::complex<double> wm = std::exp(std::complex<double>(0, -2.0 * M_PI / m));
        for (int k = 0; k < n; k += m) {
            std::complex<double> w = 1;
            for (int j = 0; j < m/2; ++j) {
                std::complex<double> t = w * x[k + j + m/2];
                std::complex<double> u = x[k + j];
                x[k + j] = u + t;
                x[k + j + m/2] = u - t;
                w *= wm;
            }
        }
    }
    double checksum = 0.0;
    for (int i = 0; i < N; ++i)
        checksum += std::abs(x[i]);
    return static_cast<int>(checksum) % 1000000;
}

// Image convolution (simple blur)
int blur(int W, int H) {
    std::vector<std::vector<int>> img(H, std::vector<int>(W, 128));
    std::vector<std::vector<int>> out(H, std::vector<int>(W, 0));
    int kernel[3][3] = {{1,2,1},{2,4,2},{1,2,1}};
    int sum = 0;
    for (int y = 1; y < H-1; ++y) {
        for (int x = 1; x < W-1; ++x) {
            int acc = 0;
            for (int ky = -1; ky <= 1; ++ky)
                for (int kx = -1; kx <= 1; ++kx)
                    acc += img[y+ky][x+kx] * kernel[ky+1][kx+1];
            out[y][x] = acc / 16;
            sum += out[y][x];
        }
    }
    return sum;
}

// Sorting large array
int sort_bench(int N) {
    std::vector<int> arr(N);
    std::mt19937 rng(42);
    std::generate(arr.begin(), arr.end(), rng);
    std::sort(arr.begin(), arr.end());
    return arr[N/2];
}

int main() {
    using namespace std::chrono;
    auto t0 = high_resolution_clock::now();
    int mandel = mandelbrot(800, 800, 1000);
    auto t1 = high_resolution_clock::now();
    int mat = matmul(256);
    auto t2 = high_resolution_clock::now();
    int fftsum = fft(2048);
    auto t3 = high_resolution_clock::now();
    int blurval = blur(1024, 1024);
    auto t4 = high_resolution_clock::now();
    int sortval = sort_bench(2*1024*1024);
    auto t5 = high_resolution_clock::now();
    unsigned long long bigsum = 0;
    for (size_t i = 0; i < sizeof(big_data); ++i) bigsum += big_data[i];
    auto t6 = high_resolution_clock::now();

    std::cout << "SuperBench Results:\n";
    std::cout << "  Mandelbrot: " << mandel << " (" << duration_cast<milliseconds>(t1-t0).count() << " ms)\n";
    std::cout << "  MatMul:     " << mat << " (" << duration_cast<milliseconds>(t2-t1).count() << " ms)\n";
    std::cout << "  FFT:        " << fftsum << " (" << duration_cast<milliseconds>(t3-t2).count() << " ms)\n";
    std::cout << "  Blur:       " << blurval << " (" << duration_cast<milliseconds>(t4-t3).count() << " ms)\n";
    std::cout << "  Sort:       " << sortval << " (" << duration_cast<milliseconds>(t5-t4).count() << " ms)\n";
    std::cout << "  BigDataSum: " << bigsum << " (" << duration_cast<milliseconds>(t6-t5).count() << " ms)\n";
    std::cout << "Total time:   " << duration_cast<milliseconds>(t6-t0).count() << " ms\n";
    return 0;
} 
