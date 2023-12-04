cmake_minimum_required(VERSION 3.10)

project(Logging)

set(SOURCES logger.h logger.cpp)

add_library(logging_shared SHARED ${SOURCES})
target_include_directories(logging_shared PRIVATE .)

add_library(logging_static STATIC ${SOURCES})
target_include_directories(logging_static PRIVATE .)

install(TARGETS logging_shared logging_static DESTINATION lib/)
install(FILES logger.h DESTINATION include/)
