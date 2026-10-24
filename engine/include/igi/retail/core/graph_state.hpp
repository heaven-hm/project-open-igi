#pragma once

#include <cstdint>

extern "C" {

// Graph/AI registration-scope leaf getters (no ai/ domain folder exists; graph
// state is owned by Graph_Init 0x4FAA60, so they live with the core natives).
// Native: Graph_Get_4F9200 (placeholder until VERIFIED); retail VA 0x004F9200.
// Returns the value at retail 0x00A76D10 (image bytes; writer Graph_Init 0x4FAA60).
std::uint32_t Graph_Get_4F9200();

// Native: Graph_Get_4F9210 (placeholder until VERIFIED); retail VA 0x004F9210.
// Returns the value at retail 0x0054E364 (image bytes; writer Graph_Init 0x4FAA60).
std::uint16_t Graph_Get_4F9210();

// Native: Graph_Get_4F9220 (placeholder until VERIFIED); retail VA 0x004F9220.
// Returns the value at retail 0x0054E36B (image bytes; writer Graph_Init 0x4FAA60).
std::uint8_t Graph_Get_4F9220();

// Native: Graph_Get_4F9230 (placeholder until VERIFIED); retail VA 0x004F9230.
// Returns the value at retail 0x0054E36C (image bytes; writer Graph_Init 0x4FAA60).
std::uint8_t Graph_Get_4F9230();

// Native: Graph_Get_4F9240 (placeholder until VERIFIED); retail VA 0x004F9240.
// Returns the value at retail 0x0054E36D (image bytes; writer Graph_Init 0x4FAA60).
std::uint8_t Graph_Get_4F9240();

// Native: Graph_Get_4F9250 (placeholder until VERIFIED); retail VA 0x004F9250.
// Returns the value at retail 0x0054E36E (image bytes; writer Graph_Init 0x4FAA60).
std::uint8_t Graph_Get_4F9250();

// Native: Graph_Get_4F9260 (placeholder until VERIFIED); retail VA 0x004F9260.
// Returns the value at retail 0x0054E36F (image bytes; writer Graph_Init 0x4FAA60).
std::uint8_t Graph_Get_4F9260();

// Native: Graph_Get_4F9280 (placeholder until VERIFIED); retail VA 0x004F9280.
// Returns the value at retail 0x0054E369 (image bytes; writer Graph_Init 0x4FAA60).
std::uint8_t Graph_Get_4F9280();

extern std::uint32_t g_a76d10;
extern std::uint16_t g_54e364;
extern std::uint8_t g_54e36b;
extern std::uint8_t g_54e36c;
extern std::uint8_t g_54e36d;
extern std::uint8_t g_54e36e;
extern std::uint8_t g_54e36f;
extern std::uint8_t g_54e369;

} // extern "C"
