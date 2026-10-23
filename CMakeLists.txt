cmake_minimum_required(VERSION 3.20)
project(project_open_igi_clean LANGUAGES CXX)

if(NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "The diagnostic baseline requires Windows x86 (PE32).")
endif()

# Target is the retail binary name. The source list is the retail main startup
# flow being certified; other ported units are added only when their headers
# and link dependencies exist on this branch.
add_executable(igi WIN32
    engine/platform/windows/scene_main.cpp
    engine/src/retail/audio/sound_adpcm_decoder.cpp
    engine/src/retail/audio/sound_attenuation_table.cpp
    engine/src/retail/audio/sound_ilsf_header.cpp
    engine/src/retail/audio/sound_ilsf_stream_reader.cpp
    engine/src/retail/audio/sound_key_functions.cpp
    engine/src/retail/config/config_graphic_options.cpp
    engine/src/retail/config/input_remap.cpp
    engine/src/retail/config/retail_config.cpp
    engine/src/retail/config/retail_input_bindings.cpp
    engine/src/retail/core/app_context.cpp
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
    engine/src/retail/core/retail_error.cpp
    engine/src/retail/core/retail_main.cpp
    engine/src/retail/core/task_types.cpp
    engine/src/retail/diagnostics/log_record.cpp
    engine/src/retail/dynobj/dynamic_cube.cpp
    engine/src/retail/dynobj/hash_lookup.cpp
    engine/src/retail/dynobj/local_models_mef.cpp
    engine/src/retail/dynobj/mtp_package_loader.cpp
    engine/src/retail/error/error_show.cpp
    engine/src/retail/human/human_camera_eye.cpp
    engine/src/retail/human/human_view.cpp
    engine/src/retail/input/input_port_process.cpp
    engine/src/retail/mem/heap_list.cpp
    engine/src/retail/mem/mem_alloc.cpp
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
    engine/platform/windows/retail_model_scene.cpp
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
