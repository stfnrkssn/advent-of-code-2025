#include <array>
#include <vector>
#include <cstdint>
#include <iostream>

#include "fast-io.hpp"
#define SAMPLE_INPUT;
constexpr bool DEBUG_FAST_IO = true;

namespace Escalator {
    //Bank size based on the line length in the input data, as they are all equal in length but differ between sample and actual input
#ifndef SAMPLE_INPUT
#define BANK_SIZE 100
#else
#define BANK_SIZE 15
#endif

    struct Bank {

    private:
        struct Battery {
            uint8_t value = 0;
            uint8_t index = 0;
        };

        std::array<Battery, BANK_SIZE> batteries {0};
        uint8_t battery_count = 0;
    public:
        [[nodiscard]] uint64_t get_max_joltage(const size_t active_battery_count) const {
            const auto find_highest_digit = [&](const uint8_t start_position, const uint8_t end_position) -> Battery {
                Battery best{0, start_position};
                for (uint8_t i = start_position; i <= end_position; i++) {
                    if (batteries[i].value > best.value) {
                        best.value = batteries[i].value;
                        best.index = i;
                    }
                }
                return best;
            };

            uint64_t joltage = 0;
            uint8_t search_from = 0;

            for (size_t i = 0; i < active_battery_count; i++) {
                // Leave room for remaining digits after this one
                uint8_t remaining_digits = active_battery_count - i - 1;
                uint8_t search_until = battery_count - 1 - remaining_digits;

                Battery found = find_highest_digit(search_from, search_until);
                joltage = joltage * 10 + found.value;
                search_from = found.index + 1;
            }

            return joltage;
        }

        void print() const {
            for (uint8_t i = 0; i < battery_count; i++) {
                std::cout << static_cast<int>(batteries[i].value);
            }
            std::cout << std::endl;
        }

        bool add_battery(uint8_t joltage) {
            if (battery_count >= BANK_SIZE) {return false;}
            batteries[battery_count] = {joltage, battery_count};
            ++battery_count;
            return true;
        }
    };
}




int main(int argc, char* argv[]) {
    const char* path = nullptr;

    if (argc < 2) {
        std::cout << "Using default data file: ./data.txt" << std::endl;
        path = "./data.txt";
    } else {
        std::cout << "Using supplied data file: " << argv[1] << std::endl;
        path = argv[1];
    }
    uint64_t joltage_one = 0;
    uint64_t joltage_two = 0;
    auto stats = fast_io::read_lines(path, [&](const char* line, size_t len) {
        Escalator::Bank bank;
        if (len < 2) return;
        for (size_t i = 0 ; i < len; i++) {
            bank.add_battery(line[i] - '0'); //damn ascii numbers! fix.
        }
        bank.print();
        joltage_one += bank.get_max_joltage(2);
        joltage_two += bank.get_max_joltage(12);
    },DEBUG_FAST_IO);

    if (!stats) {
        std::cerr << "Can't open file: " << path << std::endl;
        return 1;
    }

    std::cout << joltage_one << std::endl;
    std::cout << joltage_two << std::endl;
    return 0;
}
