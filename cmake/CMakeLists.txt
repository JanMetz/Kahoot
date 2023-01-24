cmake_minimum_required(VERSION 3.13.4)

project(kahoot)

set(CMAKE_CXX_STANDARD 17)

add_executable(kahoot.exe main.cpp game.cpp questions.cpp server.cpp)

#target_link_libraries(kahoot client.hpp game.hpp player.hpp questions.hpp server.hpp)
#target_link_directories(kahoot PRIVATE master/public)

#include_directories(public)
