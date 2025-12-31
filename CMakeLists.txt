cmake_minimum_required(VERSION 3.20)
project(Problems)

set(CMAKE_CXX_STANDARD 20)

# Make utils available to all targets
include_directories(${CMAKE_SOURCE_DIR}/utils)

foreach(i RANGE 1 24)
    if(EXISTS "${CMAKE_SOURCE_DIR}/${i}/main.cpp")
        add_executable(day_${i} ${i}/main.cpp)

        # Copy data files to build directory
        file(GLOB DATA_FILES "${CMAKE_SOURCE_DIR}/${i}/*")
        list(FILTER DATA_FILES EXCLUDE REGEX ".*\\.cpp$")

        foreach(DATA_FILE ${DATA_FILES})
            get_filename_component(FILENAME ${DATA_FILE} NAME)
            configure_file(${DATA_FILE} ${CMAKE_BINARY_DIR}/${i}/${FILENAME} COPYONLY)
        endforeach()

        set_target_properties(day_${i} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${i}"
        )
    endif()
endforeach()