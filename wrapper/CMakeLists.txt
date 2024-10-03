cmake_minimum_required(VERSION 3.22)
project(WrapperLib)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

add_subdirectory(fermi)
add_subdirectory(geant4)
