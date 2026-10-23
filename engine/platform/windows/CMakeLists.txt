if(NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "open-igi.exe requires a Windows 32-bit x86 toolchain.")
endif()
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#if !defined(_M_IX86) && !defined(__i386__)
#error Windows x86 compiler required
#endif
int main() { return 0; }
" IGI_WINDOWS_X86_COMPILER)
if(NOT IGI_WINDOWS_X86_COMPILER)
    message(FATAL_ERROR "open-igi.exe requires an Intel x86 compiler, not ARM32.")
endif()

add_executable(open-igi WIN32
    main.cpp
    player_controller.cpp
    retail_animation.cpp
    retail_model_scene.cpp
)

target_compile_features(open-igi PRIVATE cxx_std_20)
target_compile_options(open-igi PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(open-igi PRIVATE igi_retail_engine ddraw dxguid gdi32 shell32)
set_target_properties(open-igi PROPERTIES OUTPUT_NAME "open-igi")

# Verification builds keep every transcribed native in the executable so a
# runtime probe can observe the same native on the retail and the candidate
# side. The linker otherwise discards the archive members the host does not
# call, because the host reimplements their behaviour.
option(IGI_KEEP_ALL_RETAIL_NATIVES
    "Link every retail native into open-igi.exe so a runtime probe can hook it" OFF)
if(IGI_KEEP_ALL_RETAIL_NATIVES AND MINGW)
    target_sources(open-igi PRIVATE retail_native_verification_support.cpp)
    target_link_options(open-igi PRIVATE -Wl,--whole-archive
        "$<TARGET_FILE:igi_retail_engine>" -Wl,--no-whole-archive)
endif()

if(MINGW)
    target_link_options(open-igi PRIVATE -municode -static -static-libgcc -static-libstdc++)
endif()
