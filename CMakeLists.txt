cmake_minimum_required(VERSION 3.22)
project(COLA_XMLConfigHandler VERSION 1.0)

# Include CMake module for config file generation
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

# Modules can be installed whenever you please, however grouping them all in COLA directory is neat and
# makes further adjustments to CMAKE_PREFIX_PATH unnecessary. It is also advised to put module files to corresponding
# directories to avoid pollution.
set(CMAKE_INSTALL_PREFIX ${COLA_DIR})
set(COLA_MODULE_NAME XMLConfigModule)

find_package(COLA REQUIRED)

set(SRCS
    lib/xml_parser/src/pugixml.cpp
    lib/config/module.cpp
    lib/config/stage.cpp
    lib/builders/module.cpp
    lib/builders/xml.cpp

    src/factory.cpp
    src/filter.cpp
    src/registry.cpp
)

add_library(XMLConfigModule SHARED ${SRCS})

target_include_directories(XMLConfigModule PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/${COLA_MODULE_NAME}>
)

# Set public header
set_target_properties(XMLConfigModule PROPERTIES PUBLIC_HEADER include/config_module.h)

# Link against COLA
target_link_libraries(XMLConfigModule PUBLIC COLA)

# Configure config files
configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/data/XMLConfigModuleConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/XMLConfigModuleConfig.cmake"
        INSTALL_DESTINATION lib/cmake/${COLA_MODULE_NAME}
        #PATH_VARS, etc.
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/XMLConfigModuleConfigVersion.cmake
        COMPATIBILITY AnyNewerVersion
)

# Install library
install(TARGETS XMLConfigModule
        EXPORT XMLConfigModuleExport
        LIBRARY DESTINATION lib/${COLA_MODULE_NAME}
        PUBLIC_HEADER DESTINATION include/${COLA_MODULE_NAME}
        INCLUDES DESTINATION include/${COLA_MODULE_NAME}
)

# Install includes
install(DIRECTORY include/
        DESTINATION include/${COLA_MODULE_NAME}
)

# Install config lib
install(DIRECTORY lib/ DESTINATION include/${COLA_MODULE_NAME}/lib
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "lib/builders" EXCLUDE
        PATTERN "lib/xml_parser" EXCLUDE
)

# Install export file and config files
install(EXPORT XMLConfigModuleExport
        DESTINATION lib/cmake/${COLA_MODULE_NAME}
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/XMLConfigModuleConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/XMLConfigModuleConfigVersion.cmake"
        DESTINATION lib/cmake/${COLA_MODULE_NAME}
)
