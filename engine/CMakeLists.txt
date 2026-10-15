add_library(igi_gameplay STATIC
    src/game_assets.cpp
    src/human_motion.cpp
    src/human_locomotion_graph.cpp
)
target_include_directories(igi_gameplay PUBLIC include)
target_compile_features(igi_gameplay PUBLIC cxx_std_20)
target_link_libraries(igi_gameplay PUBLIC igi_asset_formats)
set_target_properties(igi_gameplay PROPERTIES CXX_EXTENSIONS OFF)

add_executable(igi_asset_probe tools/asset_probe.cpp)
target_link_libraries(igi_asset_probe PRIVATE igi_gameplay)
target_compile_features(igi_asset_probe PRIVATE cxx_std_20)
set_target_properties(igi_asset_probe PROPERTIES CXX_EXTENSIONS OFF)
