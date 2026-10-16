add_executable(igi_tests
    test_runner.cpp
    test_parsers.cpp
    test_physics.cpp
    test_weapons.cpp
    test_ai.cpp
    test_logger_and_exceptions.cpp
    test_gameplay_helpers.cpp
    test_mission_flow.cpp
    test_graphics_quality.cpp
    test_retail_controls.cpp
    test_ai_navigation.cpp
    test_building_entry.cpp
    test_magic_objects.cpp
    test_combat_integration.cpp
)

target_include_directories(igi_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../engine/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../engine/formats/vendor/source"
    "${CMAKE_CURRENT_SOURCE_DIR}/../engine/formats/vendor/source/parsers"
    "${CMAKE_CURRENT_SOURCE_DIR}/../engine/formats/vendor/igi1conv"
    "${CMAKE_CURRENT_SOURCE_DIR}/../engine/formats/vendor/third_party/glm-1.0.1"
)

target_link_libraries(igi_tests PRIVATE igi_gameplay)
target_compile_features(igi_tests PRIVATE cxx_std_20)
set_target_properties(igi_tests PROPERTIES CXX_EXTENSIONS OFF)

add_test(NAME igi_tests COMMAND igi_tests)
