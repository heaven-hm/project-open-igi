cmake_minimum_required(VERSION 3.20)
project(project_open_igi_clean LANGUAGES CXX)
include(cmake/NativeAutoVerification.cmake)

option(IGI_VERIFICATION_ONLY "Configure host-side native checks without building a Windows executable" OFF)
include(CTest)
if(IGI_VERIFICATION_ONLY)
    include(cmake/NativeVerification.cmake)
    return()
endif()

if(NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "The diagnostic baseline requires Windows x86 (PE32).")
endif()

# Target is the retail binary name. The source list is the retail main startup
# flow being certified; other ported units are added only when their headers
# and link dependencies exist on this branch.
add_executable(igi WIN32
    engine/src/retail/audio/sound_adpcm_decoder.cpp
    engine/src/retail/audio/sound_attenuation_table.cpp
    engine/src/retail/audio/sound_ilsf_header.cpp
    engine/src/retail/audio/sound_ilsf_stream_reader.cpp
    engine/src/retail/audio/sound_key_functions.cpp
    engine/src/retail/config/config_graphic_options.cpp
    engine/src/retail/config/config_profile.cpp
    engine/src/retail/config/input_remap.cpp
    engine/src/retail/config/retail_config.cpp
    engine/src/retail/config/retail_input_bindings.cpp
    engine/src/retail/core/app_context.cpp
    engine/src/retail/core/app_context_extra.cpp
    engine/src/retail/core/app_main_commands.cpp
    engine/src/retail/core/cam_task.cpp
    engine/src/retail/core/native_4014b0.cpp
    engine/src/retail/core/native_4014e0.cpp
    engine/src/retail/core/native_4015d0.cpp
    engine/src/retail/core/native_4015f0.cpp
    engine/src/retail/core/native_4cef30.cpp
    engine/src/retail/core/native_4e7530.cpp
    engine/src/retail/core/native_4f0fa0.cpp
    engine/src/retail/core/native_4f0fd0.cpp
    engine/src/retail/core/native_413b20.cpp
    engine/src/retail/core/native_401a20.cpp
    engine/src/retail/core/native_401900.cpp
    engine/src/retail/core/native_42dac0.cpp
    engine/src/retail/core/native_42db00.cpp
    engine/src/retail/core/native_42dce0.cpp
    engine/src/retail/core/native_42fe40.cpp
    engine/src/retail/core/native_42fe80.cpp
    engine/src/retail/core/native_42fef0.cpp
    engine/src/retail/mem/native_4af930.cpp
    engine/src/retail/script/native_4b8a20.cpp
    engine/src/retail/script/native_4b8a50.cpp
    engine/src/retail/script/native_4b8a80.cpp
    engine/src/retail/script/native_4b8ce0.cpp
    engine/src/retail/script/native_4b9040.cpp
    engine/src/retail/script/native_4b9120.cpp
    engine/src/retail/script/native_4b9660.cpp
    engine/src/retail/script/native_4bab80.cpp
    engine/src/retail/script/native_4babb0.cpp
    engine/src/retail/script/native_4bac30.cpp
    engine/src/retail/script/native_4bcab0.cpp
    engine/src/retail/script/native_4bcae0.cpp
    engine/src/retail/script/native_4bcc00.cpp
    engine/src/retail/script/native_4bccc0.cpp
    engine/src/retail/script/native_4bcd90.cpp
    engine/src/retail/script/native_4bce40.cpp
    engine/src/retail/script/native_4bcec0.cpp
    engine/src/retail/script/native_4bd010.cpp
    engine/src/retail/script/native_4bd040.cpp
    engine/src/retail/script/native_4c0620.cpp
    engine/src/retail/core/native_4192e0.cpp
    engine/src/retail/core/native_419330.cpp
    engine/src/retail/core/native_419540.cpp
    engine/src/retail/core/native_419630.cpp
    engine/src/retail/core/native_419760.cpp
    engine/src/retail/core/native_4197f0.cpp
    engine/src/retail/core/native_419800.cpp
    engine/src/retail/core/native_419820.cpp
    engine/src/retail/core/native_419860.cpp
    engine/src/retail/core/native_419950.cpp
    engine/src/retail/core/native_00498480.cpp
    engine/src/retail/core/native_0046df10.cpp
    engine/src/retail/core/native_0046ba60.cpp
    engine/src/retail/core/native_00467d90.cpp
    engine/src/retail/core/native_0047e700.cpp
    engine/src/retail/core/native_00476110.cpp
    engine/src/retail/core/native_00401fa0.cpp
    engine/src/retail/core/native_00460bf0.cpp
    engine/src/retail/core/native_004e6610.cpp
    engine/src/retail/core/native_413b30.cpp
    engine/src/retail/core/native_413b40.cpp
    engine/src/retail/core/native_460be0.cpp
    engine/src/retail/core/native_460c30.cpp
    engine/src/retail/core/native_509390.cpp
    engine/src/retail/core/native_5093c0.cpp
    engine/src/retail/core/native_509340.cpp
    engine/src/retail/core/native_401ae0.cpp
    engine/src/retail/core/native_401cf0.cpp
    engine/src/retail/core/native_4f1030.cpp
    engine/src/retail/core/native_4f1240.cpp
    engine/src/retail/core/native_4f1090.cpp
    engine/src/retail/core/native_4fa850.cpp
    engine/src/retail/core/native_5092e0.cpp
    engine/src/retail/core/native_4f90b0.cpp
    engine/src/retail/core/native_4f1050.cpp
    engine/src/retail/core/native_402f50.cpp
    engine/src/retail/core/native_47cab0.cpp
    engine/src/retail/core/native_472a70.cpp
    engine/src/retail/core/native_46b510.cpp
    engine/src/retail/core/native_46b7f0.cpp
    engine/src/retail/dynobj/native_43e4f0.cpp
    engine/src/retail/core/native_4688b0.cpp
    engine/src/retail/core/native_468e20.cpp
    engine/src/retail/core/native_4851b0.cpp
    engine/src/retail/core/native_46ab60.cpp
    engine/src/retail/core/native_485a40.cpp
    engine/src/retail/core/native_484f30.cpp
    engine/src/retail/core/native_4f9180.cpp
    engine/src/retail/core/native_4f92e0.cpp
    engine/src/retail/core/native_4f93b0.cpp
    engine/src/retail/model/native_4ceba0.cpp
    engine/src/retail/core/native_460c40.cpp
    engine/src/retail/core/native_460c50.cpp
    engine/src/retail/core/native_47e990.cpp
    engine/src/retail/core/native_460c10.cpp
    engine/src/retail/core/native_0046dd00.cpp
    engine/src/retail/core/native_0047b4b0.cpp
    engine/src/retail/core/native_004825c0.cpp
    engine/src/retail/render/native_00407d60.cpp
    engine/src/retail/audio/native_004ebc60.cpp
    engine/src/retail/core/native_00530a80.cpp
    engine/src/retail/core/native_005310e0.cpp
    engine/src/retail/dynobj/native_0051a1d0.cpp
    engine/src/retail/model/native_4b6ce0.cpp
    engine/src/retail/model/native_4b6d20.cpp
    engine/src/retail/render/native_00491db0.cpp
    engine/src/retail/render/native_00491dc0.cpp
    engine/src/retail/render/native_00491dd0.cpp
    engine/src/retail/render/native_00491de0.cpp
    engine/src/retail/render/native_00491df0.cpp
    engine/src/retail/render/native_00491e00.cpp
    engine/src/retail/render/native_00491e10.cpp
    engine/src/retail/render/native_00491e20.cpp
    engine/src/retail/render/native_00491e30.cpp
    engine/src/retail/render/native_00491e40.cpp
    engine/src/retail/render/native_00491e50.cpp
    engine/src/retail/render/native_00491e60.cpp
    engine/src/retail/render/native_004da1d0.cpp
    engine/src/retail/render/native_004e7c50.cpp
    engine/src/retail/render/native_490340.cpp
    engine/src/retail/render/native_490350.cpp
    engine/src/retail/script/native_5186c0.cpp
    engine/src/retail/core/native_424280.cpp
    engine/src/retail/core/native_424b90.cpp
    engine/src/retail/core/native_424c10.cpp
    engine/src/retail/core/native_424ca0.cpp
    engine/src/retail/core/native_468bb0.cpp
    engine/src/retail/core/native_488700.cpp
    engine/src/retail/qfile/native_4b11a0.cpp
    engine/src/retail/qfile/native_4b1420.cpp
    engine/src/retail/qfile/native_4b1b90.cpp
    engine/src/retail/qfile/native_4b12d0.cpp
    engine/src/retail/qfile/native_4b1be0.cpp
    engine/src/retail/qfile/native_4b1c00.cpp
    engine/src/retail/qfile/native_4b1c20.cpp
    engine/src/retail/qfile/native_4b1c40.cpp
    engine/src/retail/qfile/native_4b1c60.cpp
    engine/src/retail/qfile/native_4b1c80.cpp
    engine/src/retail/qfile/native_4b1ca0.cpp
    engine/src/retail/qfile/native_4b1cc0.cpp
    engine/src/retail/qfile/native_4b1ce0.cpp
    engine/src/retail/qfile/native_4b1d40.cpp
    engine/src/retail/qfile/native_4b1d20.cpp
    engine/src/retail/qfile/native_4b1d00.cpp
    engine/src/retail/qfile/native_4b1180.cpp
    engine/src/retail/qfile/native_4b1700.cpp
    engine/src/retail/qfile/native_4b1760.cpp
    engine/src/retail/qfile/native_4b17a0.cpp
    engine/src/retail/qfile/native_4b17c0.cpp
    engine/src/retail/qfile/native_4b1730.cpp
    engine/src/retail/qfile/native_4b0db0.cpp
    engine/src/retail/dynobj/native_4bf1e0.cpp
    engine/src/retail/dynobj/native_4c0170.cpp
    engine/src/retail/dynobj/native_4c0230.cpp
    engine/src/retail/dynobj/native_4c0240.cpp
    engine/src/retail/dynobj/native_4c56d0.cpp
    engine/src/retail/dynobj/native_4c56e0.cpp
    engine/src/retail/dynobj/native_4c56f0.cpp
    engine/src/retail/dynobj/native_4c5700.cpp
    engine/src/retail/dynobj/native_4c5800.cpp
    engine/src/retail/render/native_4d14c0.cpp
    engine/src/retail/render/native_4d14d0.cpp
    engine/src/retail/core/native_509860.cpp
    engine/src/retail/sys/native_4af860.cpp
    engine/src/retail/sys/native_4978d0.cpp
    engine/src/retail/sys/native_4af960.cpp
    engine/src/retail/sys/native_4978f0.cpp
    engine/src/retail/sys/native_4af890.cpp
    engine/src/retail/sys/native_4af8a0.cpp
    engine/src/retail/sys/native_4af8b0.cpp
    engine/src/retail/sys/native_4b0eb0.cpp
    engine/src/retail/sys/native_4b0e00.cpp
    engine/src/retail/sys/native_4b0e70.cpp
    engine/src/retail/qfile/native_4b1290.cpp
    engine/src/retail/qfile/native_4b0b70.cpp
    engine/src/retail/qfile/native_4b1020.cpp
    engine/src/retail/qfile/native_4b1fb0.cpp
    engine/src/retail/qfile/native_4b12f0.cpp
    engine/src/retail/qfile/native_4b16d0.cpp
    engine/src/retail/qfile/native_4b19c0.cpp
    engine/src/retail/config/native_4071d0.cpp
    engine/src/retail/config/native_4167d0.cpp
    engine/src/retail/config/native_416870.cpp
    engine/src/retail/config/native_416930.cpp
    engine/src/retail/config/native_4169d0.cpp
    engine/src/retail/config/native_416860.cpp
    engine/src/retail/config/native_4167e0.cpp
    engine/src/retail/config/native_4167f0.cpp
    engine/src/retail/config/native_416810.cpp
    engine/src/retail/config/native_416800.cpp
    engine/src/retail/config/native_416820.cpp
    engine/src/retail/config/native_416830.cpp
    engine/src/retail/config/native_416850.cpp
    engine/src/retail/config/native_416840.cpp
    engine/src/retail/config/native_416880.cpp
    engine/src/retail/config/native_416890.cpp
    engine/src/retail/sys/native_4aa0a0.cpp
    engine/src/retail/sys/native_505d40.cpp
    engine/src/retail/sys/native_505c80.cpp
    engine/src/retail/sys/native_504df0.cpp
    engine/src/retail/sys/native_5042a0.cpp
    engine/src/retail/sys/native_503bf0.cpp
    engine/src/retail/sys/native_5034d0.cpp
    engine/src/retail/sys/native_4fc860.cpp
    engine/src/retail/sys/native_4fc840.cpp
    engine/src/retail/sys/native_4fb5e0.cpp
    engine/src/retail/sys/native_4f4860.cpp
    engine/src/retail/sys/native_4f4850.cpp
    engine/src/retail/sys/native_4f45f0.cpp
    engine/src/retail/sys/native_4f4140.cpp
    engine/src/retail/sys/native_4f4130.cpp
    engine/src/retail/sys/native_4f4120.cpp
    engine/src/retail/sys/native_4f4110.cpp
    engine/src/retail/sys/native_4f40e0.cpp
    engine/src/retail/sys/native_4f40d0.cpp
    engine/src/retail/sys/native_4f2040.cpp
    engine/src/retail/sys/native_4f2010.cpp
    engine/src/retail/sys/native_4f2000.cpp
    engine/src/retail/sys/native_4f1ff0.cpp
    engine/src/retail/sys/native_4f1fe0.cpp
    engine/src/retail/sys/native_4f1fd0.cpp
    engine/src/retail/sys/native_4f1200.cpp
    engine/src/retail/sys/native_4ee100.cpp
    engine/src/retail/sys/native_4eee0.cpp
    engine/src/retail/sys/native_4eef0.cpp
    engine/src/retail/sys/native_4ee0f0.cpp
    engine/src/retail/sys/native_4ee0e0.cpp
    engine/src/retail/sys/native_4e5350.cpp
    engine/src/retail/sys/native_4e0830.cpp
    engine/src/retail/sys/native_4dee80.cpp
    engine/src/retail/sys/native_4af8f0.cpp
    engine/src/retail/sys/native_4d9850.cpp
    engine/src/retail/sys/native_4d97e0.cpp
    engine/src/retail/sys/native_489630.cpp
    engine/src/retail/sys/native_489620.cpp
    engine/src/retail/sys/native_489610.cpp
    engine/src/retail/sys/native_487190.cpp
    engine/src/retail/sys/native_481340.cpp
    engine/src/retail/sys/native_47e6f0.cpp
    engine/src/retail/sys/native_47b100.cpp
    engine/src/retail/sys/native_477bf0.cpp
    engine/src/retail/sys/native_46ab20.cpp
    engine/src/retail/sys/native_448550.cpp
    engine/src/retail/sys/native_443e80.cpp
    engine/src/retail/sys/native_430950.cpp
    engine/src/retail/sys/native_42fdc0.cpp
    engine/src/retail/sys/native_424b80.cpp
    engine/src/retail/sys/native_4f1e20.cpp
    engine/src/retail/sys/native_4f1e10.cpp
    engine/src/retail/sys/native_4f1e00.cpp
    engine/src/retail/sys/native_4f0ff0.cpp
    engine/src/retail/sys/native_4f0fb0.cpp
    engine/src/retail/sys/native_4f0f90.cpp
    engine/src/retail/sys/native_4f0f60.cpp
    engine/src/retail/sys/native_4f0f40.cpp
    engine/src/retail/sys/native_4f0f30.cpp
    engine/src/retail/sys/native_4f0f20.cpp
    engine/src/retail/sys/native_4f0f00.cpp
    engine/src/retail/sys/native_4f0ef0.cpp
    engine/src/retail/sys/native_4d0d80.cpp
    engine/src/retail/sys/native_4950e0.cpp
    engine/src/retail/sys/native_481350.cpp
    engine/src/retail/sys/native_443e90.cpp
    engine/src/retail/sys/native_424890.cpp
    engine/src/retail/sys/native_424880.cpp
    engine/src/retail/sys/native_424870.cpp
    engine/src/retail/sys/native_448560.cpp
    engine/src/retail/sys/native_4f2030.cpp
    engine/src/retail/sys/native_4f2020.cpp
    engine/src/retail/sys/native_4871b0.cpp
    engine/src/retail/sys/native_4b8970.cpp
    engine/src/retail/sys/native_4f1cd0.cpp
    engine/src/retail/sys/native_4f1d60.cpp
    engine/src/retail/sys/native_4abc46.cpp
    engine/src/retail/sys/native_4f0fc0.cpp
    engine/src/retail/sys/native_4f1210.cpp
    engine/src/retail/sys/native_4f1220.cpp
    engine/src/retail/sys/native_4f1010.cpp
    engine/src/retail/sys/native_4d9fa0.cpp
    engine/src/retail/sys/native_4f0f50.cpp
    engine/src/retail/sys/native_4f0f70.cpp
    engine/src/retail/sys/native_4c48c0.cpp
    engine/src/retail/sys/native_424850.cpp
    engine/src/retail/sys/native_4f0fe0.cpp
    engine/src/retail/sys/native_4d9fb0.cpp
    engine/src/retail/sys/native_4ad36e.cpp
    engine/src/retail/sys/native_4b4720.cpp
    engine/src/retail/sys/native_4f1c90.cpp
    engine/src/retail/sys/native_4b0dc0.cpp
    engine/src/retail/sys/native_4b0f90.cpp
    engine/src/retail/sys/native_4b0fd0.cpp
    engine/src/retail/sys/native_4b0e50.cpp
    engine/src/retail/sys/native_4b0f30.cpp
    engine/src/retail/resource/native_46b4d0.cpp
    engine/src/retail/resource/native_46b040.cpp
    engine/src/retail/resource/native_4b6420.cpp
    engine/src/retail/resource/native_4b64f0.cpp
    engine/src/retail/sys/native_4b0ee0.cpp
    engine/src/retail/sys/native_4b0ec0.cpp
    engine/src/retail/sys/native_4b0f70.cpp
    engine/src/retail/sys/native_4b1000.cpp
    engine/src/retail/qfile/native_4b1bc0.cpp
    engine/src/retail/qfile/native_4b1a90.cpp
    engine/src/retail/qfile/native_4b1620.cpp
    engine/src/retail/qfile/native_4b13a0.cpp
    engine/src/retail/core/native_509870.cpp
    engine/src/retail/core/native_509880.cpp
    engine/src/retail/core/native_509890.cpp
    engine/src/retail/core/native_5098a0.cpp
    engine/src/retail/qfile/native_4b1d60.cpp
    engine/src/retail/qfile/native_4b1d80.cpp
    engine/src/retail/qfile/native_4b1dc0.cpp
    engine/src/retail/qfile/native_4b1da0.cpp
    engine/src/retail/qfile/native_50a5a0.cpp
    engine/src/retail/qfile/native_50a5b0.cpp
    engine/src/retail/qfile/native_4b1de0.cpp
    engine/src/retail/script/native_401a60.cpp
    engine/src/retail/core/native_4018c0.cpp
    engine/src/retail/core/native_401860.cpp
    engine/src/retail/core/native_401d30.cpp
    engine/src/retail/core/native_4d9730.cpp
    engine/src/retail/dynobj/native_004e5370.cpp
    engine/src/retail/core/native_4c5900.cpp
    engine/src/retail/mem/native_4aad48.cpp
    engine/src/retail/mem/native_4aad90.cpp
    engine/src/retail/mem/mode2_bucket_globals.cpp
    engine/src/retail/mem/native_4ab6dd.cpp
    engine/src/retail/mem/native_4ab733.cpp
    engine/src/retail/mem/native_4ab84c.cpp
    engine/src/retail/mem/native_4aadbb.cpp
    engine/src/retail/mem/native_4a6a50.cpp
    engine/src/retail/mem/native_4b0d10.cpp
    engine/src/retail/core/qhash_slot_table_globals.cpp
    engine/src/retail/core/native_401b20.cpp
    engine/src/retail/script/native_4f0c90.cpp
    engine/src/retail/script/native_4ef830.cpp
    engine/src/retail/script/native_4ef850.cpp
    engine/src/retail/script/native_427d70.cpp
    engine/src/retail/script/native_427f80.cpp
    engine/src/retail/script/native_4efb00.cpp
    engine/src/retail/script/native_4efcd0.cpp
    engine/src/retail/script/native_4eff20.cpp
    engine/src/retail/script/native_4f00a0.cpp
    engine/src/retail/script/native_4f01b0.cpp
    engine/src/retail/script/native_4f64e0.cpp
    engine/src/retail/script/native_51baa0.cpp
    engine/src/retail/script/native_4d3710.cpp
    engine/src/retail/script/native_4f0610.cpp
    engine/src/retail/script/native_4f0640.cpp
    engine/src/retail/script/native_4f06f0.cpp
    engine/src/retail/script/native_4f0710.cpp
    engine/src/retail/script/native_4f0730.cpp
    engine/src/retail/script/script_state_globals_a758.cpp
    engine/src/retail/script/native_4b8b00.cpp
    engine/src/retail/script/native_4f1c20.cpp
    engine/src/retail/script/native_4ef8b0.cpp
    engine/src/retail/script/native_4ef990.cpp
    engine/src/retail/script/native_4ef9b0.cpp
    engine/src/retail/script/native_4ef9d0.cpp
    engine/src/retail/script/native_4efa10.cpp
    engine/src/retail/script/native_4efa90.cpp
    engine/src/retail/script/hash_table_globals.cpp
    engine/src/retail/script/native_4f11a0.cpp
    engine/src/retail/script/native_4f1180.cpp
    engine/src/retail/mem/native_4ab7f5.cpp
    engine/src/retail/core/app_startup_globals.cpp
    engine/src/retail/core/app_startup_natives.cpp
    engine/src/retail/core/class_registration.cpp
    engine/src/retail/core/debug_command_line.cpp
    engine/src/retail/core/flow.cpp
    engine/src/retail/core/fpu_state.cpp
    engine/src/retail/core/frame_natives.cpp
    engine/src/retail/core/native_402820.cpp
    engine/src/retail/core/native_402870.cpp
    engine/src/retail/sys/native_490370.cpp
    engine/src/retail/core/native_4e6030.cpp
    engine/src/retail/render/native_495cc0.cpp
    engine/src/retail/core/game_flow.cpp
    engine/src/retail/core/native_0048f0e0.cpp
    engine/src/retail/core/native_0048f0f0.cpp
    engine/src/retail/core/native_0048f970.cpp
    engine/src/retail/core/qtask_create_task.cpp
    engine/src/retail/core/qtask_list.cpp
    engine/src/retail/core/retail_error.cpp
    engine/src/retail/core/main.cpp
    engine/src/retail/core/native_48f490.cpp
    engine/src/retail/core/native_4c1780.cpp
    engine/src/retail/core/native_489560.cpp
    engine/src/retail/core/native_401810.cpp
    engine/src/retail/core/native_4f1ee0.cpp
    engine/src/retail/core/native_507e70.cpp
    engine/src/retail/core/native_4f9200.cpp
    engine/src/retail/core/native_4f9210.cpp
    engine/src/retail/core/native_4f9220.cpp
    engine/src/retail/core/native_4f9230.cpp
    engine/src/retail/core/native_4f9240.cpp
    engine/src/retail/core/native_4f9250.cpp
    engine/src/retail/core/native_4f9260.cpp
    engine/src/retail/core/native_4f9280.cpp
    engine/src/retail/core/native_4663d0.cpp
    engine/src/retail/core/native_4f9cd0.cpp
    engine/src/retail/core/native_4f9eb0.cpp
    engine/src/retail/core/native_4fa4e0.cpp
    engine/src/retail/core/native_4fa520.cpp
    engine/src/retail/core/native_4fa560.cpp
    engine/src/retail/core/native_4fa5a0.cpp
    engine/src/retail/core/native_4fa5e0.cpp
    engine/src/retail/core/native_4fa620.cpp
    engine/src/retail/core/native_4fa660.cpp
    engine/src/retail/core/native_4fa6a0.cpp
    engine/src/retail/core/native_4fa6e0.cpp
    engine/src/retail/core/native_4fa730.cpp
    engine/src/retail/core/native_4fa7a0.cpp
    engine/src/retail/core/native_4fa7e0.cpp
    engine/src/retail/core/native_4fa810.cpp
    engine/src/retail/core/native_4fa890.cpp
    engine/src/retail/core/native_4fa8d0.cpp
    engine/src/retail/core/native_4fa910.cpp
    engine/src/retail/core/native_4fa950.cpp
    engine/src/retail/core/native_4fa9d0.cpp
    engine/src/retail/core/native_4faa10.cpp
    engine/src/retail/core/native_4f9e70.cpp
    engine/src/retail/core/native_48f4f0.cpp
    engine/src/retail/core/task_types.cpp
    engine/src/retail/core/native_401890.cpp
    engine/src/retail/diagnostics/log_record.cpp
    engine/src/retail/dynobj/dynamic_cube.cpp
    engine/src/retail/dynobj/hash_lookup.cpp
    engine/src/retail/dynobj/local_models_mef.cpp
    engine/src/retail/dynobj/mtp_package_loader.cpp
    engine/src/retail/error/error_show.cpp
    engine/src/retail/error/native_4af810.cpp
    engine/src/retail/error/native_4af870.cpp
    engine/src/retail/error/native_4af880.cpp
    engine/src/retail/human/human_camera_eye.cpp
    engine/src/retail/human/human_view.cpp
    engine/src/retail/input/input_port_process.cpp
    engine/src/retail/mem/task_list.cpp
    engine/src/retail/render/native_4f0f80.cpp
    engine/src/retail/render/native_4f1000.cpp
    engine/src/retail/script/native_4ee960.cpp
    engine/src/retail/script/native_4ee970.cpp
    engine/src/retail/script/native_4ee970_globals.cpp
    engine/src/retail/script/native_4f19c0.cpp
    engine/src/retail/script/native_4f1bc0.cpp
    engine/src/retail/script/native_4f1b70.cpp
    engine/src/retail/script/native_4f1b80.cpp
    engine/src/retail/script/native_4f1b60.cpp
    engine/src/retail/script/native_4f1b90.cpp
    engine/src/retail/script/native_4f1ba0.cpp
    engine/src/retail/script/native_4f1bb0.cpp
    engine/src/retail/script/native_4f1a40.cpp
    engine/src/retail/script/native_4f1a60.cpp
    engine/src/retail/script/native_4f1a70.cpp
    engine/src/retail/script/native_4af910.cpp
    engine/src/retail/script/native_4b8810.cpp
    engine/src/retail/script/native_4b8890.cpp
    engine/src/retail/script/native_4cea10.cpp
    engine/src/retail/script/native_4cea20.cpp
    engine/src/retail/script/native_4baac0.cpp
    engine/src/retail/script/native_4bacd0.cpp
    engine/src/retail/script/native_4c0360.cpp
    engine/src/retail/script/native_4c04b0.cpp
    engine/src/retail/script/native_4c0530.cpp
    engine/src/retail/script/native_4c0560.cpp
    engine/src/retail/script/native_4f1840.cpp
    engine/src/retail/script/native_4f18c0.cpp
    engine/src/retail/script/native_4f1950.cpp
    engine/src/retail/script/native_4f1980.cpp
    engine/src/retail/script/native_4f1a80.cpp
    engine/src/retail/script/native_4f1b20.cpp
    engine/src/retail/script/native_4f1b50.cpp
    engine/src/retail/script/native_4f9460.cpp
    engine/src/retail/mem/mem_alloc.cpp
    engine/src/retail/mem/native_4b0d50.cpp
    engine/src/retail/mem/native_4b0d60.cpp
    engine/src/retail/mem/native_4b0d90.cpp
    engine/src/retail/model/iff_chunk_tree.cpp
    engine/src/retail/model/iff_skeleton.cpp
    engine/src/retail/model/mef_bone_rig.cpp
    engine/src/retail/model/mef_model_loader.cpp
    engine/src/retail/model/mef_native_attachments.cpp
    engine/src/retail/model/mef_render_data.cpp
    engine/src/retail/model/mef_skinner.cpp
    engine/src/retail/model/model_texture_package.cpp
    engine/src/retail/render/backend.cpp
    engine/src/retail/render/camera_transform.cpp
    engine/src/retail/render/lod_settings.cpp
    engine/src/retail/render/trans_context.cpp
    engine/src/retail/render/view_block.cpp
    engine/src/retail/resource/device_dispatch.cpp
    engine/src/retail/resource/is_resource_loaded.cpp
    engine/src/retail/resource/resource_archive.cpp
    engine/src/retail/resource/resource_cache.cpp
    engine/src/retail/resource/resource_lifecycle.cpp
    engine/src/retail/resource/resource_unpack.cpp
    engine/src/retail/resource/sys_resource.cpp
    engine/src/retail/script/qvm_compiled_program.cpp
    engine/src/retail/script/qvm_loader.cpp
    engine/src/retail/script/qvm_native_loader.cpp
    engine/src/retail/script/qvm_object_calls.cpp
    engine/src/retail/script/qvm_stack_natives.cpp
    engine/src/retail/script/script_mode.cpp
    engine/src/retail/sys/ilff_navigation.cpp
    engine/src/retail/sys/resource_lookup.cpp
    # All terrain translation units are in the target. terrain_storage.cpp owns the shared retail
    # storage (0x005CA114, 0x00A7E068, 0x00BCAB08, 0x00BCAAE0, 0x00A7F878..0x00A7F88C); those
    # definitions were removed from terrain_draw.cpp so exactly one definition of each exists in
    # the link. terrain_loaders.cpp no longer carries the throwing Terrain_TraverseContourQuadtree
    # and Terrain_Draw placeholders: terrain_quadtree.cpp defines the traversal native and
    # terrain_draw.cpp defines Terrain_Draw() with the retail no-argument ABI.
    # terrain_draw.cpp's Terrain_BuildMipMaps_2 (0x00514760) call resolves to terrain_mipmaps.cpp,
    # whose g_a7e070 (0x00A7E070) definition lives in terrain_draw.cpp; no unit is left out.
    engine/src/retail/terrain/terrain_draw.cpp
    engine/src/retail/terrain/terrain_loaders.cpp
    engine/src/retail/terrain/terrain_mipmaps.cpp
    engine/src/retail/terrain/terrain_out_of_terrain_renderch.cpp
    engine/src/retail/terrain/terrain_quadtree.cpp
    engine/src/retail/terrain/terrain_scene_state.cpp
    engine/src/retail/terrain/terrain_storage.cpp
    engine/src/retail/terrain/terrain_unknown_blendmode.cpp
    engine/src/retail/texture/loop_terrain_texture.cpp
    engine/src/retail/texture/model_texture_pixels.cpp
    engine/src/retail/texture/texture_loaders.cpp
    engine/platform/windows/player_controller.cpp
    engine/platform/windows/retail_animation.cpp
    engine/platform/windows/direct_input_mouse.cpp
    engine/src/retail/core/native_402ff0.cpp
    engine/src/retail/core/native_40fe30.cpp
    engine/src/retail/core/native_410360.cpp
    engine/src/retail/core/native_4108b0.cpp
    engine/src/retail/core/native_415070.cpp
    engine/src/retail/core/native_454ee0.cpp
    engine/src/retail/core/native_470410.cpp
    engine/src/retail/core/native_470610.cpp
    engine/src/retail/core/native_470fa0.cpp
    engine/src/retail/core/native_472670.cpp
    engine/src/retail/core/native_472d00.cpp
    engine/src/retail/core/native_4733f0.cpp
    engine/src/retail/core/native_4739d0.cpp
    engine/src/retail/core/native_473ed0.cpp
    engine/src/retail/core/native_475380.cpp
    engine/src/retail/core/native_475ac0.cpp
    engine/src/retail/core/native_476730.cpp
    engine/src/retail/core/native_477220.cpp
    engine/src/retail/core/native_47cf70.cpp
    engine/src/retail/core/native_4801b0.cpp
    engine/src/retail/core/native_4813e0.cpp
    engine/src/retail/core/native_485330.cpp
    engine/src/retail/core/native_486900.cpp
    engine/src/retail/core/native_4872c0.cpp
    engine/src/retail/core/native_487be0.cpp
    engine/src/retail/core/native_488620.cpp
    engine/src/retail/core/native_488800.cpp
    engine/src/retail/core/native_488bb0.cpp
    engine/src/retail/qfile/native_4e68d0.cpp
    engine/src/retail/render/native_491980.cpp
    engine/src/retail/render/native_491cf0.cpp
    engine/src/retail/render/native_491ea0.cpp
    engine/src/retail/script/native_4b88f0.cpp
    engine/src/retail/sys/native_4f1cb0.cpp
    # Hold-out. Two separate groups, both measured by the linker rather than guessed.
    #
    # GROUP 1 - two files, each blocked by one callee that has no implementation:
    #   core/native_412420.cpp calls 0x0045E060 (858 bytes, 7 blocks, 237 instructions)
    #   core/native_4f9df0.cpp is outside until its callee set settles
    # Released since this block was first written: core/native_464260.cpp and core/native_465a10.cpp
    #   (unblocked by sys/native_4d9610.cpp and sys/native_504db0.cpp), and core/native_467700.cpp,
    #   core/native_4696b0.cpp, core/native_465bc0.cpp.
    # GROUP 2 - 32 transcribed files held back together because adding them as a set surfaces 23
    # undefined symbols. Measured by temporarily adding all 32 and reading the linker's list:
    #   functions: AIGraph_InitChain_5183D0, Core_Unknown_45E060, Core_Unknown_467F30,
    #              Sys_GetGlobal16_4D9610, Sys_GetGlobal16_504DB0
    #   globals:   g_5c8e08 g_5c8e0c g_5c8e10 g_5c8e18 g_5c8e1c g_5c8e24 g_5c8e30
    #              g_a7b2e0 g_a7b440 g_a7b4e0 g_a7b4f0 g_a7b638 g_bcaa00
    #              g_c28c62 g_c28d62 g_c28e80 g_c28f80 g_c28f81
    #   The 32 are mostly the input subsystem (input/native_48f090 .. input/native_509f00) plus
    #   core/native_4123f0, core/native_413730 and the 0x00509xxx group. The globals are declared
    #   extern by those files but have no owner anywhere in the tree, which is the real blocker:
    #   each needs one owner, and the owner is the routine that writes it.
    # Measured, not assumed: the linker rejected each of these by name until its callee existed.
    engine/src/retail/core/native_46e370.cpp
    engine/src/retail/sys/native_4aff90.cpp
    engine/src/retail/sys/native_4affb0.cpp
    engine/src/retail/sys/native_4af9d0.cpp
    engine/src/retail/sys/native_4f1db0.cpp
    engine/src/retail/core/native_413df0.cpp
    engine/src/retail/core/native_413b50.cpp
    engine/src/retail/core/native_413b60.cpp
    engine/src/retail/core/native_413de0.cpp
    engine/src/retail/core/native_47cf20.cpp
    engine/src/retail/core/native_47cf10.cpp
    engine/src/retail/core/native_46db40.cpp
    engine/src/retail/core/native_46d420.cpp
    engine/src/retail/core/native_477e30.cpp
    engine/src/retail/core/native_46cb70.cpp
    engine/src/retail/core/native_46c0c0.cpp
    engine/src/retail/core/native_4682f0.cpp
    engine/src/retail/core/native_46f2b0.cpp
    engine/src/retail/script/native_4b7f40.cpp
    engine/src/retail/core/native_4696b0.cpp
    engine/src/retail/core/native_467db0.cpp
    engine/src/retail/core/native_467dd0.cpp
    engine/src/retail/core/native_467700.cpp
    engine/src/retail/core/native_465bc0.cpp
    engine/src/retail/sys/native_4d9610.cpp
    engine/src/retail/sys/native_504db0.cpp
    engine/src/retail/sys/native_4f1c70.cpp
    engine/src/retail/core/native_466e40.cpp
    engine/src/retail/core/native_464260.cpp
    engine/src/retail/core/native_465a10.cpp
    engine/src/retail/core/native_466f70.cpp
    engine/src/retail/core/native_406450.cpp
    engine/src/retail/core/native_4310f0.cpp
    engine/src/retail/core/native_42d2b0.cpp
    engine/src/retail/core/native_43ad80.cpp
    engine/src/retail/core/native_43b2f0.cpp
    engine/src/retail/core/native_43b850.cpp
    engine/src/retail/core/native_42ff00.cpp
    engine/src/retail/core/native_413db0.cpp
    engine/src/retail/core/native_413dc0.cpp
    engine/src/retail/core/native_413dd0.cpp
    engine/src/retail/core/native_42beb0.cpp
    engine/src/retail/core/native_407290.cpp
    engine/src/retail/core/native_43cc70.cpp
    engine/src/retail/core/native_43bfd0.cpp
    engine/src/retail/core/native_43c6d0.cpp
    engine/src/retail/core/native_43a840.cpp
    engine/src/retail/core/native_4397c0.cpp
    engine/src/retail/core/native_438df0.cpp
    engine/src/retail/core/native_408320.cpp
    engine/src/retail/core/native_408330.cpp
    engine/src/retail/core/native_408340.cpp
    engine/src/retail/core/native_438390.cpp
    engine/src/retail/core/native_4388b0.cpp
    engine/src/retail/core/native_437510.cpp
    engine/src/retail/core/native_436b10.cpp
    engine/src/retail/core/native_4361d0.cpp
    engine/src/retail/core/native_436780.cpp
    engine/src/retail/core/native_4330f0.cpp
    engine/src/retail/core/native_4351e0.cpp
    engine/src/retail/core/native_435870.cpp
    engine/src/retail/core/native_435d90.cpp
    engine/src/retail/core/native_48f220.cpp
    engine/src/retail/core/native_48f330.cpp
    engine/platform/windows/retail_native_verification_support.cpp)
