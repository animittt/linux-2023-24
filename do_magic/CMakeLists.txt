cmake_minimum_required(VERSION 3.10)
project(zdarova_otec)
set(CMAKE_CXX_STANDARD 20)
add_executable(magic do_magic.hpp
        ../utils/arg_parser/argument_parser.cpp
        main.cpp)
target_include_directories(magic PRIVATE ../utils)