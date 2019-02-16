cmake_minimum_required(VERSION 3.6)

set(EE_USE_TESTS OFF CACHE BOOL "Disable easy-exception tests" FORCE)
set(EE_USE_EXAMPLES OFF CACHE BOOL "Disable easy-exception examples" FORCE)
add_subdirectory(../../.. ${CMAKE_CURRENT_BINARY_DIR}/EasyException)

find_library(log-lib log)
find_library(android-lib android)

include_directories(
        ${ANDROID_NDK}/sources/android
        ../../../include
)

add_library(
        EasyExceptionAndroid SHARED
        src/cpp/android_main.cpp
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)
target_link_libraries (
        EasyExceptionAndroid
        EasyException
        ${log-lib}
        ${android-lib}
)