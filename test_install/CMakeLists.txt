cmake_minimum_required(VERSION 3.22)
project(XMLConfigHandlerTest)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

find_package(XMLConfigModule REQUIRED)
add_executable(EXE main.cpp)

target_link_libraries(EXE PUBLIC XMLConfigModule)
