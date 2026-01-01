#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <array>
#include <cmath>
#include <vector>
#include <fast-io.hpp>

constexpr size_t BUFFER_SIZE = 16*1024;
constexpr size_t INITIAL_INSTRUCTION_CAPACITY = 8096;
constexpr bool DEBUG_FAST_IO = true;
constexpr int DIAL_START_POSITION = 50;

inline int floor_div_100(int value){
    return (value > 0) ? value / 100 : (value - 99) / 100;
}

size_t count_zeros_passed(int dial_position, int instruction) {
    if (instruction > 0) {
        return floor_div_100(dial_position + instruction) - floor_div_100(dial_position);
    } else if (instruction < 0) {
        return floor_div_100(dial_position - 1) - floor_div_100(dial_position + instruction - 1);
    }
    return 0;
}


int main(int argc, char* argv[]){
    const char* path = nullptr;

    if (argc < 2) {
        std::cout << "Using default data file: ./data.txt" << std::endl;
        path = "./data.txt";
    } else {
        std::cout << "Using supplied data file: " << argv[1] << std::endl;
        path = argv[1];
    }

    size_t zeros_stops = 0; //part 1
    size_t zeros_passed = 0; //part 2
    int dial_position = DIAL_START_POSITION;

    auto stats = fast_io::read_lines(
        path,
        [&](const char* line, size_t len) {
            if (len < 2) return;

            int instruction = fast_io::parse_int(line + 1, len - 1);

            int multiplier = 0;
            if (line[0] == 'R') {
                multiplier = 1;
            } else if (line[0] == 'L') {
                multiplier = -1;
            }
            else {
                return;
            }

            instruction *= multiplier;

            zeros_passed += count_zeros_passed(dial_position, instruction); //solution part 2.

            int total = dial_position + instruction;
            dial_position = ((total % 100) + 100) % 100; // wrap to 0-99

            if (dial_position == 0)
                zeros_stops++; //solution for part 1
        },
        DEBUG_FAST_IO);

    if (!stats) {
        std::cerr << "Can't open file: " << path << std::endl;
        return 1;
    }

    std::cout << "zeros: " << zeros_stops << std::endl;
    std::cout << "wraps: " << zeros_passed << std::endl;

    return 0;
}
