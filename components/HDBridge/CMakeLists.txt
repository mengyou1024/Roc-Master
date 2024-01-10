cmake_minimum_required(VERSION 3.10)

project(HDBridge)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/wd4819)
else()
    message(FATAL_ERROR "only support msvc compiler")
endif ()

set(HDBridge_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include")

function(add_subdirectory_path path)
    file(GLOB SUBPATH "${path}/*")
    foreach(ITEM ${SUBPATH})
        if (IS_DIRECTORY "${ITEM}" AND EXISTS "${ITEM}/CMakeLists.txt")
            file(RELATIVE_PATH ITEM_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${ITEM})
            add_subdirectory(${ITEM_PATH})
        endif(IS_DIRECTORY "${ITEM}" AND EXISTS "${ITEM}/CMakeLists.txt")
    endforeach(ITEM ${ITEM_PATH})
endfunction(add_subdirectory_path path)

add_subdirectory_path("boards")

aux_source_directory("src" UTILS_SRC_LIST)
add_library(HDBridge_Utils STATIC ${UTILS_SRC_LIST})

if (CMAKE_USE_SQLITE_ORM)
    message(STATUS "HDBridge_Utils: use sqlite orm")
    target_link_libraries(HDBridge_Utils PRIVATE SQLite3::SQLite3)
    target_link_libraries(HDBridge_Utils PRIVATE spdlog)
endif()

target_include_directories(HDBridge_Utils PUBLIC "include" boards/TOFDPort/include)

add_library(HDBridge::Utils ALIAS HDBridge_Utils)
