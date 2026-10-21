cmake_minimum_required(VERSION 3.20)
project(project_open_igi_clean LANGUAGES CXX)

if(NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "The diagnostic baseline requires Windows x86 (PE32).")
endif()

add_executable(open-igi WIN32 engine/main.cpp)
target_compile_features(open-igi PRIVATE cxx_std_20)
