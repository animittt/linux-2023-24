cmake_minimum_required(VERSION 3.10)
project(zdarova_otec)
set(CMAKE_CXX_STANDARD 20)
add_executable(super_rm super_rm.cpp
        ../utils/arg_parser/argument_parser.cpp)
target_include_directories(super_rm PRIVATE ../utils)