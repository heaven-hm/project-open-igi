cmake_minimum_required(VERSION 3.20)
project(project_open_igi_clean LANGUAGES CXX)

option(IGI_VERIFICATION_ONLY "Configure host-side native checks without building a Windows executable" OFF)
include(CTest)
if(IGI_VERIFICATION_ONLY)
    include(cmake/NativeVerification.cmake)
    return()
endif()

if(NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "The diagnostic baseline requires Windows x86 (PE32).")
endif()

# Target is the retail binary name. The source list is the retail main startup
# flow being certified; other ported units are added only when their headers
# and link dependencies exist on this branch.
add_executable(igi WIN32
    engine/src/retail/audio/sound_adpcm_decoder.cpp
    engine/src/retail/audio/sound_attenuation_table.cpp
    engine/src/retail/audio/sound_ilsf_header.cpp
    engine/src/retail/audio/sound_ilsf_stream_reader.cpp
    engine/src/retail/audio/sound_key_functions.cpp
    engine/src/retail/config/config_graphic_options.cpp
    engine/src/retail/config/config_profile.cpp
    engine/src/retail/config/input_remap.cpp
    engine/src/retail/config/retail_config.cpp
    engine/src/retail/config/retail_input_bindings.cpp
    engine/src/retail/core/app_context.cpp
    engine/src/retail/core/app_context_extra.cpp
    engine/src/retail/core/app_main_commands.cpp
    engine/src/retail/core/cam_task.cpp
    engine/src/retail/core/native_4014b0.cpp
    engine/src/retail/core/native_4014e0.cpp
    engine/src/retail/core/native_4015d0.cpp
    engine/src/retail/core/native_4015f0.cpp
    engine/src/retail/core/native_4cef30.cpp
    engine/src/retail/script/native_401a60.cpp
    engine/src/retail/core/native_4018c0.cpp
    engine/src/retail/core/native_401860.cpp
    engine/src/retail/core/native_401d30.cpp
    engine/src/retail/core/native_4d9730.cpp
    engine/src/retail/dynobj/native_004e5370.cpp
    engine/src/retail/core/native_4c5900.cpp
    engine/src/retail/mem/native_4aad48.cpp
    engine/src/retail/mem/native_4aad90.cpp
    engine/src/retail/mem/mode2_bucket_globals.cpp
    engine/src/retail/mem/native_4ab6dd.cpp
    engine/src/retail/mem/native_4ab733.cpp
    engine/src/retail/mem/native_4ab84c.cpp
    engine/src/retail/mem/native_4aadbb.cpp
    engine/src/retail/mem/native_4a6a50.cpp
    engine/src/retail/mem/native_4b0d10.cpp
    engine/src/retail/core/qhash_slot_table_globals.cpp
    engine/src/retail/core/native_401b20.cpp
    engine/src/retail/script/native_4f0c90.cpp
    engine/src/retail/script/native_4ef830.cpp
    engine/src/retail/script/native_4ef850.cpp
    engine/src/retail/script/native_427d70.cpp
    engine/src/retail/script/native_427f80.cpp
    engine/src/retail/script/native_4efb00.cpp
    engine/src/retail/script/native_4efcd0.cpp
    engine/src/retail/script/native_4eff20.cpp
    engine/src/retail/script/native_4f00a0.cpp
    engine/src/retail/script/native_4f01b0.cpp
    engine/src/retail/script/native_4f64e0.cpp
    engine/src/retail/script/native_51baa0.cpp
    engine/src/retail/script/native_4d3710.cpp
    engine/src/retail/script/native_4f0610.cpp
    engine/src/retail/script/native_4f0640.cpp
    engine/src/retail/script/native_4f06f0.cpp
    engine/src/retail/script/native_4f0710.cpp
    engine/src/retail/script/native_4f0730.cpp
    engine/src/retail/script/script_state_globals_a758.cpp
    engine/src/retail/script/native_4b8b00.cpp
    engine/src/retail/script/native_4f1c20.cpp
    engine/src/retail/script/native_4ef8b0.cpp
    engine/src/retail/script/native_4ef990.cpp
    engine/src/retail/script/native_4ef9b0.cpp
    engine/src/retail/script/native_4ef9d0.cpp
    engine/src/retail/script/native_4efa10.cpp
    engine/src/retail/script/native_4efa90.cpp
    engine/src/retail/script/hash_table_globals.cpp
    engine/src/retail/script/native_4f11a0.cpp
    engine/src/retail/script/native_4f1180.cpp
    engine/src/retail/mem/native_4ab7f5.cpp
    engine/src/retail/core/app_startup_globals.cpp
    engine/src/retail/core/app_startup_natives.cpp
    engine/src/retail/core/class_registration.cpp
    engine/src/retail/core/debug_command_line.cpp
    engine/src/retail/core/flow.cpp
    engine/src/retail/core/fpu_state.cpp
    engine/src/retail/core/frame_natives.cpp
    engine/src/retail/core/game_flow.cpp
    engine/src/retail/core/native_0048f0e0.cpp
    engine/src/retail/core/native_0048f0f0.cpp
    engine/src/retail/core/native_0048f970.cpp
    engine/src/retail/core/qtask_create_task.cpp
    engine/src/retail/core/qtask_list.cpp
    engine/src/retail/core/retail_error.cpp
    engine/src/retail/core/main.cpp
    engine/src/retail/core/native_48f490.cpp
    engine/src/retail/core/native_4c1780.cpp
    engine/src/retail/core/native_489560.cpp
    engine/src/retail/core/native_401810.cpp
    engine/src/retail/core/native_4f1ee0.cpp
    engine/src/retail/core/native_507e70.cpp
    engine/src/retail/core/native_4f9200.cpp
    engine/src/retail/core/native_4f9210.cpp
    engine/src/retail/core/native_4f9220.cpp
    engine/src/retail/core/native_4f9230.cpp
    engine/src/retail/core/native_4f9240.cpp
    engine/src/retail/core/native_4f9250.cpp
    engine/src/retail/core/native_4f9260.cpp
    engine/src/retail/core/native_4f9280.cpp
    engine/src/retail/core/native_48f4f0.cpp
    engine/src/retail/core/task_types.cpp
    engine/src/retail/core/native_401890.cpp
    engine/src/retail/diagnostics/log_record.cpp
    engine/src/retail/dynobj/dynamic_cube.cpp
    engine/src/retail/dynobj/hash_lookup.cpp
    engine/src/retail/dynobj/local_models_mef.cpp
    engine/src/retail/dynobj/mtp_package_loader.cpp
    engine/src/retail/error/error_show.cpp
    engine/src/retail/error/native_4af810.cpp
    engine/src/retail/error/native_4af870.cpp
    engine/src/retail/error/native_4af880.cpp
    engine/src/retail/human/human_camera_eye.cpp
    engine/src/retail/human/human_view.cpp
    engine/src/retail/input/input_port_process.cpp
    engine/src/retail/mem/heap_list.cpp
    engine/src/retail/mem/task_list.cpp
    engine/src/retail/render/native_4f0f80.cpp
    engine/src/retail/render/native_4f1000.cpp
    engine/src/retail/script/native_4ee960.cpp
    engine/src/retail/script/native_4f19c0.cpp
    engine/src/retail/script/native_4f1bc0.cpp
    engine/src/retail/script/native_4f1b70.cpp
    engine/src/retail/script/native_4f1b80.cpp
    engine/src/retail/script/native_4f1b60.cpp
    engine/src/retail/script/native_4f1b90.cpp
    engine/src/retail/script/native_4f1ba0.cpp
    engine/src/retail/script/native_4f1bb0.cpp
    engine/src/retail/script/native_4f1a40.cpp
    engine/src/retail/script/native_4f1a60.cpp
    engine/src/retail/script/native_4f1a70.cpp
    engine/src/retail/script/native_4af910.cpp
    engine/src/retail/script/native_4b8810.cpp
    engine/src/retail/script/native_4b8890.cpp
    engine/src/retail/script/native_4cea10.cpp
    engine/src/retail/script/native_4cea20.cpp
    engine/src/retail/script/native_4baac0.cpp
    engine/src/retail/script/native_4bacd0.cpp
    engine/src/retail/script/native_4c0360.cpp
    engine/src/retail/script/native_4c04b0.cpp
    engine/src/retail/script/native_4c0530.cpp
    engine/src/retail/script/native_4c0560.cpp
    engine/src/retail/script/native_4f1840.cpp
    engine/src/retail/script/native_4f18c0.cpp
    engine/src/retail/script/native_4f1950.cpp
    engine/src/retail/script/native_4f1980.cpp
    engine/src/retail/script/native_4f1a80.cpp
    engine/src/retail/script/native_4f1b20.cpp
    engine/src/retail/script/native_4f1b50.cpp
    engine/src/retail/script/native_4f9460.cpp
    engine/src/retail/script/native_4f9e70.cpp
    engine/src/retail/mem/mem_alloc.cpp
    engine/src/retail/mem/native_4b0d50.cpp
    engine/src/retail/mem/native_4b0d60.cpp
    engine/src/retail/mem/native_4b0d90.cpp
    engine/src/retail/model/iff_chunk_tree.cpp
    engine/src/retail/model/iff_skeleton.cpp
    engine/src/retail/model/mef_bone_rig.cpp
    engine/src/retail/model/mef_model_loader.cpp
    engine/src/retail/model/mef_native_attachments.cpp
    engine/src/retail/model/mef_render_data.cpp
    engine/src/retail/model/mef_skinner.cpp
    engine/src/retail/model/model_texture_package.cpp
    engine/src/retail/qfile/alias_resolve.cpp
    engine/src/retail/render/backend.cpp
    engine/src/retail/render/camera_transform.cpp
    engine/src/retail/render/lod_settings.cpp
    engine/src/retail/render/trans_context.cpp
    engine/src/retail/render/view_block.cpp
    engine/src/retail/resource/device_dispatch.cpp
    engine/src/retail/resource/is_resource_loaded.cpp
    engine/src/retail/resource/resource_archive.cpp
    engine/src/retail/resource/resource_cache.cpp
    engine/src/retail/resource/resource_lifecycle.cpp
    engine/src/retail/resource/resource_unpack.cpp
    engine/src/retail/resource/sys_resource.cpp
    engine/src/retail/script/qvm_compiled_program.cpp
    engine/src/retail/script/qvm_loader.cpp
    engine/src/retail/script/qvm_native_loader.cpp
    engine/src/retail/script/qvm_object_calls.cpp
    engine/src/retail/script/qvm_stack_natives.cpp
    engine/src/retail/script/script_mode.cpp
    engine/src/retail/sys/ilff_navigation.cpp
    engine/src/retail/sys/resource_lookup.cpp
    # All terrain translation units are in the target. terrain_storage.cpp owns the shared retail
    # storage (0x005CA114, 0x00A7E068, 0x00BCAB08, 0x00BCAAE0, 0x00A7F878..0x00A7F88C); those
    # definitions were removed from terrain_draw.cpp so exactly one definition of each exists in
    # the link. terrain_loaders.cpp no longer carries the throwing Terrain_TraverseContourQuadtree
    # and Terrain_Draw placeholders: terrain_quadtree.cpp defines the traversal native and
    # terrain_draw.cpp defines Terrain_Draw() with the retail no-argument ABI.
    # terrain_draw.cpp's Terrain_BuildMipMaps_2 (0x00514760) call resolves to terrain_mipmaps.cpp,
    # whose g_a7e070 (0x00A7E070) definition lives in terrain_draw.cpp; no unit is left out.
    engine/src/retail/terrain/terrain_draw.cpp
    engine/src/retail/terrain/terrain_loaders.cpp
    engine/src/retail/terrain/terrain_mipmaps.cpp
    engine/src/retail/terrain/terrain_out_of_terrain_renderch.cpp
    engine/src/retail/terrain/terrain_quadtree.cpp
    engine/src/retail/terrain/terrain_scene_state.cpp
    engine/src/retail/terrain/terrain_storage.cpp
    engine/src/retail/terrain/terrain_unknown_blendmode.cpp
    engine/src/retail/texture/loop_terrain_texture.cpp
    engine/src/retail/texture/model_texture_pixels.cpp
    engine/src/retail/texture/texture_loaders.cpp
    engine/platform/windows/player_controller.cpp
    engine/platform/windows/retail_animation.cpp
    engine/platform/windows/direct_input_mouse.cpp
    engine/platform/windows/retail_native_verification_support.cpp)
