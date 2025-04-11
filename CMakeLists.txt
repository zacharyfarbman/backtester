cmake_minimum_required(VERSION 3.20)
project(backtester LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Compiler Settings ---
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Good practice to avoid compiler extensions

# Add flags for Clang (adjust warning levels as needed)
# Use CMAKE_BUILD_TYPE=Release or Debug when configuring
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror) # Be strict!
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG) # Performance optimizations
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0) # Debug symbols, no optimization
    endif()
endif()

# --- Executable Target ---
add_executable(backtester
    src/main.cpp
    src/DataFeed.cpp
    src/OrderManager.cpp
    src/ExecutionHandler.cpp
    src/strategies/MovingAverageCrossover.cpp
    # Add more source files here later
)

# --- Project Includes ---
# Tell CMake where to find our header files
target_include_directories(backtester PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# --- Dependencies (Placeholder for now) ---
# We will add find_package for Boost and Google Benchmark later

# --- Basic Output ---
message(STATUS "CXX Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}") # Show build type
