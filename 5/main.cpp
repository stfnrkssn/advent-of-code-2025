

#include <iostream>
#include <unordered_set>

#include "fast-io.hpp"

namespace Inventory {

    using IdType = uint64_t;

    template<typename IntType>
    inline std::pair<IntType, IntType> parse_pair(const char* str, size_t len, char delimiter = '-') {
        IntType first = 0;
        IntType second = 0;
        size_t i = 0;

        while (i < len && str[i] != delimiter) {
            first = first * 10 + (str[i] - '0');
            ++i;
        }

        ++i; // skip delimiter

        while (i < len) {
            second = second * 10 + (str[i] - '0');
            ++i;
        }

        return {first, second};
    }

    inline bool is_empty_line(size_t len) {
        return len == 0;
    }

    inline bool is_blank_line(const char* line, size_t len) {
        if (len == 0) return true;
        for (size_t i = 0; i < len; i++) {
            if (line[i] != ' ' && line[i] != '\t') return false;
        }
        return true;
    }

    struct IdRange {
        IdType first = 0;
        IdType last = 0;

        [[nodiscard]] bool contains(IdType id) const {
            return first <= id && id <= last;
        }

        bool operator<(const IdRange& other) const {
            return first < other.first;
        }

        [[nodiscard]] uint64_t size() const {
            return static_cast<uint64_t>( last - first + 1); //inclusive count
        }
    };

}


constexpr bool DEBUG_FAST_IO = true;


int main(int argc, char* argv[]) {
    const char* path = nullptr;

    if (argc < 2) {
        std::cout << "Using default data file: ./data.txt" << std::endl;
        path = "./data.txt";
    } else {
        std::cout << "Using supplied data file: " << argv[1] << std::endl;
        path = argv[1];
    }



    //actually add to the fresh_ids map instead of using a range data structure - suspicious of part 2 coming up...
    std::vector<Inventory::IdRange> fresh_ids;
    fresh_ids.reserve(8096); //no idea about the size of the input data, but should be enough to avoid reallocation
    std::vector<Inventory::IdType> ids;
    ids.reserve(8096);

    auto stats = fast_io::read_lines(
        path,
        [&](const char* line, size_t len) {
            // fast_io line parsing skips empty lines - so detect type of input based on pattern.
            bool is_range = false;
            for (size_t i = 0; i < len; i++) {
                if (line[i] == '-') {
                    is_range = true;
                    break;
                }
            }

            if (is_range) {
                const auto [first, last] = Inventory::parse_pair<Inventory::IdType>(line, len);
                fresh_ids.push_back({first, last});
            } else {
                const auto id = fast_io::parse_int<Inventory::IdType>(line, len);
                ids.push_back(id);
            }
        },
        DEBUG_FAST_IO);

    if (!stats) {
        std::cerr << "Can't open file: " << path << std::endl;
        return 1;
    }

    std::cout << "fresh id ranges (pre merge): " << fresh_ids.size() << std::endl;

    // sort our fresh id ranges and then merge them for faster lookups
    {
        std::sort(fresh_ids.begin(), fresh_ids.end(), [](const auto& a, const auto& b) {
           return a.first < b.first;
       });

        std::vector<Inventory::IdRange> merged;
        merged.reserve(fresh_ids.size());
        for (const auto& range : fresh_ids) {
            if (merged.empty() || merged.back().last + 1 < range.first) {
                merged.push_back(range);
            } else {
                merged.back().last = std::max(merged.back().last, range.last);
            }
        }
        fresh_ids = std::move(merged);
    }

    std::cout << "fresh id ranges (post merge): " << fresh_ids.size() << std::endl;
    std::cout << "number of active ids: " << ids.size() << std::endl;

    auto is_fresh = [&](const Inventory::IdType& id) -> bool {
        //find first range where start > id
        auto it = std::upper_bound(fresh_ids.begin(), fresh_ids.end(), id,
            [](const Inventory::IdType val, const Inventory::IdRange& range) {
                return val < range.first;
            });

        //check the range before (if exists) - it has start <= id
        if (it != fresh_ids.begin()) {
            --it;
            if (id <= it->last) return true;
        }
        return false;
    };


    unsigned int fresh_count = 0;
    for (const Inventory::IdType& id : ids) {
        if (is_fresh(id))
            ++fresh_count;
    }

    uint64_t total_range_size = 0;
    for (const auto& range : fresh_ids) {
        total_range_size += range.size();
    }

    std::cout << "fresh ingredients :" <<fresh_count << std::endl;
    std::cout << "total range size: " << total_range_size << std::endl;
    return 0;
}
