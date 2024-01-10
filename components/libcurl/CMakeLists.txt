cmake_minimum_required(VERSION 3.10)

project(libcurl)

add_library(${PROJECT_NAME} SHARED IMPORTED GLOBAL)
set_target_properties(${PROJECT_NAME} PROPERTIES 
    IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/libcurl.dll 
    IMPORTED_IMPLIB ${CMAKE_CURRENT_LIST_DIR}/lib/libcurl.lib
)

target_include_directories(${PROJECT_NAME} INTERFACE include)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/lib/libcurl.dll ${CMAKE_BINARY_DIR}/libcurl.dll)
