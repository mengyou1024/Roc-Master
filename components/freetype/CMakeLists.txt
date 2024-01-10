cmake_minimum_required(VERSION 3.10)

project(freetype)

add_library(${PROJECT_NAME} STATIC IMPORTED GLOBAL)

set_target_properties(${PROJECT_NAME} PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/freetype.lib")

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
