cmake_minimum_required(VERSION 3.20)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

option(IGI_ENABLE_NATIVE_GATE "Run retail binary verification gate before building" ON)
option(IGI_FULL_PRODUCTION_GATE "Require strict changed-native gate at VERIFIED (legacy option name)" OFF)

set(IGI_RETAIL_EXE "$ENV{IGI_RETAIL_EXE}" CACHE FILEPATH "Path to retail igi.exe")
if(NOT IGI_RETAIL_EXE)
    set(IGI_RETAIL_EXE "/Users/haseeb-mir/Downloads/IGI/igi.exe")
endif()

get_filename_component(IGI_PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
if(NOT EXISTS "${IGI_PROJECT_ROOT}/scripts/native_gate.py")
    set(IGI_PROJECT_ROOT "${CMAKE_SOURCE_DIR}")
endif()

add_library(igi_retail_engine STATIC
    src/retail/audio/sound_key_functions.cpp
    src/retail/audio/sound_ilsf_header.cpp
    src/retail/audio/sound_ilsf_stream_reader.cpp
    src/retail/audio/sound_adpcm_decoder.cpp
    src/retail/audio/sound_attenuation_table.cpp
    src/retail/core/frame_natives.cpp
    src/retail/core/window_procedure.cpp
    src/retail/core/retail_main.cpp
    src/retail/core/app_startup_natives.cpp
    src/retail/core/native_0048f0d0.cpp
    src/retail/core/native_0048f0f0.cpp
    src/retail/core/native_0048f0e0.cpp
    src/retail/core/flow.cpp
    src/retail/core/class_registration.cpp
    src/retail/core/task_types.cpp
    src/retail/core/game_flow.cpp
    src/retail/core/app_context.cpp
    src/retail/core/fpu_state.cpp
    src/retail/core/debug_command_line.cpp
    src/retail/input/input_port.cpp
    src/retail/input/input_port_process.cpp
    src/retail/diagnostics/log_record.cpp
    src/retail/error/error_show.cpp
    src/retail/human/human_camera_eye.cpp
    src/retail/human/human_view.cpp
    src/retail/model/mef_render_data.cpp
    src/retail/model/mef_bone_rig.cpp
    src/retail/model/iff_chunk_tree.cpp
    src/retail/model/iff_skeleton.cpp
    src/retail/model/mef_model_loader.cpp
    src/retail/model/model_texture_package.cpp
    src/retail/model/mef_skinner.cpp
    src/retail/config/config_graphic_options.cpp
    src/retail/config/retail_config.cpp
    src/retail/config/retail_input_bindings.cpp
    src/retail/config/input_remap.cpp
    src/retail/render/trans_context.cpp
    src/retail/render/view_block.cpp
    src/retail/render/camera_transform.cpp
    src/retail/render/backend.cpp
    src/retail/sys/ilff_navigation.cpp
    src/retail/sys/resource_lookup.cpp
    src/retail/dynobj/hash_lookup.cpp
    src/retail/dynobj/dynamic_cube.cpp
    src/retail/dynobj/local_models_mef.cpp
    src/retail/dynobj/mtp_package_loader.cpp
    src/retail/qfile/alias_resolve.cpp
    src/retail/resource/is_resource_loaded.cpp
    src/retail/resource/resource_lifecycle.cpp
    src/retail/resource/resource_unpack.cpp
    src/retail/resource/sys_resource.cpp
    src/retail/resource/device_dispatch.cpp
    src/retail/mem/heap_list.cpp
    src/retail/mem/mem_alloc.cpp
    src/retail/resource/resource_archive.cpp
    src/retail/resource/resource_cache.cpp
    src/retail/script/qvm_compiled_program.cpp
    src/retail/script/qvm_loader.cpp
    src/retail/script/qvm_native_loader.cpp
    src/retail/script/humanplayer_defines.cpp
    src/retail/script/qvm_object_calls.cpp
    src/retail/script/qvm_stack_natives.cpp
    src/retail/script/script_mode.cpp
    src/retail/texture/model_texture_pixels.cpp
    src/retail/texture/loop_terrain_texture.cpp
    src/retail/texture/texture_loaders.cpp
    src/retail/terrain/terrain_loaders.cpp
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

if(IGI_ENABLE_NATIVE_GATE)
    set(IGI_GATE_ARGS --root "${IGI_PROJECT_ROOT}")
    if(IGI_RETAIL_EXE)
        list(APPEND IGI_GATE_ARGS --binary "${IGI_RETAIL_EXE}")
    endif()
    if(IGI_FULL_PRODUCTION_GATE)
        list(APPEND IGI_GATE_ARGS --full-production)
    else()
        list(APPEND IGI_GATE_ARGS --development)
    endif()

    file(GLOB_RECURSE IGI_NATIVE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/retail/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/igi/retail/*.hpp"
    )

    add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/igi_native_verification.stamp"
        COMMAND ${Python3_EXECUTABLE} "${IGI_PROJECT_ROOT}/scripts/cmake_native_gate.py" ${IGI_GATE_ARGS}
        COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/igi_native_verification.stamp"
        WORKING_DIRECTORY "${IGI_PROJECT_ROOT}"
        DEPENDS
            ${IGI_NATIVE_SOURCES}
            "${IGI_PROJECT_ROOT}/docs/fidelity/native-verification.json"
            "${IGI_PROJECT_ROOT}/docs/fidelity/gate-integrity.sha256"
            "${IGI_PROJECT_ROOT}/scripts/native_gate.py"
            "${IGI_PROJECT_ROOT}/scripts/cmake_native_gate.py"
            "${IGI_PROJECT_ROOT}/scripts/discover_referenced_natives.py"
        COMMENT "Verifying retail native implementations against igi.exe (CFG/angr/MCP)..."
        USES_TERMINAL
    )

    add_custom_target(igi_native_verification
        DEPENDS "${CMAKE_BINARY_DIR}/igi_native_verification.stamp"
    )

    add_custom_target(igi_native_verification_force
        COMMAND ${Python3_EXECUTABLE} "${IGI_PROJECT_ROOT}/scripts/cmake_native_gate.py" ${IGI_GATE_ARGS}
        COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/igi_native_verification.stamp"
        WORKING_DIRECTORY "${IGI_PROJECT_ROOT}"
        COMMENT "Force-verifying retail native implementations against igi.exe..."
        USES_TERMINAL
    )

    add_dependencies(igi_retail_engine igi_native_verification)
endif()

include(CTest)

# TESTS REMOVED: if(BUILD_TESTING)
# TESTS REMOVED:     if(IGI_ENABLE_NATIVE_GATE)
# TESTS REMOVED:         add_test(
# TESTS REMOVED:             NAME igi_native_verification_gate
# TESTS REMOVED:             COMMAND ${Python3_EXECUTABLE} "${IGI_PROJECT_ROOT}/scripts/cmake_native_gate.py" ${IGI_GATE_ARGS}
# TESTS REMOVED:             WORKING_DIRECTORY "${IGI_PROJECT_ROOT}"
# TESTS REMOVED:         )
# TESTS REMOVED:     endif()

# TESTS REMOVED:     add_executable(igi_developer_controls_tests tests/core/developer_controls_test.cpp)
# TESTS REMOVED:     target_compile_features(igi_developer_controls_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_developer_controls_tests COMMAND igi_developer_controls_tests)
# TESTS REMOVED:     add_executable(igi_retail_iff_skeleton_tests tests/model/iff_skeleton_test.cpp)
# TESTS REMOVED:     target_link_libraries(igi_retail_iff_skeleton_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_iff_skeleton_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_iff_skeleton_tests COMMAND igi_retail_iff_skeleton_tests)

# TESTS REMOVED:     add_executable(igi_retail_render_data_tests tests/model/mef_render_data_test.cpp)
# TESTS REMOVED:     target_link_libraries(igi_retail_render_data_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     add_test(NAME igi_retail_render_data_tests COMMAND igi_retail_render_data_tests)

# TESTS REMOVED:     add_executable(igi_retail_engine_tests
# TESTS REMOVED:         tests/audio/sound_key_functions_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_engine_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_engine_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_engine_tests COMMAND igi_retail_engine_tests)

# TESTS REMOVED:     add_executable(igi_retail_sound_tests
# TESTS REMOVED:         tests/audio/sound_ilsf_header_test.cpp
# TESTS REMOVED:     )
# TESTS REMOVED:     target_link_libraries(igi_retail_sound_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_sound_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_sound_tests COMMAND igi_retail_sound_tests)

# TESTS REMOVED:     add_executable(igi_retail_ilsf_stream_tests
# TESTS REMOVED:         tests/audio/sound_ilsf_stream_reader_test.cpp
# TESTS REMOVED:     )
# TESTS REMOVED:     target_link_libraries(igi_retail_ilsf_stream_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_ilsf_stream_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_ilsf_stream_tests COMMAND igi_retail_ilsf_stream_tests)

# TESTS REMOVED:     add_executable(igi_retail_adpcm_tests
# TESTS REMOVED:         tests/audio/sound_adpcm_decoder_test.cpp
# TESTS REMOVED:     )
# TESTS REMOVED:     target_link_libraries(igi_retail_adpcm_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_adpcm_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_adpcm_tests COMMAND igi_retail_adpcm_tests)

# TESTS REMOVED:     add_executable(igi_retail_attenuation_tests
# TESTS REMOVED:         tests/audio/sound_attenuation_table_test.cpp
# TESTS REMOVED:     )
# TESTS REMOVED:     target_link_libraries(igi_retail_attenuation_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_attenuation_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_attenuation_tests COMMAND igi_retail_attenuation_tests)

# TESTS REMOVED:     add_executable(igi_retail_core_tests
# TESTS REMOVED:         tests/core/frame_natives_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_core_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_core_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_core_tests COMMAND igi_retail_core_tests)

# TESTS REMOVED:     add_executable(igi_retail_input_port_tests
# TESTS REMOVED:         tests/core/input_port_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_input_port_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_input_port_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_input_port_tests COMMAND igi_retail_input_port_tests)

# TESTS REMOVED:     add_executable(igi_retail_config_tests
# TESTS REMOVED:         tests/config/retail_config_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_config_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_config_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_config_tests COMMAND igi_retail_config_tests)

# TESTS REMOVED:     add_executable(igi_retail_player_controller_tests
# TESTS REMOVED:         tests/platform/player_controller_test.cpp
# TESTS REMOVED:         platform/windows/player_controller.cpp
# TESTS REMOVED:         platform/windows/retail_animation.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_player_controller_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_player_controller_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_player_controller_tests COMMAND igi_retail_player_controller_tests)

# TESTS REMOVED:     add_executable(igi_retail_log_record_tests
# TESTS REMOVED:         tests/core/log_record_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_log_record_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_log_record_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_log_record_tests COMMAND igi_retail_log_record_tests)

# TESTS REMOVED:     add_executable(igi_retail_debug_command_line_tests
# TESTS REMOVED:         tests/core/debug_command_line_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_debug_command_line_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_debug_command_line_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_debug_command_line_tests COMMAND igi_retail_debug_command_line_tests)

# TESTS REMOVED:     add_executable(igi_retail_error_show_tests
# TESTS REMOVED:         tests/error/error_show_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_error_show_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_error_show_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_error_show_tests COMMAND igi_retail_error_show_tests)

# TESTS REMOVED:     add_executable(igi_retail_heap_list_tests
# TESTS REMOVED:         tests/mem/heap_list_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_heap_list_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_heap_list_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_heap_list_tests COMMAND igi_retail_heap_list_tests)

# TESTS REMOVED:     add_executable(igi_retail_mem_alloc_tests
# TESTS REMOVED:         tests/mem/mem_alloc_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_mem_alloc_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_mem_alloc_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_mem_alloc_tests COMMAND igi_retail_mem_alloc_tests)

# TESTS REMOVED:     add_executable(igi_retail_device_dispatch_tests
# TESTS REMOVED:         tests/resource/device_dispatch_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_device_dispatch_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_device_dispatch_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_device_dispatch_tests COMMAND igi_retail_device_dispatch_tests)

# TESTS REMOVED:     add_executable(igi_retail_resource_unpack_tests
# TESTS REMOVED:         tests/resource/resource_unpack_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_resource_unpack_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_resource_unpack_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_resource_unpack_tests COMMAND igi_retail_resource_unpack_tests)

# TESTS REMOVED:     add_executable(igi_retail_is_resource_loaded_tests
# TESTS REMOVED:         tests/resource/is_resource_loaded_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_is_resource_loaded_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_is_resource_loaded_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_is_resource_loaded_tests COMMAND igi_retail_is_resource_loaded_tests)

# TESTS REMOVED:     add_executable(igi_retail_resource_lifecycle_tests
# TESTS REMOVED:         tests/resource/resource_lifecycle_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_resource_lifecycle_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_resource_lifecycle_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_resource_lifecycle_tests COMMAND igi_retail_resource_lifecycle_tests)

# TESTS REMOVED:     add_executable(igi_retail_sys_resource_tests
# TESTS REMOVED:         tests/resource/sys_resource_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_sys_resource_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_sys_resource_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_sys_resource_tests COMMAND igi_retail_sys_resource_tests)

# TESTS REMOVED:     add_executable(igi_retail_alias_resolve_tests
# TESTS REMOVED:         tests/qfile/alias_resolve_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_alias_resolve_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_alias_resolve_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_alias_resolve_tests COMMAND igi_retail_alias_resolve_tests)

# TESTS REMOVED:     add_executable(igi_retail_hash_lookup_tests
# TESTS REMOVED:         tests/dynobj/hash_lookup_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_hash_lookup_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_hash_lookup_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_hash_lookup_tests COMMAND igi_retail_hash_lookup_tests)

# TESTS REMOVED:     add_executable(igi_retail_local_models_mef_tests
# TESTS REMOVED:         tests/dynobj/local_models_mef_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_local_models_mef_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_local_models_mef_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_local_models_mef_tests COMMAND igi_retail_local_models_mef_tests)

# TESTS REMOVED:     add_executable(igi_retail_mtp_package_loader_tests
# TESTS REMOVED:         tests/dynobj/mtp_package_loader_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_mtp_package_loader_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_mtp_package_loader_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_mtp_package_loader_tests COMMAND igi_retail_mtp_package_loader_tests)

# TESTS REMOVED:     add_executable(igi_retail_resource_lookup_tests
# TESTS REMOVED:         tests/sys/resource_lookup_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_resource_lookup_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_resource_lookup_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_resource_lookup_tests COMMAND igi_retail_resource_lookup_tests)

# TESTS REMOVED:     add_executable(igi_retail_ilff_navigation_tests
# TESTS REMOVED:         tests/sys/ilff_navigation_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_ilff_navigation_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_ilff_navigation_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_ilff_navigation_tests COMMAND igi_retail_ilff_navigation_tests)

# TESTS REMOVED:     add_executable(igi_retail_camera_transform_tests
# TESTS REMOVED:         tests/render/camera_transform_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_camera_transform_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_camera_transform_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_camera_transform_tests COMMAND igi_retail_camera_transform_tests)

# TESTS REMOVED:     add_executable(igi_retail_trans_context_tests
# TESTS REMOVED:         tests/render/trans_context_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_trans_context_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_trans_context_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_trans_context_tests COMMAND igi_retail_trans_context_tests)

# TESTS REMOVED:     add_executable(igi_retail_mef_tests
# TESTS REMOVED:         tests/model/mef_model_loader_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_mef_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_mef_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_mef_tests COMMAND igi_retail_mef_tests)

# TESTS REMOVED:     add_executable(igi_retail_mtp_tests
# TESTS REMOVED:         tests/model/model_texture_package_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_mtp_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_mtp_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_mtp_tests COMMAND igi_retail_mtp_tests)

# TESTS REMOVED:     add_executable(igi_retail_qvm_tests
# TESTS REMOVED:         tests/script/qvm_compiled_program_test.cpp
# TESTS REMOVED:     )
# TESTS REMOVED:     target_link_libraries(igi_retail_qvm_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_qvm_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_qvm_tests COMMAND igi_retail_qvm_tests)

# TESTS REMOVED:     add_executable(igi_retail_qvm_stack_tests
# TESTS REMOVED:         tests/script/qvm_stack_natives_test.cpp
# TESTS REMOVED:     )
# TESTS REMOVED:     target_link_libraries(igi_retail_qvm_stack_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_qvm_stack_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_qvm_stack_tests COMMAND igi_retail_qvm_stack_tests)

# TESTS REMOVED:     add_executable(igi_retail_qvm_dispatch_tests
# TESTS REMOVED:         tests/script/qvm_instruction_dispatch_test.cpp
# TESTS REMOVED:     )
# TESTS REMOVED:     target_link_libraries(igi_retail_qvm_dispatch_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_qvm_dispatch_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_qvm_dispatch_tests COMMAND igi_retail_qvm_dispatch_tests)

# TESTS REMOVED:     add_executable(igi_retail_qvm_native_loader_tests
# TESTS REMOVED:         tests/script/qvm_native_loader_test.cpp
# TESTS REMOVED:     )
# TESTS REMOVED:     target_link_libraries(igi_retail_qvm_native_loader_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_qvm_native_loader_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_qvm_native_loader_tests COMMAND igi_retail_qvm_native_loader_tests)

# TESTS REMOVED:     add_executable(igi_retail_resource_tests
# TESTS REMOVED:         tests/resource/resource_archive_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_resource_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_resource_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_resource_tests COMMAND igi_retail_resource_tests)

# TESTS REMOVED:     add_executable(igi_retail_texture_tests
# TESTS REMOVED:         tests/texture/loop_terrain_texture_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_texture_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_texture_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_texture_tests COMMAND igi_retail_texture_tests)

# TESTS REMOVED:     add_executable(igi_retail_texture_loaders_tests
# TESTS REMOVED:         tests/texture/texture_loaders_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_texture_loaders_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_texture_loaders_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_texture_loaders_tests COMMAND igi_retail_texture_loaders_tests)

# TESTS REMOVED:     add_executable(igi_retail_terrain_loaders_tests
# TESTS REMOVED:         tests/terrain/terrain_loaders_test.cpp
# TESTS REMOVED:     )

# TESTS REMOVED:     target_link_libraries(igi_retail_terrain_loaders_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:     target_compile_features(igi_retail_terrain_loaders_tests PRIVATE cxx_std_20)
# TESTS REMOVED:     add_test(NAME igi_retail_terrain_loaders_tests COMMAND igi_retail_terrain_loaders_tests)

# TESTS REMOVED: endif()

if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    target_sources(igi_retail_engine PRIVATE src/retail/model/mef_native_attachments.cpp)
# TESTS REMOVED:     if(BUILD_TESTING)
# TESTS REMOVED:         add_executable(igi_retail_terrain_record_tests tests/terrain/terrain_native_records_test.cpp)
# TESTS REMOVED:         target_link_libraries(igi_retail_terrain_record_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:         add_test(NAME igi_retail_terrain_record_tests COMMAND igi_retail_terrain_record_tests)
# TESTS REMOVED:         add_executable(igi_retail_native_differential_tests tests/model/mef_native_differential_test.cpp)
# TESTS REMOVED:         target_link_libraries(igi_retail_native_differential_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:         add_executable(igi_retail_qvm_differential_tests tests/script/qvm_stack_native_differential_test.cpp)
# TESTS REMOVED:         target_link_libraries(igi_retail_qvm_differential_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:         add_executable(igi_retail_qvm_dispatch_differential_tests
# TESTS REMOVED:             tests/script/qvm_dispatch_native_differential_test.cpp)
# TESTS REMOVED:         target_link_libraries(igi_retail_qvm_dispatch_differential_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:         set(IGI_RETAIL_EXECUTABLE "" CACHE FILEPATH "Retail IGI.exe for original-instruction differential tests")
# TESTS REMOVED:         if(IGI_RETAIL_EXECUTABLE)
# TESTS REMOVED:             file(SHA256 "${IGI_RETAIL_EXECUTABLE}" IGI_RETAIL_SHA256)
# TESTS REMOVED:             if(NOT IGI_RETAIL_SHA256 STREQUAL "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01")
# TESTS REMOVED:                 message(FATAL_ERROR "Retail executable digest differs; native addresses need revalidation")
# TESTS REMOVED:             endif()
# TESTS REMOVED:             add_test(NAME igi_retail_native_differential_tests
# TESTS REMOVED:                 COMMAND igi_retail_native_differential_tests "${IGI_RETAIL_EXECUTABLE}")
# TESTS REMOVED:             add_test(NAME igi_retail_qvm_differential_tests
# TESTS REMOVED:                 COMMAND igi_retail_qvm_differential_tests "${IGI_RETAIL_EXECUTABLE}")
# TESTS REMOVED:             add_test(NAME igi_retail_qvm_dispatch_differential_tests
# TESTS REMOVED:                 COMMAND igi_retail_qvm_dispatch_differential_tests "${IGI_RETAIL_EXECUTABLE}")
# TESTS REMOVED:         endif()
# TESTS REMOVED:         add_executable(igi_retail_native_attachment_tests tests/model/mef_native_attachments_test.cpp)
# TESTS REMOVED:         target_link_libraries(igi_retail_native_attachment_tests PRIVATE igi_retail_engine)
# TESTS REMOVED:         add_test(NAME igi_retail_native_attachment_tests COMMAND igi_retail_native_attachment_tests)
# TESTS REMOVED:     endif()
endif()

if(WIN32)
    add_subdirectory(platform/windows)
endif()
