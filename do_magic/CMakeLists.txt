cmake_minimum_required(VERSION 3.10)
project(zdarova_otec)

set(SOURCES main.cpp do_magic.hpp do_magic.cpp)
set(CMAKE_CXX_STANDARD 20)

add_executable(do_magic ${SOURCES})
target_include_directories(do_magic PRIVATE ../utils/install/include/)
target_link_directories(do_magic PRIVATE ../utils/install/lib)
target_link_libraries(do_magic PRIVATE libargparser_shared.so liblogging_shared.so)

install(TARGETS do_magic DESTINATION bin/)
