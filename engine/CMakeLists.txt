cmake_minimum_required(VERSION 3.20)

project(igi_clean_room_engine LANGUAGES CXX)

add_library(igi_retail_engine STATIC
    src/retail/audio/sound_key_functions.cpp
    src/retail/audio/sound_ilsf_header.cpp
    src/retail/audio/sound_adpcm_decoder.cpp
    src/retail/audio/sound_attenuation_table.cpp
    src/retail/core/frame_natives.cpp
    src/retail/diagnostics/log_record.cpp
    src/retail/model/mef_model_loader.cpp
    src/retail/model/model_texture_package.cpp
    src/retail/resource/resource_archive.cpp
    src/retail/script/qvm_compiled_program.cpp
    src/retail/texture/loop_terrain_texture.cpp
)

target_include_directories(igi_retail_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(igi_retail_engine PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(igi_retail_engine PRIVATE /W4 /permissive-)
else()
    target_compile_options(igi_retail_engine PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
endif()

include(CTest)

if(BUILD_TESTING)
    add_executable(igi_retail_engine_tests
        tests/audio/sound_key_functions_test.cpp
    )

    target_link_libraries(igi_retail_engine_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_engine_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_engine_tests COMMAND igi_retail_engine_tests)

    add_executable(igi_retail_sound_tests
        tests/audio/sound_ilsf_header_test.cpp
    )
    target_link_libraries(igi_retail_sound_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_sound_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_sound_tests COMMAND igi_retail_sound_tests)

    add_executable(igi_retail_adpcm_tests
        tests/audio/sound_adpcm_decoder_test.cpp
    )
    target_link_libraries(igi_retail_adpcm_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_adpcm_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_adpcm_tests COMMAND igi_retail_adpcm_tests)

    add_executable(igi_retail_attenuation_tests
        tests/audio/sound_attenuation_table_test.cpp
    )
    target_link_libraries(igi_retail_attenuation_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_attenuation_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_attenuation_tests COMMAND igi_retail_attenuation_tests)

    add_executable(igi_retail_core_tests
        tests/core/frame_natives_test.cpp
    )

    target_link_libraries(igi_retail_core_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_core_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_core_tests COMMAND igi_retail_core_tests)

    add_executable(igi_retail_log_record_tests
        tests/core/log_record_test.cpp
    )

    target_link_libraries(igi_retail_log_record_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_log_record_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_log_record_tests COMMAND igi_retail_log_record_tests)

    add_executable(igi_retail_mef_tests
        tests/model/mef_model_loader_test.cpp
    )

    target_link_libraries(igi_retail_mef_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_mef_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_mef_tests COMMAND igi_retail_mef_tests)

    add_executable(igi_retail_mtp_tests
        tests/model/model_texture_package_test.cpp
    )

    target_link_libraries(igi_retail_mtp_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_mtp_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_mtp_tests COMMAND igi_retail_mtp_tests)

    add_executable(igi_retail_qvm_tests
        tests/script/qvm_compiled_program_test.cpp
    )
    target_link_libraries(igi_retail_qvm_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_qvm_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_qvm_tests COMMAND igi_retail_qvm_tests)

    add_executable(igi_retail_resource_tests
        tests/resource/resource_archive_test.cpp
    )

    target_link_libraries(igi_retail_resource_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_resource_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_resource_tests COMMAND igi_retail_resource_tests)

    add_executable(igi_retail_texture_tests
        tests/texture/loop_terrain_texture_test.cpp
    )

    target_link_libraries(igi_retail_texture_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_texture_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_texture_tests COMMAND igi_retail_texture_tests)
endif()

if(WIN32)
    add_subdirectory(platform/windows)
endif()
