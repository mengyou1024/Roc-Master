cmake_minimum_required(VERSION 3.10)

project(Snap7)

add_library(${PROJECT_NAME} STATIC "src/snap7.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC "include")

target_link_libraries(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/snap7.lib")

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/snap7.dll"
    "${CMAKE_BINARY_DIR}/snap7.dll"
)
