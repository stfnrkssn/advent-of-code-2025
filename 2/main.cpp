#include <iostream>
#include <stdint.h>
#include "fast-io.hpp"

constexpr bool DEBUG_FAST_IO = true;

struct IntRange {
    uint64_t first;
    uint64_t last;

    struct Iterator {
        uint64_t value;
        int64_t step;

        uint64_t  operator*() const { return value; }
        Iterator& operator++() { value += step; return *this; }
        bool operator!=(const Iterator& other) const { return value != other.value; }
    };

    [[nodiscard]] Iterator begin() const { return {first, first <= last ? 1 : -1}; }
    [[nodiscard]] Iterator end() const { return {first <= last ? last + 1 : last - 1, 0}; }

    [[nodiscard]] size_t size() const { return last - first + 1; }
};

inline bool is_valid_id(uint64_t id) {
    uint64_t temp = id;
    uint64_t digits = 0;
    while (temp > 0) {
        ++digits;
        temp /= 10;
    }

    if (digits % 2 != 0) return true; //no even number of digits, must be valid.

    const uint64_t half = digits / 2;
    uint64_t divisor = 1;
    for (uint64_t i = 0; i < half; ++i) divisor *= 10;

    return (id / divisor) != (id % divisor);
}

inline IntRange parse_range(const char* start, size_t len) {
    constexpr char delimiter = '-';
    uint64_t first = 0;
    uint64_t second = 0;
    size_t i = 0;

    while (i < len && start[i] != delimiter) {
        first = first * 10 + (start[i] - '0');
        ++i;
    }

    ++i; // skip delimiter

    while (i < len) {
        second = second * 10 + (start[i] - '0');
        ++i;
    }

    return {first, second};
}

int main(int argc, char* argv[]) {
    const char* path;

    if (argc < 2) {
        std::cout << "Using default data file: ./data.txt" << std::endl;
        path = "./data.txt";
    } else {
        std::cout << "Using supplied data file: " << argv[1] << std::endl;
        path = argv[1];
    }


    uint64_t invalid_ids = 0;
    const auto stats = fast_io::read_csv(
        path,
        [&](const char* line, const size_t len) {
            for (const IntRange int_range = parse_range(line, len); const uint64_t i : int_range) {
                if (!is_valid_id(i)) {
                    std::cout << "invalid index: " << i << " from range: " << int_range.first << "-" << int_range.last << std::endl;
                    invalid_ids += i;
                }

            }
        },
        DEBUG_FAST_IO);

    if (!stats) {
        std::cerr << "Can't open file: " << path << std::endl;
        return 1;
    }

    std::cout << invalid_ids << std::endl;

    return 0;
}
