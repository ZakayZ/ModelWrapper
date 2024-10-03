cmake_minimum_required(VERSION 3.22)
project(Tester)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

find_package(XMLConfigModule REQUIRED)
find_package(CLHEP REQUIRED)
find_package(Geant4 REQUIRED)

# add_library(FermiWrapper SHARED wrapper.cpp condition.cpp util.cpp)

add_executable(exe main.cpp wrapper.cpp condition.cpp util.cpp)

file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/handler.xml config.xml)

# target_link_libraries(FermiWrapper PUBLIC XMLConfigModule FermiBreakUp ${Geant4_LIBRARIES})
target_link_libraries(exe PUBLIC XMLConfigModule ${Geant4_LIBRARIES})
