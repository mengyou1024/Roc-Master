cmake_minimum_required(VERSION 3.10)

project(glew)

if (CMAKE_USE_GLEW_SHARED)
    add_library(${PROJECT_NAME} SHARED IMPORTED GLOBAL)
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/glew32.dll 
        IMPORTED_IMPLIB ${CMAKE_CURRENT_LIST_DIR}/lib/glew32.lib
    )
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/lib/glew32.dll ${CMAKE_BINARY_DIR}/glew32.dll)
else ()
    add_library(${PROJECT_NAME} STATIC IMPORTED GLOBAL)
    set_target_properties(${PROJECT_NAME} PROPERTIES IMPORTED_LOCATION  ${CMAKE_CURRENT_LIST_DIR}/lib/glew32s.lib)
endif()

target_include_directories(${PROJECT_NAME} INTERFACE include)

