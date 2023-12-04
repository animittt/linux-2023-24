cmake_minimum_required(VERSION 3.10)

project(ArgParser)

set(SOURCES argument_parser.cpp
	argument_parser.h
	argument.h)

add_library(argparser_shared SHARED ${SOURCES})
target_include_directories(argparser_shared PRIVATE .)
target_compile_options(argparser_shared PRIVATE -std=c++17)

add_library(argparser_static STATIC ${SOURCES})
target_include_directories(argparser_static PRIVATE .)
target_compile_options(argparser_static PRIVATE -std=c++17)

install(TARGETS argparser_shared argparser_static  DESTINATION lib/)
install(FILES argument_parser.h argument.h DESTINATION include/)
