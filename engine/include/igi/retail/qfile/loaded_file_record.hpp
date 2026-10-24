#pragma once

#include <cstdint>

extern "C" {

// A loaded-file record as 0x004B19C0 builds it and 0x004B1A90 walks it.
//   +0x00 next     : walk link read by 0x004B1A90; also node[0] of HeapList_Insert (0x004AF8F0)
//   +0x04 link_04  : node[1] of HeapList_Insert (0x004AF8F0)
//   +0x08 name     : pointer to the copied file name
//   +0x0C..+0x17   : cleared by the create routine; no reader/writer established
//   +0x18 address  : the word 0x004B1A90 compares its argument against
//   +0x1C..        : NUL-terminated copy of the file name (allocation is strlen + 0x1C bytes)
struct LoadedFileRecord {
    LoadedFileRecord* next;     // +0x00: walk link read by 0x004B1A90; node[0] slot of HeapList_Insert
    void* link_04;              // +0x04: node[1] slot of HeapList_Insert; no reader established
    char* name;                 // +0x08: pointer to the copied file name
    std::uint8_t opaque[0x0C];  // +0x0C..+0x17: cleared by 0x004B19C0; no reader/writer established
    std::uint32_t address;      // +0x18: the word 0x004B1A90 compares its argument against
};

// 0x00943970: head word of the loaded-file list. Defined by native_4b1a90.cpp.
extern LoadedFileRecord* g_943970;

} // extern "C"
