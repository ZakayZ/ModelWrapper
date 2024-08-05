cmake_minimum_required(VERSION 3.22)
project(ConfigInterfaceLibraryTest)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

# Config library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_BINARY_DIR}/ConfigInterfaceLib)

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

set(Tests
        test.cpp
        test_builder.cpp
        test_condition.cpp
        test_default.cpp
        test_registry.cpp
)

file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/example.xml config.xml)

add_executable(ConfigInterfaceTest ${Tests})

target_link_libraries(ConfigInterfaceTest XMLConfigModule)
target_link_libraries(ConfigInterfaceTest GTest::GTest GTest::Main)

gtest_discover_tests(ConfigInterfaceTest)