target_include_directories(igi PRIVATE engine/include)
target_compile_features(igi PRIVATE cxx_std_20)
set_target_properties(igi PROPERTIES OUTPUT_NAME "igi1")
if(MINGW)
    # The bottle is Windows 2000-era and ships no api-ms-win-crt-* redistributable,
    # so statically bind the C runtime rather than importing the UCRT DLLs.
    # The bottle is Windows 2000-era and ships no api-ms-win-crt-* or libstdc++-6
    # redistributable, so both the C and C++ runtimes must be fully static. The
    # trailing -Bstatic keeps the subsequent libraries static as well.
    target_link_options(igi PRIVATE -static -static-libgcc -static-libstdc++
                        -Wl,-Bstatic)
    # The retail renders through DirectDraw 7 (igi.exe imports ddraw.dll).
    target_link_libraries(igi PRIVATE ddraw dxguid dinput)
endif()

# Qvm_Execute (0x004B85B0) and Qvm_GetFloatArg (0x004B8A50) are transcribed, but their eight
# retail callees (0x004BD080, 0x004C0200, 0x004BF1C0, 0x004BFC70, 0x004BF1F0, 0x004BFA80,
# 0x004BF8F0, 0x004BEFC0, 0x004BD010, 0x004B9660) and three shared QVM state words are not
# ported yet. They are compiled as a separate object target so every build checks this source
# without introducing undefined symbols into the igi link; move the file into the igi source
# list once those dependencies exist. The object is compile evidence only, not a linked native.
add_library(igi_qvm_execute_object OBJECT engine/src/retail/script/qvm_execute.cpp)
target_include_directories(igi_qvm_execute_object PRIVATE engine/include)
target_compile_features(igi_qvm_execute_object PRIVATE cxx_std_20)
add_dependencies(igi igi_qvm_execute_object)

