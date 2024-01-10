cmake_minimum_required(VERSION 3.10)

project(sqlite3)

option(SQLITE3_SHARED "SQLite3 lib use dynamic link library" ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(SQLITE3_SHARED)
    add_library(sqlite3 SHARED "src/sqlite3.c")
    if(Morose_FOUND)
        morose_copy(TARGET sqlite3 FILES "${CMAKE_CURRENT_BINARY_DIR}/sqlite3.dll")
    endif()
else()
    add_library(sqlite3 STATIC "src/sqlite3.c")
endif()

target_include_directories(sqlite3 INTERFACE "include")

add_library(SQLite3::SQLite3 ALIAS sqlite3)

set(CMAKE_USE_SQLITE_ORM TRUE CACHE BOOL "Use SQLite3 ORM")