target_include_directories(igi PRIVATE engine/include)
target_compile_features(igi PRIVATE cxx_std_20)
set_target_properties(igi PROPERTIES OUTPUT_NAME "igi1")
if(MINGW)
    # The bottle is Windows 2000-era and ships no api-ms-win-crt-* redistributable,
    # so statically bind the C runtime rather than importing the UCRT DLLs.
    # The bottle is Windows 2000-era and ships no api-ms-win-crt-* or libstdc++-6
    # redistributable, so both the C and C++ runtimes must be fully static. The
    # trailing -Bstatic keeps the subsequent libraries static as well.
    target_link_options(igi PRIVATE -static -static-libgcc -static-libstdc++
                        -Wl,-Bstatic)
    # The retail renders through DirectDraw 7 (igi.exe imports ddraw.dll).
    target_link_libraries(igi PRIVATE ddraw dxguid dinput)
endif()

# Qvm_Execute (0x004B85B0) and Qvm_GetFloatArg (0x004B8A50) are transcribed, but their eight
# retail callees (0x004BD080, 0x004C0200, 0x004BF1C0, 0x004BFC70, 0x004BF1F0, 0x004BFA80,
# 0x004BF8F0, 0x004BEFC0, 0x004BD010, 0x004B9660) and three shared QVM state words are not
# ported yet. They are compiled as a separate object target so every build checks this source
# without introducing undefined symbols into the igi link; move the file into the igi source
# list once those dependencies exist. The object is compile evidence only, not a linked native.
add_library(igi_qvm_execute_object OBJECT engine/src/retail/script/qvm_execute.cpp)
target_include_directories(igi_qvm_execute_object PRIVATE engine/include)
target_compile_features(igi_qvm_execute_object PRIVATE cxx_std_20)
add_dependencies(igi igi_qvm_execute_object)

