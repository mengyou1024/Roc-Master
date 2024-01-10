cmake_minimum_required(VERSION 3.10)

project(spdlog)

add_library(spdlog INTERFACE)

target_include_directories(spdlog INTERFACE "include")

target_compile_definitions(spdlog INTERFACE SPDLOG_WCHAR_TO_UTF8_SUPPORT)
