cmake_minimum_required(VERSION 3.22)
project(COLA_XMLConfigHandler VERSION 1.1)

# c++17 is recommended
set(CMAKE_CXX_STANDARD 17)

# Find COLA package
find_package(COLA)

# Include CMake module for config file generation
include(CMakePackageConfigHelpers)

# Modules can be installed whenever you please, however grouping them all in COLA directory is neat and
# makes further adjustments to CMAKE_PREFIX_PATH unnecessary. It is also advised to put module files to corresponding
# directories to avoid pollution.
set(CMAKE_INSTALL_PREFIX ${COLA_DIR})
set(COLA_MODULE_NAME XMLConfigHandler)

# Add library target
set(srcs 
        src/factory.cpp
        src/filter.cpp
        src/registry.cpp
)

add_library(XMLConfigHandler SHARED ${srcs})
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)

target_include_directories(XMLConfigHandler PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/${COLA_MODULE_NAME}>)

# Set public header
set_target_properties(XMLConfigHandler PROPERTIES PUBLIC_HEADER include/config_module.h)

# Link against COLA
target_link_libraries(XMLConfigHandler PRIVATE ConfigLib)
target_link_libraries(XMLConfigHandler PUBLIC COLA)

# Configure config files
configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/data/XMLConfigHandler.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/XMLConfigHandler.cmake"
        INSTALL_DESTINATION lib/cmake/${COLA_MODULE_NAME}
        #PATH_VARS, etc.
        )

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/XMLConfigHandlerVersion.cmake
        COMPATIBILITY AnyNewerVersion)

# Install library
install(TARGETS XMLConfigHandler
        EXPORT XMLConfigHandlerExport
        LIBRARY DESTINATION lib/${COLA_MODULE_NAME}
        PUBLIC_HEADER DESTINATION include/${COLA_MODULE_NAME}
        INCLUDES DESTINATION include/${COLA_MODULE_NAME})

# Install includes
install(DIRECTORY include/
        DESTINATION include/${COLA_MODULE_NAME})

# Install export file and config files
install(EXPORT XMLConfigHandlerExport
        DESTINATION lib/cmake/${COLA_MODULE_NAME})

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/XMLConfigHandler.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/XMLConfigHandlerVersion.cmake"
        DESTINATION lib/cmake/${COLA_MODULE_NAME})
