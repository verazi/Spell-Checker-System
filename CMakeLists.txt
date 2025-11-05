cmake_minimum_required(VERSION 3.26)

# Detect OS
if (APPLE)
    set(IS_MACOS 1)
else()
    set(IS_MACOS 0)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (IS_MACOS)
    set(ENV{PATH} "/opt/homebrew/bin/:/opt/homebrew/opt/llvm/bin:$ENV{PATH}")
    set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
    set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
    set(ENV{LDFLAGS} "-L/opt/homebrew/opt/llvm/lib -L/opt/homebrew/lib")
    set(ENV{CCFLAGS} "-I/opt/homebrew/opt/llvm/include -I/opt/homebrew/include -Xpreprocessor -fopenmp")
endif()

# Set compiler to mpiCC
#find_program(MPI_CXX_COMPILER mpiCC)
#set(CMAKE_CXX_COMPILER ${MPI_CXX_COMPILER})
#message(STATUS "MPI C++ Compiler: ${MPI_CXX_COMPILER}")

#find_program(MPI_C_COMPILER clang)
#set(CMAKE_C_COMPILER ${MPI_C_COMPILER})
#message(STATUS "MPI C Compiler: ${MPI_C_COMPILER}")


# set compiler to CC=/usr/local/opt/llvm/bin/clang
# exit if mpiCC does not exist
if (NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "mpiCC not found. Please install mpicxx to compile this project.")
endif ()

project(spellcheck_project)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Collect source files
file(GLOB_RECURSE SRC_FILES src/*.cc)
set(MAIN_SRC_FILE spellcheck.cc)
set(TEST_SRC_FILE test/test_main.cc)
set(GTEST_SRCS gtest/src/gtest-all.cc)
set(GMOCK_SRCS gmock/src/gmock-all.cc)

# Create spellcheck executable
add_executable(spellcheck ${SRC_FILES} ${MAIN_SRC_FILE})
target_include_directories(spellcheck PRIVATE include)

# Create test executable
add_executable(test ${SRC_FILES} ${TEST_SRC_FILE} ${GTEST_SRCS} ${GMOCK_SRCS})
target_include_directories(test PRIVATE include gtest/include gtest gmock/include gmock)

# Enable OpenMP
find_package(OpenMP REQUIRED)
if (OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found")
    target_link_libraries(spellcheck PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(test PUBLIC OpenMP::OpenMP_CXX)
else()
    message(FATAL_ERROR "OpenMP not found. Please install OpenMP or ensure your compiler supports it.")
endif()

## Find MPI
#find_package(MPI REQUIRED)
#if (MPI_CXX_FOUND)
#    target_include_directories(spellcheck PUBLIC ${MPI_CXX_INCLUDE_PATH})
#    target_link_libraries(spellcheck PUBLIC MPI::MPI_CXX)
#    target_include_directories(test PUBLIC ${MPI_CXX_INCLUDE_PATH})
#    target_link_libraries(test PUBLIC MPI::MPI_CXX)
#else()
#    message(FATAL_ERROR "MPI not found. Please install MPI.")
#endif()

# Link stdc++ if necessary
target_link_libraries(spellcheck PUBLIC stdc++)
target_link_libraries(test PUBLIC stdc++)

# Rest of your CMakeLists.txt remains the same...

# Find clang-format
find_program(CLANG_FORMAT_PATH clang-format)
if (CLANG_FORMAT_PATH)
    add_custom_target(format
            COMMAND ${CLANG_FORMAT_PATH} -i ${SRC_FILES} ${MAIN_SRC_FILE} ${TEST_SRC_FILE}
            COMMENT "Running clang-format"
    )
else()
    message(WARNING "clang-format not found. Please install clang-format to run the 'format' target.")
endif()

# Archive target
set(VER "" CACHE STRING "Version number for the archive")
add_custom_target(archive
        COMMAND git archive --format=tar.gz --prefix=skeleton-p2/ -o comp90025-skeleton-p2-v${VER}.tar.gz HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating archive with version ${VER}"
)
