#pragma once

#include <cstdint>

extern "C" {

// Native: Mem_AllocIndexedBlock (placeholder name until VERIFIED; the
// catalog alias Sys_FreeMemory_2 is wrong: this allocates).
// Behavior: six-dword descriptor + index table allocation for a task type's
// class list; see engine/src/retail/mem/task_list.cpp for the retail bytes.
std::int32_t Mem_AllocIndexedBlock(std::int32_t* descriptor, std::int32_t element_size,
                                          std::int32_t count, std::int32_t alignment);

} // extern "C"
