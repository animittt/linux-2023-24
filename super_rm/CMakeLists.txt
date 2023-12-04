cmake_minimum_required(VERSION 3.10)
project(zdarova_otec)

set(SOURCES main.cpp super_rm.cpp super_rm.h)
set(CMAKE_CXX_STANDARD 20)

add_executable(super_rm ${SOURCES})
target_include_directories(super_rm PRIVATE ../utils/install/include/)
target_link_directories(super_rm PRIVATE ../utils/install/lib)
target_link_libraries(super_rm PRIVATE libargparser_shared.so liblogging_shared.so)

install(TARGETS super_rm DESTINATION bin/)