# The three human player/config natives in humanplayer_defines.cpp are transcribed (0x00410070
# DefineHumanPlayerGeneral, 0x004137A0 HumanPlayer_LoadParameters, 0x00405850 Config_Read), but
# they cannot join the igi link yet. HumanPlayer_LoadParameters and Config_Read call
# Qvm_Load/Qvm_Execute/Qvm_Cleanup/Qvm_GetFloatArg: Qvm_Load and Qvm_Cleanup are already in the
# igi link (qvm_native_loader.cpp), while Qvm_Execute/Qvm_GetFloatArg live in the
# igi_qvm_execute_object unit above, which cannot link until its own ten retail callees and the
# three QVM state words exist. Config_Read's remaining four callees are separate retail natives
# that are not transcribed on this branch: 0x00405300 Config_SaveGraphicOptions,
# 0x004058A0 Config_SaveSoundOptions, 0x00405980 Config_NoD3DDriversWithHard and
# 0x00405B30 Config_Language_2 (13-function closure, see
# docs/fidelity/evidence/humanplayer-defines-link-blockers.json). The unit is therefore compiled
# by every build as an object target so its source is checked without adding undefined symbols to
# the igi link; move it into the igi source list once those dependencies exist. The object is
# compile evidence only, not a linked native.
add_library(igi_humanplayer_defines_object OBJECT engine/src/retail/script/humanplayer_defines.cpp)
target_include_directories(igi_humanplayer_defines_object PRIVATE engine/include)
target_compile_features(igi_humanplayer_defines_object PRIVATE cxx_std_20)
add_dependencies(igi igi_humanplayer_defines_object)

include(cmake/NativeVerification.cmake)

# Testing only: candidate uses IGI1MUTEX instead of retail IGIMUTEX so retail
# igi.exe and igi1.exe can run side by side. Must stay OFF for production.
option(IGI_TESTING_MUTEX "Use IGI1MUTEX so retail and candidate can run together" OFF)
if(IGI_TESTING_MUTEX)
  target_compile_definitions(igi PRIVATE IGI_TESTING_MUTEX)
endif()
