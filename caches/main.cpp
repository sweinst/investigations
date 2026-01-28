#include <print>
#include <random>
#include <chrono>
#include <cstring>

void usage(const char* progname, bool error = false) {
    std::println("Usage: {} [-s <number>] [-n <number>] [--help|-h]", progname);
    std::println("  -s <number> the size in power of 2 of the test array (i.e., 20 means 2^20 = 1,048,576 elements) (default: 20)");
    std::println("  -n <number> number of iterations (default: 10,000)");
    std::println("  [-h|--help] show this help message\n");
    std::exit(error ? 1 : 0);
}

using namespace std::chrono_literals;
using sclock = std::chrono::steady_clock;

int64_t array_size = 20;
int64_t n_iterations = 10'000;

void test() {
    int64_t n_elements = 1LL << array_size;
    std::println(stderr, "Allocating array of size 2^{} = {} elements", array_size, n_elements);
    std::unique_ptr<int8_t[]> array(new int8_t[n_elements]);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int64_t> dis(0, n_elements - 1);

    auto start = sclock::now();
    int64_t sum = 0;
    for (int64_t iter = 0; iter < n_iterations; ++iter) {
        #pragma optimize("", off)
        int64_t index = dis(gen);
        auto value = array[index];
        sum += value;
        #pragma optimize("", on)
    }
    auto end = sclock::now();
    auto d = end - start;
    auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
    double avg_ns = static_cast<double>(total_ns) / static_cast<double>(n_iterations);
    std::println(stderr, "Total time for {} iterations: {} ns", n_iterations, total_ns);
    std::println(stderr, "Average time per access: {:.2f} ns", avg_ns);

    std::println(stderr, "OK");
}

int main(int argc, char** argv) {
    for (int i = 0; i < argc; ++i) {
        if (std::strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            array_size = std::strtoll(argv[++i], nullptr, 10);
            if (array_size <= 0 || array_size >= 64) {
                std::print("Invalid array size: {}\n", argv[i]);
                usage(argv[0], true);
            }
        } else if (std::strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            n_iterations = std::strtoll(argv[++i], nullptr, 10);
            if (n_iterations <= 0) {
                std::print("Invalid number of iterations: {}\n", argv[i]);
                usage(argv[0], true);
            }
        } else if ((std::strcmp(argv[i], "--help") == 0) || (std::strcmp(argv[i], "-h") == 0)) {
            usage(argv[0], false);
        }
    }

    try {
        test();
    }
    catch (const std::exception& e) {
        std::println("Error: {}", e.what());
        return 1;
    }
    catch (...) {
        std::println("Unknown error occurred");
        return 1;
    }
    return 0;
}
