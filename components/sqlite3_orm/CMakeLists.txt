cmake_minimum_required(VERSION 3.10)

project(sqlite3)

option(SQLITE3_SHARED "SQLite3 lib use dynamic link library" OFF)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(SQLITE3_SHARED)
    message(STATUS "sqlite orm use shared library")
    add_library(sqlite3 SHARED "src/sqlite3.c")
else()
    message(STATUS "sqlite orm use static library")
    add_library(sqlite3 STATIC "src/sqlite3.c")
endif()

target_compile_definitions(sqlite3 PUBLIC USE_SQLITE_ORM)

target_include_directories(sqlite3 PUBLIC "include" "include/sqlite_orm")

add_library(SQLite3::SQLite3 ALIAS sqlite3)

set(CMAKE_USE_SQLITE_ORM TRUE CACHE BOOL "Use SQLite3 ORM")
