cmake_minimum_required(VERSION 3.10)
project(project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Enable compiler warnings and treat warnings as errors
if (MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -Werror)
endif()

# Add default directory as include directory (allows #include <file.h>)
include_directories(${CMAKE_SOURCE_DIR}/default)

# Recursively gather all .c files from src/
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.c)

# Recursively gather all .c files from default/
file(GLOB_RECURSE DEFAULT_FILES ${CMAKE_SOURCE_DIR}/default/*.c)

# Create executable with all source files
add_executable(${PROJECT_NAME} ${SRC_FILES} ${DEFAULT_FILES})