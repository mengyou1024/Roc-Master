cmake_minimum_required(VERSION 3.10)

add_library(_TOFDPort SHARED IMPORTED GLOBAL)

set_target_properties(
    _TOFDPort PROPERTIES 
    IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/TOFDPort.dll"
    IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/TOFDPort.lib"
)

add_library(TOFDPort TOFDPort.cpp)

include_directories(${HDBridge_INCLUDE})

target_include_directories(TOFDPort PUBLIC "include" )

target_link_libraries(TOFDPort PUBLIC _TOFDPort)

if (CMAKE_USE_SQLITE_ORM)
    message(STATUS TOFDPort: use sqlite orm)
    target_link_libraries(TOFDPort PUBLIC SQLite3::SQLite3)
endif()

if (CMAKE_USE_SQLITE_ORM)
    message(STATUS "HDBridge::TOFDPort: use sqlite orm")
    target_link_libraries(TOFDPort PRIVATE SQLite3::SQLite3)
endif()

add_library(HDBridge::TOFDPort ALIAS TOFDPort)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/lib/TOFDPort.dll ${CMAKE_BINARY_DIR}/TOFDPort.dll
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/lib/ftd2xx.dll ${CMAKE_BINARY_DIR}/ftd2xx.dll
)

if (Morose_FOUND)
    morose_copy(
        TARGET 
        TOFDPort 
        FILES 
        "${CMAKE_CURRENT_LIST_DIR}/lib/TOFDPort.dll"
        "${CMAKE_CURRENT_LIST_DIR}/lib/ftd2xx.dll"
    )
endif()
