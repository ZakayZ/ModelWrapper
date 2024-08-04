cmake_minimum_required(VERSION 3.22)
project(ConfigLibraryTest)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

# Config library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_BINARY_DIR}/ConfigLib)

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

set(Tests
        test_builder.cpp
        test_condition.cpp
        test_default.cpp
        test_registry.cpp
)

file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/example.xml config.xml)

add_executable(ConfigTest ${Tests})

target_link_libraries(ConfigTest ConfigLib)
target_link_libraries(ConfigTest GTest::GTest GTest::Main)

gtest_discover_tests(ConfigTest)
