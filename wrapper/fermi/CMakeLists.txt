cmake_minimum_required(VERSION 3.22)
project(Tester)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

find_package(XMLConfigModule REQUIRED)
find_package(FermiBreakUp REQUIRED)
find_package(CLHEP REQUIRED)

# add_executable(exec main.cpp wrapper.cpp)

add_library(FermiWrapper SHARED wrapper.cpp)

add_executable(exe main.cpp wrapper.cpp)

target_link_libraries(FermiWrapper PUBLIC XMLConfigModule FermiBreakUp)
target_link_libraries(exe PUBLIC XMLConfigModule FermiBreakUp)
