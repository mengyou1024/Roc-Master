cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(BuildSystem REQUIRED)

project(Roc-Master)

aux_source_directory(src/Model Src_Model)
aux_source_directory(src/UI Src_Ui)
aux_source_directory(src/Widgets Src_Widgets)
aux_source_directory(src Src_Ext)

add_definitions(-DORM_DB_NAME=\"data.db\")

if (APP_RELEASE)
    add_definitions(-DAPP_RELEASE)
endif()

add_executable(${PROJECT_NAME} WIN32 ${Src_Model} ${Src_Ui} ${Src_Widgets} ${Src_Ext})

# 资源文件
file(GLOB_RECURSE RES_THEME_FILES src/res/Theme/*)
add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/src/res/Theme.zip
    COMMAND 7z a -tZip ${CMAKE_SOURCE_DIR}/src/res/Theme.zip ${CMAKE_SOURCE_DIR}/src/res/Theme/ -bso0
    DEPENDS ${RES_THEME_FILES}
    COMMENT "Compress Theme files"
)
file(GLOB_RECURSE RES_THEME_FILES src/res/*.ttf src/res/*.ico src/res/*.zip)
add_custom_target(GEN_RES ALL
    rc.exe /nologo /fo Resource.res ${CMAKE_SOURCE_DIR}/src/Mutilple.rc
    DEPENDS ${CMAKE_SOURCE_DIR}/src/res/Theme.zip
    BYPRODUCTS Resource.res
    COMMENT "Generate Resource.res"
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}/Resource.res
    libcurl Duilib HDBridge::Utils HDBridge::TOFDPort HDBridge::NetworkMulti 
    spdlog SQLite3::SQLite3 freetype glew glm jsoncpp RTTR::Core_Lib Snap7 Duckx
)

target_include_directories(${PROJECT_NAME} PUBLIC "include" src/Model src/UI src/Widgets src)

target_link_options(${PROJECT_NAME} PRIVATE /ENTRY:wWinMainCRTStartup)

# Config File

configure_file(
    script/pack-installer.iss.in
    ${CMAKE_SOURCE_DIR}/pack-installer.iss
    @ONLY
)

configure_file(
    ProjectConfig.h.in
    ${CMAKE_SOURCE_DIR}/src/ProjectConfig.h
    @ONLY
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
    "${CMAKE_SOURCE_DIR}/template"
    "${CMAKE_BINARY_DIR}/template"
)

file(RELATIVE_PATH CMAKE_BINARY_DIR_RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

if(APP_RELEASE)

add_custom_target(
    PackExecutable
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME}-${APP_VERSION}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME}-${APP_VERSION}/template"
    COMMAND powershell "Copy-Item -Force -Recurse ${CMAKE_BINARY_DIR}/*.dll ${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME}-${APP_VERSION}/;\
    Copy-Item -Force -Recurse ${CMAKE_BINARY_DIR}/*.exe ${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME}-${APP_VERSION}/;\
    Copy-Item -Recurse ${CMAKE_SOURCE_DIR}/template/* ${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME}-${APP_VERSION}/template"
    COMMAND copypedeps -n -r ${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME}-${APP_VERSION}/${PROJECT_NAME}.exe 
    ${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME}-${APP_VERSION}/ 1>nul 2> nul
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS ${PROJECT_NAME}
)

add_custom_target(
    PackInstaller 
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME}Installer-${APP_VERSION}.exe
    COMMAND ISCC pack-installer.iss /Q
    DEPENDS ${PROJECT_NAME} PackExecutable
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    BYPRODUCTS "${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME}Installer.exe" ${CMAKE_SOURCE_DIR}/pack-installer.iss
)

add_custom_target(
    Archive 
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME}-${APP_VERSION}-archive.zip
    COMMAND powershell "Copy-Item -Force -Recurse ${CMAKE_SOURCE_DIR}/.git ${CMAKE_SOURCE_DIR}/output/archive/.git;\
    cd ${CMAKE_SOURCE_DIR}/output/archive/; \
    git reset --hard" > nul
    COMMAND echo "{\
\"APP_VERSION\":\"${APP_VERSION}\",\
\"GIT_REPOSITORY_URL\": \"-\",\
\"GIT_USER_NAME\": \"-\",\
\"GIT_USER_EMAIL\": \"-\"\
}" > ${CMAKE_SOURCE_DIR}/output/archive/archive.json
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/output/archive/.git ${CMAKE_SOURCE_DIR}/output/archive/.github
    COMMAND 7z a -tZip ${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME}-${APP_VERSION}-archive.zip ${CMAKE_SOURCE_DIR}/output/archive/* -bso0
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/output/archive
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    BYPRODUCTS ${CMAKE_SOURCE_DIR}/output/${PROJECT_NAME}-${APP_VERSION}-archive.zip
    ${CMAKE_SOURCE_DIR}/archive.json
)

message("
--------------------------------------------------------------------
|
|   Usage: cmake --build ${CMAKE_BINARY_DIR_RELATIVE} --target <target> [...]
|
|   当前软件版本: ${APP_VERSION}
|
|   targets:
|
|       ${PROJECT_NAME}: 可执行程序
|           --> ${CMAKE_BINARY_DIR_RELATIVE}/${PROJECT_NAME}.exe
|
|       PackExecutable: 打包生成免安装包
|           --> output/${PROJECT_NAME}-${APP_VERSION}
|
|       PackInstaller: 打包生成安装包
|           --> output/${PROJECT_NAME}Installer-${APP_VERSION}.exe
|
|       Archive: 归档
|           --> output/${PROJECT_NAME}-${APP_VERSION}.zip
|
|       clean: 清理
|
--------------------------------------------------------------------
")

endif(APP_RELEASE)
