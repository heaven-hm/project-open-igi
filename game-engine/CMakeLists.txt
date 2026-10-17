cmake_minimum_required(VERSION 3.20)

project(igi_retail_engine LANGUAGES CXX)

add_library(igi_retail_audio STATIC
    src/retail/audio/sound_key_functions.cpp
)

target_include_directories(igi_retail_audio
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(igi_retail_audio PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(igi_retail_audio PRIVATE /W4 /permissive-)
else()
    target_compile_options(igi_retail_audio PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
endif()

include(CTest)

if(BUILD_TESTING)
    add_executable(igi_retail_audio_tests
        tests/audio/sound_key_functions_test.cpp
    )

    target_link_libraries(igi_retail_audio_tests PRIVATE igi_retail_audio)
    target_compile_features(igi_retail_audio_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_audio_tests COMMAND igi_retail_audio_tests)
endif()
