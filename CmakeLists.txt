cmake_minimum_required(VERSION 3.25) # C++23 support is better in 3.25+
project(Lifeboat LANGUAGES CXX)

# Set the C++ standard to 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Use "Pure" C++23 without compiler-specific extensions

# Rename the source file to .cpp in your project structure
add_executable(lifeboat src/main.cpp)

# Add Homebrew paths for Mac (Modern way: use CMAKE_PREFIX_PATH if possible)
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

# Find packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Link everything to the C++ target
target_link_libraries(lifeboat PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
)

# Asset Copying (Unchanged logic, updated for clarity)
add_custom_command(TARGET lifeboat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/public"
    "${CMAKE_BINARY_DIR}/public"
    COMMENT "Copying assets to build directory..."
)
