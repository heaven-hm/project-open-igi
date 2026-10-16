cmake_minimum_required(VERSION 3.20)
project(project_open_igi VERSION 0.1.0 LANGUAGES C CXX)

# Engine: native IGI port (retal-format MEF/TEX/QVM/GRAPH loading, skeletal
# scene rendering, AI/navigation, collision, gameplay). The engine CMakeLists
# defines every target: igi_gameplay, igi_formats, igi_assets, igi_runtime,
# igi_input, the mvp_* probes and the igi_game application bundle.
include(CTest)

add_subdirectory(engine)
