cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)
project(checkers)
add_executable(checkers src/main.cpp src/ai/interface.cpp
		src/ai/brain.cpp src/ai/cache.cpp src/communication/console_interface.cpp)
set_property(TARGET checkers PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
