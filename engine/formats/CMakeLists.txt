set(IGI_CONVERTER_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/source")
set(IGI_CONVERTER_PARSERS "${IGI_CONVERTER_SOURCE}/parsers")
file(GLOB IGI_FORMAT_SOURCES CONFIGURE_DEPENDS "${IGI_CONVERTER_PARSERS}/*.cpp")

# MTPTool is a Windows editor workflow, not a runtime asset decoder.
list(REMOVE_ITEM IGI_FORMAT_SOURCES "${IGI_CONVERTER_PARSERS}/mtp_tool.cpp")

add_library(igi_asset_formats STATIC
    ${IGI_FORMAT_SOURCES}
    vendor/igi1conv/support.cpp
)
target_include_directories(igi_asset_formats PUBLIC
    "${IGI_CONVERTER_SOURCE}"
    "${IGI_CONVERTER_PARSERS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/igi1conv"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/third_party/glm-1.0.1"
)
target_compile_features(igi_asset_formats PUBLIC cxx_std_20)
set_target_properties(igi_asset_formats PROPERTIES CXX_EXTENSIONS OFF)
