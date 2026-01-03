
#include <array>
#include <bitset>
#include <cstdint>
#include <functional>

#include "fast-io.hpp"

constexpr bool DEBUG_FAST_IO = true;
constexpr size_t EXPECTED_MAX_GRID_SIZE = 8096;

namespace Grid {

    struct GridDef {
        int width = 0;
        int height = 0;
    };

    struct GridCoord {
        int x = 0;
        int y = 0;
    };

    using GridIndex = int;

    GridIndex coord_to_index(const GridDef& def, const GridCoord& coord) {
        return coord.x + def.width * coord.y;
    }

    GridCoord index_to_coord(const GridDef& def, GridIndex index) {
        return {index % static_cast<int>(def.width), index / static_cast<int>(def.width)};
    }

    inline bool is_valid(const GridDef& def, const GridCoord& coord) {
        return coord.x >= 0 && coord.x < def.width && coord.y >= 0 && coord.y < def.height;
    }

    inline bool is_valid(const GridDef& def, const GridIndex index) {
        return index >= 0 && index < (def.width * def.height);
    }

    template<typename Func>
    void for_each_neighbor(const GridDef& def, const GridCoord& coord, Func&& func) {
        constexpr GridCoord offsets[8] {
            {-1,0},
            {-1,1},
            {0, 1},
            {1, 1},
            {1, 0},
            {1,-1},
            {0,-1},
            {-1,-1}
        };
        for (auto offset : offsets) {
            GridCoord neighbor_coord{.x = coord.x + offset.x, .y = coord.y + offset.y};
            if (is_valid(def, neighbor_coord)) {
                func(coord_to_index(def, neighbor_coord));
            }
        }
    }

    void for_each_neighbor(const GridDef& def, const GridIndex index, std::function<void(const GridIndex)>&& func) {
        if (is_valid(def, index)) {
            GridCoord neighbor_coord = index_to_coord(def, index);
            for_each_neighbor(def, neighbor_coord, std::move(func));
        }
    }
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

    std::vector<bool> grid;
    grid.reserve(EXPECTED_MAX_GRID_SIZE); // should be enough.
    Grid::GridDef def;
    auto stats = fast_io::read_lines(
        path,
        [&](const char* line, size_t len) {
            if (def.width == 0) {
                def.width = static_cast<unsigned int>(len);
            }
            def.height++;
            for (size_t i = 0; i < len; i++) {
                grid.push_back(line[i] == '@');
            }
        },
        DEBUG_FAST_IO);

    if (!stats) {
        std::cerr << "Can't open file: " << path << std::endl;
        return 1;
    }

    unsigned int movable_count = 0;
    for (size_t i = 0; i < grid.size(); i++) {
        if (!grid[i])
            continue; //count only for cells with a roll in it.

        uint8_t neighbor_count = 0;
        Grid::for_each_neighbor(def,static_cast<Grid::GridIndex>(i),[&](const Grid::GridIndex index) {
            if (grid[index] == true)
                neighbor_count++;
        });
        if (neighbor_count < 4)
            movable_count++;
    }

    std::cout << movable_count << std::endl; //Part 1.

    unsigned int removed_count = movable_count;
    movable_count = 0;

    while (removed_count > 0) {
        removed_count = 0;
        for (size_t i = 0; i < grid.size(); i++) {
            if (!grid[i])
                continue;

            uint8_t neighbor_count = 0;
            Grid::for_each_neighbor(def, static_cast<Grid::GridIndex>(i), [&](const Grid::GridIndex index) {
                if (grid[index] == true)
                    neighbor_count++;
            });

            if (neighbor_count < 4) {
                removed_count++;
                grid[i] = false; //remove the roll.
            }
        }
        movable_count += removed_count;
    }

    std::cout << movable_count << std::endl; //Part 2

    return 0;
}