cmake_minimum_required(VERSION 3.16)

project(practice_assignment_3)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

#find_package(mosquitto REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_check_modules(Mosquitto REQUIRED IMPORTED_TARGET libmosquitto)

include_directories(./dependencies)

add_executable(hw ./src/main.cpp)

#set_target_properties(hw PROPERTIES COMPILE_FLAGS "-Wall -Werror -Wpedantic -Wextra -pedantic-errors -Wold-style-cast -O3 -std=c++17")

target_link_libraries(hw ${OpenCV_LIBS} PkgConfig::Mosquitto)