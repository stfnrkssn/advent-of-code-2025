#include <array>
#include <vector>
#include <cstdint>
#include <iostream>

#include "fast-io.hpp"
//#define SAMPLE_INPUT;
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
        [[nodiscard]] uint8_t get_max_joltage() const {
            uint8_t first = 0;
            uint8_t second = 0;
            for (uint8_t i = 0; i < battery_count-1; i++) {
                if (batteries[i].value > first) {
                    first = batteries[i].value;
                    second = 0;
                    for (uint8_t y = i+1; y < battery_count; y++) {
                        if (batteries[y].value > second) {
                            second = batteries[y].value;
                        }
                    }
                }
            }
            const uint8_t max = (first*10)+second;
            std::cout << static_cast<int>(max) << std::endl;
            return max;
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
    uint64_t joltage = 0;
    auto stats = fast_io::read_lines(path, [&](const char* line, size_t len) {
        Escalator::Bank bank;
        if (len < 2) return;
        for (size_t i = 0 ; i < len; i++) {
            bank.add_battery(line[i] - '0'); //damn ascii numbers! fix.
        }
        bank.print();
        joltage += bank.get_max_joltage();
    },DEBUG_FAST_IO);

    if (!stats) {
        std::cerr << "Can't open file: " << path << std::endl;
        return 1;
    }

    std::cout << joltage << std::endl;
    return 0;
}
