#pragma once
#include <windows.h>
#include <cstdint>

#define NATIVE_DECL __cdecl
extern "C" std::uint32_t NATIVE_DECL Native_00498480(std::uint32_t* capability,
                                                  std::uint32_t* platform);
#undef NATIVE_DECL