# The three human player/config natives in humanplayer_defines.cpp are transcribed (0x00410070
# DefineHumanPlayerGeneral, 0x004137A0 HumanPlayer_LoadParameters, 0x00405850 Config_Read), but
# they cannot join the igi link yet. HumanPlayer_LoadParameters and Config_Read call
# Qvm_Load/Qvm_Execute/Qvm_Cleanup/Qvm_GetFloatArg: Qvm_Load and Qvm_Cleanup are already in the
# igi link (qvm_native_loader.cpp), while Qvm_Execute/Qvm_GetFloatArg live in the
# igi_qvm_execute_object unit above, which cannot link until its own ten retail callees and the
# three QVM state words exist. Config_Read's remaining four callees are separate retail natives
# that are not transcribed on this branch: 0x00405300 Config_SaveGraphicOptions,
# 0x004058A0 Config_SaveSoundOptions, 0x00405980 Config_NoD3DDriversWithHard and
# 0x00405B30 Config_Language_2 (13-function closure, see
# docs/fidelity/evidence/humanplayer-defines-link-blockers.json). The unit is therefore compiled
# by every build as an object target so its source is checked without adding undefined symbols to
# the igi link; move it into the igi source list once those dependencies exist. The object is
# compile evidence only, not a linked native.
add_library(igi_humanplayer_defines_object OBJECT engine/src/retail/script/humanplayer_defines.cpp)
target_include_directories(igi_humanplayer_defines_object PRIVATE engine/include)
target_compile_features(igi_humanplayer_defines_object PRIVATE cxx_std_20)
add_dependencies(igi igi_humanplayer_defines_object)

include(cmake/NativeVerification.cmake)

# Testing only: candidate uses IGI1MUTEX instead of retail IGIMUTEX so retail
# igi.exe and igi1.exe can run side by side. Must stay OFF for production.
option(IGI_TESTING_MUTEX "Use IGI1MUTEX so retail and candidate can run together" OFF)
if(IGI_TESTING_MUTEX)
  target_compile_definitions(igi PRIVATE IGI_TESTING_MUTEX)
endif()
