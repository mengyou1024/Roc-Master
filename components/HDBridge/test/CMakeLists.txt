cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(test)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/wd4819)
endif ()

add_subdirectory(sqlite3_orm)
add_subdirectory(.. HDBridge)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} HDBridge::TOFDPort HDBridge::Utils SQLite3::SQLite3)
add_definitions(-DUSE_SQLITE_ORM)
