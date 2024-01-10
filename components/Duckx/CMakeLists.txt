cmake_minimum_required(VERSION 3.10)

project(Duckx)

aux_source_directory(src Duckx_SOURCES)

add_library(${PROJECT_NAME} STATIC ${Duckx_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
