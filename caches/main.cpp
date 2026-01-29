#include <chrono>
#include <cstring>
#include <print>
#include <random>

using namespace std::chrono_literals;
using sclock = std::chrono::steady_clock;

uint64_t array_size = 20;
uint64_t n_iterations = 250'000'000;

uint64_t read_uint64(const char* txt) {
    try {
        return std::strtoll(txt, nullptr, 10);
    } catch (const std::exception&) {
        return -1;
    }
}

void usage(const char* progname, bool error = false) {
    std::println("Usage: {} [-s <number>] [-n <number>] [--help|-h]", progname);
    std::println(
        "  -s <number> the size in power of 2 of the test array (i.e., 20 means 2^20 = 1,048,576 elements) (default: 20)"
    );
    std::println("  -n <number> number of iterations (default: 10,000)");
    std::println("  [-h|--help] show this help message\n");
    std::exit(error ? 1 : 0);
}

void test() {
    uint64_t n_elements = 1ULL << array_size;
    std::println(stderr, "Allocating array of size 2^{} = {} elements", array_size, n_elements);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int64_t> dis(0, n_elements - 1);

    std::vector<int8_t> array(n_elements, 0);

    auto start = sclock::now();
#pragma optimize("", off)
    uint64_t sum = 0;
    for (uint64_t iter = 0; iter < n_iterations; ++iter) {
        uint64_t index = dis(gen);
        auto value = array[index];
        sum += value;
    }
    sum += 1;
#pragma optimize("", on)
    auto end = sclock::now();
    auto d = end - start;
    auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
    double avg_ns = static_cast<double>(total_ns) / static_cast<double>(n_iterations);
    // std::println(stderr, "Total time for {} iterations: {} ns", n_iterations, total_ns);
    // std::println(stderr, "Average time per access: {:.2f} ns", avg_ns);
    std::print("{}", avg_ns);
}

int main(int argc, char** argv) {
    for (int i = 0; i < argc; ++i) {
        if (std::strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            array_size = read_uint64(argv[++i]);
            if (array_size <= 0 || array_size >= 64) {
                std::print("Invalid array size: {}\n", argv[i]);
                usage(argv[0], true);
            }
        } else if (std::strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            n_iterations = read_uint64(argv[++i]);
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
    } catch (const std::exception& e) {
        std::println("Error: {}", e.what());
        return 1;
    } catch (...) {
        std::println("Unknown error occurred");
        return 1;
    }
    return 0;
}
