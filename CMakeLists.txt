cmake_minimum_required(VERSION 3.15)
project(kMenu)

add_library(kMenu INTERFACE)

target_include_directories(kMenu INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)