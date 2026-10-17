cmake_minimum_required(VERSION 3.20)

project(igi_clean_room_engine LANGUAGES CXX)

add_library(igi_retail_engine STATIC
    src/retail/audio/sound_key_functions.cpp
    src/retail/audio/sound_ilsf_header.cpp
    src/retail/audio/sound_ilsf_stream_reader.cpp
    src/retail/audio/sound_adpcm_decoder.cpp
    src/retail/audio/sound_attenuation_table.cpp
    src/retail/core/frame_natives.cpp
    src/retail/core/app_context.cpp
    src/retail/core/debug_command_line.cpp
    src/retail/diagnostics/log_record.cpp
    src/retail/model/mef_render_data.cpp
    src/retail/model/mef_model_loader.cpp
    src/retail/model/model_texture_package.cpp
    src/retail/render/trans_context.cpp
    src/retail/resource/resource_archive.cpp
    src/retail/resource/resource_cache.cpp
    src/retail/script/qvm_compiled_program.cpp
    src/retail/script/qvm_loader.cpp
    src/retail/script/qvm_object_calls.cpp
    src/retail/texture/model_texture_pixels.cpp
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
    add_executable(igi_developer_controls_tests tests/core/developer_controls_test.cpp)
    target_compile_features(igi_developer_controls_tests PRIVATE cxx_std_20)
    add_test(NAME igi_developer_controls_tests COMMAND igi_developer_controls_tests)
    add_executable(igi_retail_render_data_tests tests/model/mef_render_data_test.cpp)
    target_link_libraries(igi_retail_render_data_tests PRIVATE igi_retail_engine)
    add_test(NAME igi_retail_render_data_tests COMMAND igi_retail_render_data_tests)

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

    add_executable(igi_retail_ilsf_stream_tests
        tests/audio/sound_ilsf_stream_reader_test.cpp
    )
    target_link_libraries(igi_retail_ilsf_stream_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_ilsf_stream_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_ilsf_stream_tests COMMAND igi_retail_ilsf_stream_tests)

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

    add_executable(igi_retail_debug_command_line_tests
        tests/core/debug_command_line_test.cpp
    )

    target_link_libraries(igi_retail_debug_command_line_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_debug_command_line_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_debug_command_line_tests COMMAND igi_retail_debug_command_line_tests)

    add_executable(igi_retail_trans_context_tests
        tests/render/trans_context_test.cpp
    )

    target_link_libraries(igi_retail_trans_context_tests PRIVATE igi_retail_engine)
    target_compile_features(igi_retail_trans_context_tests PRIVATE cxx_std_20)
    add_test(NAME igi_retail_trans_context_tests COMMAND igi_retail_trans_context_tests)

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

if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    target_sources(igi_retail_engine PRIVATE src/retail/model/mef_native_attachments.cpp)
    if(BUILD_TESTING)
        add_executable(igi_retail_native_differential_tests tests/model/mef_native_differential_test.cpp)
        target_link_libraries(igi_retail_native_differential_tests PRIVATE igi_retail_engine)
        set(IGI_RETAIL_EXECUTABLE "" CACHE FILEPATH "Retail IGI.exe for original-instruction differential tests")
        if(IGI_RETAIL_EXECUTABLE)
            file(SHA256 "${IGI_RETAIL_EXECUTABLE}" IGI_RETAIL_SHA256)
            if(NOT IGI_RETAIL_SHA256 STREQUAL "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01")
                message(FATAL_ERROR "Retail executable digest differs; native addresses need revalidation")
            endif()
            add_test(NAME igi_retail_native_differential_tests
                COMMAND igi_retail_native_differential_tests "${IGI_RETAIL_EXECUTABLE}")
        endif()
        add_executable(igi_retail_native_attachment_tests tests/model/mef_native_attachments_test.cpp)
        target_link_libraries(igi_retail_native_attachment_tests PRIVATE igi_retail_engine)
        add_test(NAME igi_retail_native_attachment_tests COMMAND igi_retail_native_attachment_tests)
    endif()
endif()

if(WIN32)
    add_subdirectory(platform/windows)
endif()
