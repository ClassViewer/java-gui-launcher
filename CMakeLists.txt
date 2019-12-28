cmake_minimum_required(VERSION 3.15)

if (NOT DEFINED APP_NAME)
    message(FATAL_ERROR "Application name has not been defined")
endif ()

if (NOT DEFINED APP_VERSION)
    message(FATAL_ERROR "Application version number has not been defined")
endif ()

if (NOT DEFINED APP_ORG)
    message(FATAL_ERROR "Application organization has not been defined")
endif ()

if (NOT DEFINED APP_JVM_TYPE)
    set(APP_JVM_TYPE "server")
endif ()

if (DEFINED APP_MAIN_CLASS)
    string(REPLACE "." "/" APP_MAIN_CLASS_NAME ${APP_MAIN_CLASS})
elseif ()
    message(FATAL_ERROR "Application main class has not been defined")
endif ()

if (DEFINED APP_PREDEF_OPTIONS)
    string(REPLACE "\\" "\\\\" APP_MAIN_CLASS_NAME ${APP_MAIN_CLASS})
    string(REPLACE "\"" "\\\"" APP_MAIN_CLASS_NAME ${APP_MAIN_CLASS})
    string(REPLACE ";;;" "\\0" APP_MAIN_CLASS_NAME ${APP_MAIN_CLASS})
endif ()

if (DEFINED APP_ICON)
    get_filename_component(_APP_ICON_PATH ${APP_ICON} ABSOLUTE)
    string(REPLACE "\\" "\\\\" APP_ICON_PATH ${_APP_ICON_PATH})
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake)

project(launcher
        VERSION 2.0
        LANGUAGES C)

include(VersionParser)
parse_version_number(APP_VERSION)

set(CMAKE_C_STANDARD 99)

find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(
        launcher_config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/launcher_config.h
        @ONLY
)

if (WIN32 AND MSVC)
    configure_file(
            launcher.rc.in
            ${CMAKE_CURRENT_BINARY_DIR}/launcher.rc
            @ONLY
    )
    add_executable(${APP_NAME} launcher_win_main.c ${CMAKE_CURRENT_BINARY_DIR}/launcher.rc ${CMAKE_CURRENT_BINARY_DIR}/launcher_config.h launcher.h launcher_win.h launcher_win.c)
    target_link_libraries(${APP_NAME} pathcch)
    if (NOT DEFINED APP_SHOW_CONSOLE)
        set_property(TARGET ${APP_NAME} PROPERTY WIN32_EXECUTABLE true)
    else ()
        add_compile_definitions("APPLICATION_SHOW_CONSOLE")
    endif ()
elseif ()
    message(FATAL_ERROR "Unsupported platform")
endif ()
