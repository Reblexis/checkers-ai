cmake_minimum_required(VERSION 3.14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)
project(checkers)
add_executable(checkers ./main.cpp ./interface.cpp ./minimax.cpp)