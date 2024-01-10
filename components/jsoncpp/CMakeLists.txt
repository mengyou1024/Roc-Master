cmake_minimum_required(VERSION 3.10)

project(jsoncpp)

aux_source_directory(src jsoncpp_SOURCES)

add_library(${PROJECT_NAME} STATIC ${jsoncpp_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
