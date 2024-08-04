cmake_minimum_required(VERSION 3.22)
project(ConfigLibrary)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

find_package(COLAlib REQUIRED)

set(SRCS
    xml_parser/src/pugixml.cpp
    module.cpp
    stage.cpp
    builders/module_builder.cpp
    builders/xml.cpp
)

add_library(ConfigLib STATIC ${SRCS})

target_link_libraries(ConfigLib PUBLIC COLA)

target_include_directories(ConfigLib PUBLIC ${PROJECT_SOURCE_DIR})

set_target_properties(ConfigLib PROPERTIES POSITION_INDEPENDENT_CODE True)
