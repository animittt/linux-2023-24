cmake_minimum_required(VERSION 3.10)
project(zdarova_otec)

set(SOURCES dir_watcher.cpp)
set(CMAKE_CXX_STANDARD 20)

add_executable(dir_watcher ${SOURCES})
target_include_directories(dir_watcher PRIVATE ../utils/install/include/)
target_link_directories(dir_watcher PRIVATE ../utils/install/lib)
target_link_libraries(dir_watcher PRIVATE libargparser_shared.so liblogging_shared.so)

install(TARGETS dir_watcher DESTINATION bin/)
