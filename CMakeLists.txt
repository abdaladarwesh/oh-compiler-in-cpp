cmake_minimum_required(VERSION 3.10)
project(Tokenizer)



add_executable(ohc src/main.cpp src/Tokenizer.hpp src/parser.hpp src/Generator.hpp)
set(CMAKE_CXX_STANDARD 20)

set_target_properties(ohc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/debug"
)