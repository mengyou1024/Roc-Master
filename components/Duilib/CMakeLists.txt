cmake_minimum_required(VERSION 3.10)

project(Duilib)

set(CMAKE_MFC_FLAG 2)
find_package(MFC)

if (NOT MFC_FOUND)
    message(FATAL_ERROR "MFC not found")
endif()

aux_source_directory(Core Duilib_Core_Src)
aux_source_directory(Control Duilib_Control_Src)
aux_source_directory(Layout Duilib_Layout_Src)
aux_source_directory(Utils Duilib_Utils_Src)

set(Duilib_Src 
    ${Duilib_Core_Src} 
    ${Duilib_Control_Src} 
    ${Duilib_Layout_Src} 
    ${Duilib_Utils_Src}
    UIlib.cpp
    StdAfx.cpp
)

add_compile_options(/wd4819 /wd4244 /wd5033)

add_library(${PROJECT_NAME} STATIC ${Duilib_Src})

target_include_directories(${PROJECT_NAME} PUBLIC Control Core Layout Utils .)
target_compile_definitions(${PROJECT_NAME} PRIVATE UILIB_STATIC) # _CRT_SECURE_NO_WARNINGS
target_compile_definitions(${PROJECT_NAME} PUBLIC WIN32 _WINDOWS _UNICODE UNICODE _AFXDLL) # 
