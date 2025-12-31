#ifndef UTILS_FAST_IO_HPP
#define UTILS_FAST_IO_HPP

#include <cstdint>
#include <cstring>
#include <iostream>
#include <chrono>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

//utility for reading input files. General purpose. Not part of the solutions as such.
namespace fast_io {

struct ReadStats {
    size_t file_size = 0;
    size_t line_count = 0;
    double parse_time_ms = 0.0;
};

namespace detail {
    struct MappedFile {
        const char* data = nullptr;
        size_t size = 0;

#ifdef _WIN32
        HANDLE file_handle = INVALID_HANDLE_VALUE;
        HANDLE mapping_handle = nullptr;
#else
        int fd = -1;
#endif

        bool open(const char* path) {
#ifdef _WIN32
            file_handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ,
                nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file_handle == INVALID_HANDLE_VALUE) return false;

            LARGE_INTEGER file_size;
            if (!GetFileSizeEx(file_handle, &file_size)) { close(); return false; }
            size = static_cast<size_t>(file_size.QuadPart);

            if (size == 0) return true;  // empty file is valid

            mapping_handle = CreateFileMappingA(file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
            if (!mapping_handle) { close(); return false; }

            data = static_cast<const char*>(MapViewOfFile(mapping_handle, FILE_MAP_READ, 0, 0, 0));
            if (!data) { close(); return false; }
#else
            fd = ::open(path, O_RDONLY);
            if (fd < 0) return false;

            struct stat st;
            if (fstat(fd, &st) < 0) { close(); return false; }
            size = static_cast<size_t>(st.st_size);

            if (size == 0) return true;

            data = static_cast<const char*>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
            if (data == MAP_FAILED) { data = nullptr; close(); return false; }

            // Hint to OS we'll read sequentially
            madvise(const_cast<char*>(data), size, MADV_SEQUENTIAL);
#endif
            return true;
        }

        void close() {
#ifdef _WIN32
            if (data) { UnmapViewOfFile(data); data = nullptr; }
            if (mapping_handle) { CloseHandle(mapping_handle); mapping_handle = nullptr; }
            if (file_handle != INVALID_HANDLE_VALUE) { CloseHandle(file_handle); file_handle = INVALID_HANDLE_VALUE; }
#else
            if (data && size > 0) { munmap(const_cast<char*>(data), size); data = nullptr; }
            if (fd >= 0) { ::close(fd); fd = -1; }
#endif
        }

        ~MappedFile() { close(); }

        MappedFile() = default;
        MappedFile(const MappedFile&) = delete;
        MappedFile& operator=(const MappedFile&) = delete;
    };
}

// Line parser must implement: void operator()(const char* line_start, size_t line_length)
template<typename LineParser>
std::optional<ReadStats> read_lines(const char* path, LineParser&& parser, bool debug = false) {
    ReadStats stats;
    auto start_time = std::chrono::high_resolution_clock::now();

    detail::MappedFile file;
    if (!file.open(path)) {
        if (debug) std::cerr << "[fast_io] Failed to open: " << path << '\n';
        return std::nullopt;
    }

    stats.file_size = file.size;

    if (file.size == 0) {
        if (debug) std::cout << "[fast_io] Empty file\n";
        return stats;  // valid empty file
    }

    const char* ptr = file.data;
    const char* end = file.data + file.size;
    const char* line_start = ptr;

    while (ptr < end) {
        if (*ptr == '\n' || *ptr == '\r') {
            size_t line_len = ptr - line_start;
            if (line_len > 0) {
                parser(line_start, line_len);
                ++stats.line_count;
            }

            if (ptr + 1 < end && (ptr[1] == '\n' || ptr[1] == '\r') && ptr[0] != ptr[1]) {
                ++ptr;
            }
            line_start = ptr + 1;
        }
        ++ptr;
    }

    if (line_start < end) {
        parser(line_start, end - line_start);
        ++stats.line_count;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    stats.parse_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    if (debug) {
        std::cout << "[fast_io] File: " << path << '\n'
                  << "[fast_io] Size: " << stats.file_size << " bytes\n"
                  << "[fast_io] Lines: " << stats.line_count << '\n'
                  << "[fast_io] Time: " << stats.parse_time_ms << " ms\n";
    }

    return stats;
}

// Convenience: parse integers with optional prefix char
inline int parse_int(const char* start, size_t len) {
    int value = 0;
    for (size_t i = 0; i < len; ++i) {
        char c = start[i];
        if (c >= '0' && c <= '9') {
            value = value * 10 + (c - '0');
        }
    }
    return value;
}

}  // namespace fast_io



#endif