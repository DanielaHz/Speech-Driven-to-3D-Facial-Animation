# Project plugin name
project(Retargeting-Plugin)

# Qt configuration 
set(CMAKE_PREFIX_PATH "~/Qt/6.5.3/gcc_64/lib/cmake")

set(CMAKE_AUTOMOC ON)
find_package(Qt6 COMPONENTS OpenGL Widgets QUIET)
if (Qt6_FOUND)
    message("Qt6 founded")
else()
    message("Qt6 not founded, using Qt5")
endif()

# GLM
find_package(glm CONFIG REQUIRED)

# tinyobjloader Config
find_package(tinyobjloader CONFIG REQUIRED)

# Maya Devkit
if(NOT DEVKIT_LOCATION)
    message(FATAL_ERROR "DEVKIT_LOCATION it's not defined use -DDEVKIT_LOCATION=...")
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)
qt6_add_resources(RESOURCES static/resources.qrc)
 
set(DEVKIT_INCLUDE "${DEVKIT_LOCATION}/include")
set(DEVKIT_LIB "${DEVKIT_LOCATION}/lib")

# Plugin Source files
file(GLOB_RECURSE PLUGIN_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/pluginEntry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/interface/widgets/PixelMuxPluginWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DCCInterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MayaMesh.cpp
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/src  
)

## Plugin Header files 
file(GLOB_RECURSE PLUGIN_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/interface/widgets/PixelMuxPluginWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/DCCInterface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MayaMesh.h
)

# local headers
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/include  
    ${PROJECT_SOURCE_DIR}/interface/widgets
    ${PROJECT_SOURCE_DIR}/models
)

# Devkit macro to build the plugin 
include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

# Devkit libraries
set(LIBRARIES OpenMaya Foundation OpenMayaAnim)

# Files to compile the plugin 
set(SOURCE_FILES ${PLUGIN_SRC} ${PLUGIN_HEADERS})

# Calling the build plugin 
build_plugin()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/models
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/models
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/landmarks-data
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/landmarks-data
)

# To make sure the plugin is compiling with c++17
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

target_link_libraries(${PROJECT_NAME} retargeting_lib glm::glm Qt6::Core Qt6::Widgets Qt6::Gui tinyobjloader::tinyobjloader)