find_package(Git QUIET)

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/archive.json")
    if(GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags
            OUTPUT_VARIABLE APP_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    if(NOT APP_VERSION)
        message(FATAL_ERROR "Git repository must have a tag , use `git tag <tag_name> -m <tag_message>` to create a tag.\n"
            "\te.g.: `git tag v0.0.1 -m \"init\"`\n"
            "the git describe is use for varible `APP_VERSION`"
        )
    endif(NOT APP_VERSION)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} remote
        OUTPUT_VARIABLE GIT_REMOTE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} remote get-url ${GIT_REMOTE}
        OUTPUT_VARIABLE GIT_REPOSITORY_URL
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    unset(GIT_REMOTE)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} config user.name
        OUTPUT_VARIABLE GIT_USER_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} config user.email
        OUTPUT_VARIABLE GIT_USER_EMAIL
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    else(GIT_FOUND)
        message(FATAL_ERROR "no git found, use `choco install git` to install")
    endif(GIT_FOUND)
else (NOT EXISTS "${CMAKE_SOURCE_DIR}/archive.json")
    file(READ "${CMAKE_SOURCE_DIR}/archive.json" ARCHIVE_JSON_STRING)
    string(JSON APP_VERSION GET "${ARCHIVE_JSON_STRING}" APP_VERSION)
    string(JSON GIT_USER_NAME GET "${ARCHIVE_JSON_STRING}" GIT_USER_NAME)
    string(JSON GIT_REPOSITORY_URL GET "${ARCHIVE_JSON_STRING}" GIT_REPOSITORY_URL)
    string(JSON GIT_USER_EMAIL GET "${ARCHIVE_JSON_STRING}" GIT_USER_EMAIL)
endif(NOT EXISTS "${CMAKE_SOURCE_DIR}/archive.json")

message(STATUS "APP VERSION:" ${APP_VERSION})
message(STATUS "GIT_REPOSITORY_URL:${GIT_REPOSITORY_URL}")
message(STATUS "GIT_USER_NAME:${GIT_USER_NAME}")
message(STATUS "GIT_USER_EMAIL:${GIT_USER_EMAIL}")

# 搜索inno setup工具
find_program(ISCC_PATH ISCC)
if(ISCC_PATH)
    message(STATUS "Detected ISCC_PATH: ${ISCC_PATH}")
else(ISCC_PATH)
    message(WARNING "no ISCC path found, use `choco install innosetup` to install")
endif(ISCC_PATH)

# 搜索copypedeps
find_program(COPYPEDEPS_PATH copypedeps.exe)
if (COPYPEDEPS_PATH)
    message(STATUS "Detected COPYPEDEPS_PATH: ${COPYPEDEPS_PATH}")
else(COPYPEDEPS_PATH)
    message(WARNING "no COPYPEDEPS path found, `use choco install pedeps` to install")
endif(COPYPEDEPS_PATH)

# 搜索7zip
find_program(7ZIP_PATH 7z)
if (7ZIP_PATH)
    message(STATUS "Detected 7ZIP_PATH: ${7ZIP_PATH}")
else()
    message(WARNING "no 7ZIP path found, use `choco install 7zip` to install")
endif()

#[[
    添加子目录的路径, 该函数会遍历目录下所有的文件夹, 如果存在CMakeLists.txt则添加至子目录的构建目录
    `add_subdirectory_path(path)`
    `path`: 子目录路径
]]
function(add_subdirectory_path path)
    file(GLOB SUBPATH "${path}/*")
    foreach(ITEM ${SUBPATH})
        if (IS_DIRECTORY "${ITEM}" AND EXISTS "${ITEM}/CMakeLists.txt")
            file(RELATIVE_PATH ITEM_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${ITEM})
            add_subdirectory(${ITEM_PATH})
        endif(IS_DIRECTORY "${ITEM}" AND EXISTS "${ITEM}/CMakeLists.txt")
    endforeach(ITEM ${ITEM_PATH})
endfunction(add_subdirectory_path path)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
message(STATUS "Note: runtime output directory set to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

add_subdirectory_path("components")

add_compile_definitions(/wd4819)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options(/bigobj)
