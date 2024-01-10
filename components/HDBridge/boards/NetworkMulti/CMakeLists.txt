cmake_minimum_required(VERSION 3.10)

add_library(_NetworkMulti SHARED IMPORTED GLOBAL)

set_target_properties(
    _NetworkMulti PROPERTIES 
    IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/NetworkMulti.dll"
    IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/NetworkMulti.lib"
)

add_library(NetworkMulti NetWorkMulti.cpp)

message(STATUS HDBridge_INCLUDE: ${HDBridge_INCLUDE})

include_directories(${HDBridge_INCLUDE})

target_include_directories(NetworkMulti PUBLIC "include")

if (CMAKE_USE_SQLITE_ORM)
    message(STATUS "HDBridge::NetworkMulti: use sqlite orm")
    target_link_libraries(NetworkMulti PRIVATE SQLite3::SQLite3)
endif()

target_link_libraries(NetworkMulti PUBLIC _NetworkMulti)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/lib/NetworkMulti.dll ${CMAKE_BINARY_DIR}/NetworkMulti.dll)
add_library(HDBridge::NetworkMulti ALIAS NetworkMulti)

if (Morose_FOUND)
    message(STATUS "HDBrideg::NetworkMulti Morose Open")
    morose_copy(TARGET NetworkMulti FILES "${CMAKE_CURRENT_LIST_DIR}/lib/NetworkMulti.dll")
endif()


