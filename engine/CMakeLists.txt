add_library(igi_gameplay STATIC
    src/human_motion.cpp
    src/human_locomotion_graph.cpp
    src/human_movement_integrator.cpp
    src/logger.cpp
    src/exceptions.cpp
)
target_include_directories(igi_gameplay PUBLIC include)
target_compile_features(igi_gameplay PUBLIC cxx_std_20)
set_target_properties(igi_gameplay PROPERTIES CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED CONFIG)
set(IGI_GAME_PATH "$ENV{IGI_GAME_PATH}" CACHE PATH "Original game installation for integration tests")
set(IGI_RETAIL_PARITY_DIRECTORY "" CACHE PATH "Original and port captures required for retail fidelity verification")

set(FORMAT_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/formats")
add_library(igi_formats STATIC
    "${FORMAT_SOURCE}/parsers/mef_native.cpp"
    "${FORMAT_SOURCE}/parsers/tex_parser.cpp"
    "${FORMAT_SOURCE}/parsers/res_parser.cpp"
    "${FORMAT_SOURCE}/parsers/dat_parser.cpp"
    "${FORMAT_SOURCE}/parsers/mtp_parser.cpp"
    "${FORMAT_SOURCE}/parsers/iff_parser.cpp"
    "${FORMAT_SOURCE}/parsers/qvm_parser.cpp"
    "${FORMAT_SOURCE}/parsers/graph_parser.cpp"
    "${FORMAT_SOURCE}/parsers/olm_parser.cpp"
    src/formats/ilsf_sound.cpp
    src/format_support.cpp
    src/diagnostics.cpp
)
target_compile_features(igi_formats PUBLIC cxx_std_20)
target_include_directories(igi_formats
    PUBLIC "${FORMAT_SOURCE}/parsers" "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${FORMAT_SOURCE}"
)
target_include_directories(igi_formats SYSTEM PUBLIC third_party/glm/glm-1.0.1)
target_link_libraries(igi_formats PUBLIC igi_gameplay)

add_library(igi_audio STATIC
    src/audio/sound_volume_curve.cpp
    src/audio/ilsf_stream_reader.cpp
    src/audio/audio_mixer.cpp
    src/audio/sdl_audio_output.cpp
    src/audio/sound_definition_registry.cpp
    src/audio/sound_bank.cpp
    src/audio/sound_system.cpp
)
target_include_directories(igi_audio PUBLIC include)
target_compile_features(igi_audio PUBLIC cxx_std_20)
set_target_properties(igi_audio PROPERTIES CXX_EXTENSIONS OFF)

add_library(igi_assets STATIC
    src/asset_io.cpp
    src/qvm_initializer.cpp
    src/mission_assets.cpp
    src/mission_scene.cpp
    src/mission_gameplay.cpp
    src/input_configuration.cpp
    src/qvm_task_tree.cpp
    src/skeletal_animation.cpp
    src/terrain_surface.cpp
)
target_compile_features(igi_assets PUBLIC cxx_std_20)
target_include_directories(igi_assets PUBLIC include)
target_link_libraries(igi_assets PUBLIC igi_formats)
target_link_libraries(igi_audio PUBLIC igi_assets)

add_executable(igi_inspect_mission tools/inspect_mission.cpp)
target_link_libraries(igi_inspect_mission PRIVATE igi_assets)

add_executable(igi_inspect_scene tools/inspect_scene.cpp)
target_link_libraries(igi_inspect_scene PRIVATE igi_assets)

add_library(igi_runtime STATIC
    src/collision_world.cpp
    src/game_simulation.cpp
    src/game_combat.cpp
    src/weapon_accuracy.cpp
    src/game_inventory.cpp
    src/game_ai.cpp
    src/navigation.cpp
    src/scene_renderer.cpp
    src/weapon_sound_player.cpp
)
target_compile_features(igi_runtime PUBLIC cxx_std_20)
target_link_libraries(igi_audio PUBLIC igi_formats PRIVATE SDL2::SDL2)
target_link_libraries(igi_runtime PUBLIC igi_assets igi_audio igi_gameplay)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(igi_assets PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(igi_runtime PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(igi_game
    src/main.cpp
)
add_library(igi_input STATIC src/input_controller.cpp)
target_compile_features(igi_input PUBLIC cxx_std_20)
target_link_libraries(igi_input PUBLIC igi_runtime SDL2::SDL2)
target_compile_features(igi_game PRIVATE cxx_std_20)
target_link_libraries(igi_game PRIVATE igi_input)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(igi_game PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(TARGET iginatives)
    target_link_libraries(igi_game PRIVATE iginatives)
endif()
target_compile_definitions(igi_game PRIVATE SDL_MAIN_HANDLED)
set_target_properties(igi_game PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(igi_game PROPERTIES OUTPUT_NAME "igi")

foreach(IGI_NUMERIC_TARGET IN ITEMS igi_gameplay igi_formats igi_assets igi_audio igi_runtime igi_input)
    set_target_properties(${IGI_NUMERIC_TARGET} PROPERTIES CXX_EXTENSIONS OFF)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${IGI_NUMERIC_TARGET} PRIVATE -fno-fast-math -ffp-contract=off)
    elseif(MSVC)
        target_compile_options(${IGI_NUMERIC_TARGET} PRIVATE /fp:strict)
    endif()
endforeach()

if(NOT APPLE)
    install(TARGETS igi_game RUNTIME DESTINATION .)
endif()

if(APPLE)
    find_package(SDL3 QUIET CONFIG)
    set_target_properties(igi_game PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Project I.G.I."
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.heavenhm.project-igi-native"
        MACOSX_BUNDLE_BUNDLE_VERSION "0.2.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "0.2.0"
        INSTALL_RPATH "@executable_path/../Frameworks"
        OUTPUT_NAME "igi"
    )

    install(TARGETS igi_game BUNDLE DESTINATION .)
    set(IGI_HAS_RECOVERED_NATIVES FALSE)
    if(TARGET iginatives)
        set(IGI_HAS_RECOVERED_NATIVES TRUE)
        install(TARGETS iginatives LIBRARY DESTINATION "igi.app/Contents/Frameworks")
    endif()
    install(FILES third_party/licenses/igi-formats/LICENSE third_party/licenses/igi-formats/LICENSE_SCOPE.md
        DESTINATION "igi.app/Contents/Resources/Licenses/igi-formats")
    install(FILES third_party/glm/glm-1.0.1/copying.txt
        DESTINATION "igi.app/Contents/Resources/Licenses/glm")
    install(FILES third_party/licenses/igi-editor-ref/LICENSE third_party/licenses/igi-editor-ref/LICENSE_SCOPE.md
        third_party/licenses/igi-editor-ref/LICENSE-PRIOR third_party/licenses/igi-editor-ref/PROVENANCE.md
        DESTINATION "igi.app/Contents/Resources/Licenses/igi-editor-ref")
    install(FILES third_party/licenses/openigi-ref/LICENSE
        DESTINATION "igi.app/Contents/Resources/Licenses/openigi-ref")
    if(TARGET SDL3::SDL3)
        get_target_property(IGI_SDL3_LOCATION SDL3::SDL3 IMPORTED_LOCATION_RELEASE)
        if(NOT IGI_SDL3_LOCATION)
            get_target_property(IGI_SDL3_LOCATION SDL3::SDL3 IMPORTED_LOCATION)
        endif()
        file(REAL_PATH "${IGI_SDL3_LOCATION}" IGI_SDL3_REAL_LOCATION)
        install(FILES "${IGI_SDL3_REAL_LOCATION}"
            DESTINATION "igi.app/Contents/Frameworks" RENAME "libSDL3.dylib")
    endif()
    foreach(SDL_CONFIGURATION_DIRECTORY IN ITEMS "${SDL2_DIR}" "${SDL3_DIR}")
        get_filename_component(SDL_INSTALL_PREFIX "${SDL_CONFIGURATION_DIRECTORY}/../../.." ABSOLUTE)
        foreach(SDL_LICENSE_NAME IN ITEMS sdl2-compat SDL2 SDL3)
            if(EXISTS "${SDL_INSTALL_PREFIX}/share/licenses/${SDL_LICENSE_NAME}")
                file(REAL_PATH "${SDL_INSTALL_PREFIX}/share/licenses/${SDL_LICENSE_NAME}" SDL_LICENSE_DIRECTORY)
                install(DIRECTORY "${SDL_LICENSE_DIRECTORY}"
                    DESTINATION "igi.app/Contents/Resources/Licenses")
            endif()
        endforeach()
    endforeach()
    install(CODE "
        cmake_policy(PUSH)
        cmake_policy(SET CMP0007 NEW)
        set(IGI_FRAMEWORKS \"\${CMAKE_INSTALL_PREFIX}/igi.app/Contents/Frameworks\")
        set(IGI_EXECUTABLE \"\${CMAKE_INSTALL_PREFIX}/igi.app/Contents/MacOS/igi\")
        file(REAL_PATH \"$<TARGET_FILE:SDL2::SDL2>\" IGI_SDL2_REAL_LOCATION)
        execute_process(COMMAND otool -D \"\${IGI_SDL2_REAL_LOCATION}\"
            OUTPUT_VARIABLE IGI_SDL2_ID_TEXT COMMAND_ERROR_IS_FATAL ANY)
        string(REPLACE \"\\n\" \";\" IGI_SDL2_ID_LINES \"\${IGI_SDL2_ID_TEXT}\")
        list(GET IGI_SDL2_ID_LINES 1 IGI_SDL2_ORIGINAL_ID)
        string(STRIP \"\${IGI_SDL2_ORIGINAL_ID}\" IGI_SDL2_ORIGINAL_ID)
        execute_process(COMMAND \"${CMAKE_COMMAND}\" -E copy_if_different
            \"\${IGI_SDL2_REAL_LOCATION}\" \"\${IGI_FRAMEWORKS}/libSDL2.dylib\" COMMAND_ERROR_IS_FATAL ANY)
        execute_process(COMMAND install_name_tool -id @rpath/libSDL2.dylib
            \"\${IGI_FRAMEWORKS}/libSDL2.dylib\" COMMAND_ERROR_IS_FATAL ANY)
        execute_process(COMMAND install_name_tool -change \"\${IGI_SDL2_ORIGINAL_ID}\"
            @executable_path/../Frameworks/libSDL2.dylib \"\${IGI_EXECUTABLE}\" COMMAND_ERROR_IS_FATAL ANY)
        if(${IGI_HAS_RECOVERED_NATIVES})
            execute_process(COMMAND install_name_tool -change @rpath/libiginatives.dylib
                @executable_path/../Frameworks/libiginatives.dylib \"\${IGI_EXECUTABLE}\" COMMAND_ERROR_IS_FATAL ANY)
            execute_process(COMMAND install_name_tool -change \"\${IGI_SDL2_ORIGINAL_ID}\"
                @loader_path/libSDL2.dylib \"\${IGI_FRAMEWORKS}/libiginatives.dylib\" COMMAND_ERROR_IS_FATAL ANY)
        endif()
        foreach(IGI_DETRITUS_ATTRIBUTE IN ITEMS com.apple.FinderInfo com.apple.ResourceFork)
            execute_process(COMMAND xattr -rd \"\${IGI_DETRITUS_ATTRIBUTE}\"
                \"\${CMAKE_INSTALL_PREFIX}/igi.app\" OUTPUT_QUIET ERROR_QUIET)
        endforeach()
        execute_process(
            COMMAND codesign --force --deep --sign - \"\${CMAKE_INSTALL_PREFIX}/igi.app\"
            COMMAND_ERROR_IS_FATAL ANY
        )
        cmake_policy(POP)
    ")
endif()

if(BUILD_TESTING)
    add_executable(sound_volume_curve_test tests/audio/sound_volume_curve_test.cpp)
    target_link_libraries(sound_volume_curve_test PRIVATE igi_audio)
    add_test(NAME retail_sound_volume_curve COMMAND sound_volume_curve_test)

    add_executable(audio_mixer_test tests/audio/audio_mixer_test.cpp)
    target_link_libraries(audio_mixer_test PRIVATE igi_audio)
    add_test(NAME retail_audio_mixer COMMAND audio_mixer_test)

    add_executable(sound_definition_registry_test tests/audio/sound_definition_registry_test.cpp)
    target_link_libraries(sound_definition_registry_test PRIVATE igi_audio)
    if(IGI_GAME_PATH)
        add_test(NAME retail_level_one_sound_definitions
            COMMAND sound_definition_registry_test "${IGI_GAME_PATH}")
    endif()

    add_executable(sound_bank_test tests/audio/sound_bank_test.cpp)
    target_link_libraries(sound_bank_test PRIVATE igi_audio)
    if(IGI_GAME_PATH)
        add_test(NAME retail_level_one_sound_bank
            COMMAND sound_bank_test "${IGI_GAME_PATH}")
    endif()

    add_executable(sound_system_test tests/audio/sound_system_test.cpp)
    target_link_libraries(sound_system_test PRIVATE igi_audio)
    if(IGI_GAME_PATH)
        add_test(NAME retail_level_one_sound_playback
            COMMAND sound_system_test "${IGI_GAME_PATH}")
    endif()

    add_executable(weapon_sound_player_test tests/audio/weapon_sound_player_test.cpp)
    target_link_libraries(weapon_sound_player_test PRIVATE igi_runtime)
    if(IGI_GAME_PATH)
        add_test(NAME retail_level_one_weapon_sound
            COMMAND weapon_sound_player_test "${IGI_GAME_PATH}")
    endif()

    add_executable(sdl_audio_output_test tests/audio/sdl_audio_output_test.cpp)
    target_link_libraries(sdl_audio_output_test PRIVATE igi_audio)
    add_test(NAME sdl_audio_output_callback COMMAND sdl_audio_output_test)
    set_tests_properties(sdl_audio_output_callback PROPERTIES ENVIRONMENT "SDL_AUDIODRIVER=dummy")

    add_executable(ilsf_sound_test tests/audio/ilsf_sound_test.cpp)
    target_link_libraries(ilsf_sound_test PRIVATE igi_audio)
    add_test(NAME retail_ilsf_sound_parser COMMAND ilsf_sound_test)
    if(IGI_GAME_PATH)
        add_test(NAME retail_level_one_ilsf_samples COMMAND ilsf_sound_test "${IGI_GAME_PATH}")
    endif()

    add_executable(retail_random_generator_test tests/retail_random_generator_test.cpp)
    target_include_directories(retail_random_generator_test PRIVATE include)
    target_compile_features(retail_random_generator_test PRIVATE cxx_std_20)
    add_test(NAME retail_rng_captured_checkpoints COMMAND retail_random_generator_test)

    add_executable(weapon_accuracy_test tests/weapon_accuracy_test.cpp)
    target_link_libraries(weapon_accuracy_test PRIVATE igi_runtime)
    add_test(NAME weapon_projectile_accuracy COMMAND weapon_accuracy_test)

    add_test(NAME retail_engine_bit_exact COMMAND "${CMAKE_COMMAND}"
        "-DPARITY_DIRECTORY=${IGI_RETAIL_PARITY_DIRECTORY}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/VerifyRetailParity.cmake")
    set_tests_properties(retail_engine_bit_exact PROPERTIES LABELS "retail;fidelity")
    add_custom_target(igi_verify_retail
        COMMAND "${CMAKE_CTEST_COMMAND}" --test-dir "${CMAKE_BINARY_DIR}" -L retail --output-on-failure
        USES_TERMINAL)
    add_executable(mvp_assets_probe tests/mvp_assets_probe.cpp)
    target_link_libraries(mvp_assets_probe PRIVATE igi_assets)
    add_test(NAME mvp_asset_boundaries COMMAND mvp_assets_probe)

    add_executable(qvm_integer_operands_probe tests/qvm_integer_operands_probe.cpp)
    target_link_libraries(qvm_integer_operands_probe PRIVATE igi_assets)
    add_test(NAME qvm_signed_integer_operands COMMAND qvm_integer_operands_probe)

    add_library(igi_scripting STATIC
        src/scripting/script_interpreter.cpp
        src/scripting/native_function_registry.cpp)
    target_include_directories(igi_scripting PUBLIC include)
    target_compile_features(igi_scripting PUBLIC cxx_std_20)
    set_target_properties(igi_scripting PROPERTIES CXX_EXTENSIONS OFF)
    target_link_libraries(igi_scripting PUBLIC igi_formats)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(igi_scripting PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    add_executable(level_one_qvm_probe tests/level_one_qvm_probe.cpp)
    add_executable(script_integer_arithmetic_test tests/scripting/integer_arithmetic_test.cpp)
    target_link_libraries(script_integer_arithmetic_test PRIVATE igi_scripting)
    add_test(NAME script_integer_arithmetic COMMAND script_integer_arithmetic_test)
    target_link_libraries(level_one_qvm_probe PRIVATE igi_scripting igi_assets)
    if(IGI_GAME_PATH)
        add_test(NAME level_one_original_ai_script COMMAND level_one_qvm_probe "${IGI_GAME_PATH}")
    endif()

    add_executable(mvp_renderer_probe tests/mvp_renderer_probe.cpp)
    target_link_libraries(mvp_renderer_probe PRIVATE igi_runtime)
    add_test(NAME mvp_renderer_depth_clipping COMMAND mvp_renderer_probe)
    add_executable(mvp_olm_probe tests/mvp_olm_probe.cpp)
    target_link_libraries(mvp_olm_probe PRIVATE igi_assets)
    add_test(NAME mvp_object_lightmap_format COMMAND mvp_olm_probe)
    add_executable(mvp_lightmap_task_tree_probe tests/mvp_lightmap_task_tree_probe.cpp)
    target_link_libraries(mvp_lightmap_task_tree_probe PRIVATE igi_assets)
    add_test(NAME mvp_lightmap_task_tree COMMAND mvp_lightmap_task_tree_probe)
    if(IGI_GAME_PATH)
        add_test(NAME mvp_original_object_lightmap_format
                 COMMAND mvp_olm_probe "${IGI_GAME_PATH}")
        add_test(NAME mvp_original_lightmap_task_tree
                 COMMAND mvp_lightmap_task_tree_probe "${IGI_GAME_PATH}")
    endif()
    add_executable(mvp_collision_probe tests/mvp_collision_probe.cpp)
    target_link_libraries(mvp_collision_probe PRIVATE igi_runtime)
    add_test(NAME mvp_collision_world COMMAND mvp_collision_probe)

    add_executable(mvp_gameplay_probe tests/mvp_gameplay_probe.cpp)
    target_link_libraries(mvp_gameplay_probe PRIVATE igi_runtime)
    add_test(NAME mvp_movement_weapon_ai COMMAND mvp_gameplay_probe)

    add_executable(mvp_navigation_inventory_probe tests/mvp_navigation_inventory_probe.cpp)
    target_link_libraries(mvp_navigation_inventory_probe PRIVATE igi_runtime)
    add_test(NAME mvp_navigation_inventory COMMAND mvp_navigation_inventory_probe)
    add_executable(mvp_input_probe tests/mvp_input_probe.cpp)
    target_link_libraries(mvp_input_probe PRIVATE igi_input)
    add_test(NAME mvp_config_input COMMAND mvp_input_probe)
    add_executable(mvp_skeletal_probe tests/mvp_skeletal_probe.cpp)
    target_link_libraries(mvp_skeletal_probe PRIVATE igi_runtime)
    add_test(NAME mvp_skeletal_animation COMMAND mvp_skeletal_probe)

    if(IGI_GAME_PATH)
        add_test(NAME mvp_original_config_input COMMAND mvp_input_probe "${IGI_GAME_PATH}")
        add_executable(mvp_real_scene_probe tests/mvp_real_scene_probe.cpp)
        target_link_libraries(mvp_real_scene_probe PRIVATE igi_runtime)
        add_test(NAME mvp_original_level_gameplay COMMAND mvp_real_scene_probe
            "${IGI_GAME_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/test-cache/assets")
        add_test(NAME igi_game_headless_loop COMMAND igi_game --headless --frames 3
            --game "${IGI_GAME_PATH}" --cache "${CMAKE_CURRENT_BINARY_DIR}/test-cache")
        add_executable(mvp_level_two_probe tests/mvp_level_two_probe.cpp)
        target_link_libraries(mvp_level_two_probe PRIVATE igi_runtime)
        add_test(NAME mvp_original_level_two COMMAND mvp_level_two_probe
            "${IGI_GAME_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/test-cache/assets")
        add_test(NAME igi_original_style_level_selection COMMAND igi_game window level2 --headless --frames 3
            --game "${IGI_GAME_PATH}" --cache "${CMAKE_CURRENT_BINARY_DIR}/test-cache")
        add_test(NAME igi_reject_invalid_level COMMAND igi_game window level15 --headless)
        set_tests_properties(igi_reject_invalid_level PROPERTIES WILL_FAIL TRUE)
        add_test(NAME igi_graphics_alias COMMAND igi_game window level2 --gfx high --headless --frames 1
            --game "${IGI_GAME_PATH}" --cache "${CMAKE_CURRENT_BINARY_DIR}/test-cache")
        add_test(NAME igi_vanilla_graphics_preset COMMAND igi_game window level1 --graphics vanilla --headless --frames 1
            --game "${IGI_GAME_PATH}" --cache "${CMAKE_CURRENT_BINARY_DIR}/test-cache")
        add_test(NAME igi_ultra_graphics_preset COMMAND igi_game window level1 --graphics ultra --headless --frames 1
            --game "${IGI_GAME_PATH}" --cache "${CMAKE_CURRENT_BINARY_DIR}/test-cache")
        add_test(NAME igi_reject_invalid_graphics COMMAND igi_game --graphics invalid --headless)
        set_tests_properties(igi_reject_invalid_graphics PROPERTIES WILL_FAIL TRUE)
        add_test(NAME igi_reject_invalid_texture_anisotropy
            COMMAND igi_game --texture-anisotropy 3 --headless)
        set_tests_properties(igi_reject_invalid_texture_anisotropy PROPERTIES WILL_FAIL TRUE)
    endif()

    add_executable(human_movement_integrator_probe
        tests/human_movement_integrator_probe.cpp
    )
    target_compile_features(human_movement_integrator_probe PRIVATE cxx_std_20)
    target_link_libraries(human_movement_integrator_probe PRIVATE igi_gameplay)
    set_target_properties(human_movement_integrator_probe PROPERTIES CXX_EXTENSIONS OFF)
    add_test(NAME human_movement_integrator_order COMMAND human_movement_integrator_probe)
endif()
