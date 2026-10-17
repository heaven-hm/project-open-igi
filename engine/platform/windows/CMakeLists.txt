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
