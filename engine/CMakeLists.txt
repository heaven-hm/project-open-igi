add_library(igi_gameplay STATIC
    src/ai_navigation.cpp
    src/game_loop.cpp
    src/game_session.cpp
    src/input_state.cpp
    src/game_assets.cpp
    src/human_motion.cpp
    src/human_locomotion_graph.cpp
    src/native_registry.cpp
    src/mission_world.cpp
    src/world_collision.cpp
    src/software_renderer.cpp
    src/weapon_system.cpp
    src/ai_system.cpp
    src/logger.cpp
    src/player_fall_impact.cpp
    src/weapon_view_sway.cpp
    src/weapon_view_recoil.cpp
    src/player_ladder.cpp
    src/mission_expression.cpp
    src/level_flow.cpp
    src/mission_objective_loader.cpp
)
target_include_directories(igi_gameplay PUBLIC include)
target_compile_features(igi_gameplay PUBLIC cxx_std_20)
target_link_libraries(igi_gameplay PUBLIC igi_asset_formats)
set_target_properties(igi_gameplay PROPERTIES CXX_EXTENSIONS OFF)

add_executable(igi_asset_probe tools/asset_probe.cpp)
target_link_libraries(igi_asset_probe PRIVATE igi_gameplay)
target_compile_features(igi_asset_probe PRIVATE cxx_std_20)
set_target_properties(igi_asset_probe PROPERTIES CXX_EXTENSIONS OFF)

add_executable(igi_native_probe tools/native_probe.cpp)
target_link_libraries(igi_native_probe PRIVATE igi_gameplay)
target_compile_features(igi_native_probe PRIVATE cxx_std_20)
set_target_properties(igi_native_probe PROPERTIES CXX_EXTENSIONS OFF)

add_executable(igi_world_probe tools/world_probe.cpp)
target_link_libraries(igi_world_probe PRIVATE igi_gameplay)
target_compile_features(igi_world_probe PRIVATE cxx_std_20)
set_target_properties(igi_world_probe PROPERTIES CXX_EXTENSIONS OFF)

find_package(SDL2 CONFIG QUIET)
if(TARGET SDL2::SDL2)
    set(IGI_SDL_TARGET SDL2::SDL2)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    set(IGI_SDL_TARGET PkgConfig::SDL2)
endif()

add_executable(igi_game
    src/main.cpp
    platform/sdl/sdl_application.cpp
)
target_link_libraries(igi_game PRIVATE igi_gameplay "${IGI_SDL_TARGET}")
target_compile_features(igi_game PRIVATE cxx_std_20)
set_target_properties(igi_game PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    OUTPUT_NAME "igi"
)

if(APPLE)
    set_target_properties(igi_game PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Project Open IGI"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.heavenhm.project-open-igi"
        MACOSX_BUNDLE_BUNDLE_VERSION "0.1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0"
    )
endif()
