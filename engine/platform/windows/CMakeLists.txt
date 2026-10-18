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
    retail_model_scene.cpp
)

target_compile_features(open-igi PRIVATE cxx_std_20)
target_compile_options(open-igi PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(open-igi PRIVATE igi_retail_engine ddraw dxguid gdi32 shell32)
set_target_properties(open-igi PROPERTIES OUTPUT_NAME "open-igi")

if(MINGW)
    target_link_options(open-igi PRIVATE -municode -static -static-libgcc -static-libstdc++)
endif()
