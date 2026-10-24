# Diagnostic-only: compile the one candidate and a source-bound startup test copy.
# The root production source list is unchanged. DEFER runs after igi exists.
function(configure_directx_candidate)
    get_target_property(sources igi SOURCES)
    list(REMOVE_ITEM sources engine/src/retail/core/main.cpp)
    set_property(TARGET igi PROPERTY SOURCES "${sources}")
    target_sources(igi PRIVATE
        "${CMAKE_SOURCE_DIR}/build/native-analysis/directx-candidate-sol/main.cpp"
        "${CMAKE_SOURCE_DIR}/scripts/GameNatives/display/00498480-Native_00498480.cpp")
    # Retail ends these paths with CALL / epilogue / RET, not a sibling jump.
    set_source_files_properties(
        "${CMAKE_SOURCE_DIR}/scripts/GameNatives/display/00498480-Native_00498480.cpp"
        PROPERTIES COMPILE_OPTIONS "-fno-optimize-sibling-calls")
endfunction()
cmake_language(DEFER CALL configure_directx_candidate)
