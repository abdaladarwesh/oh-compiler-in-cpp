cmake_minimum_required(VERSION 3.10)
project(Tokenizer)
set(CMAKE_BUILD_TYPE Debug)



set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Optional: Makes C++23 a hard requirement
add_executable(ohc src/main.cpp src/Tokenizer.hpp src/parser.hpp src/Generator.hpp src/arena.hpp)

set_target_properties(ohc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/debug"
)