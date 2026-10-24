#pragma once

// The retail task lists: a counted array of task pointers.
//
// The retail keeps a QTaskList as an eight-byte header (count at +0x00,
// capacity at +0x04) followed by `capacity` pointer-sized task slots starting
// at +0x08. The list functions reconstructed here are the ones the retail's
// flow/task registry uses to hold its live task objects; they are independent
// of the QTask type/event registry in task_types.hpp.
//
// Reconstructed natively from igi.exe (SHA-256
// dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01):
//   0x004C1790 QTaskList_Register
//   0x004C17C0 QTaskList_Remove
//   0x004C1800 QTaskList_New
//
// Not yet reconstructed (kept out of this header deliberately):
//   0x004C1830 QTaskList_Delete -> calls retail 0x004B0D10 Mem_DeAllocFn,
//             whose body tail-calls 0x004A6A50 (Camera_UpdateTransform_9, a
//             ten-block heap-free routine). That free chain has to be
//             reconstructed before Delete can be offered as a native.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Native: QTaskList_New; retail VA 0x004C1800.
// Signature: x86 cdecl `int32_t*(int32_t)`; ported as `void* QTaskList_New(int32_t capacity)`.
// Allocates `capacity * 4 + 8` bytes through Mem_AllocFn(0x004B0C60) with
// alignment 4, stores the capacity at +0x04 and zeroes the count at +0x00.
void* NATIVE_DECL QTaskList_New(std::int32_t capacity);

// Native: QTaskList_Register; retail VA 0x004C1790.
// Signature: x86 cdecl `int32_t*(int32_t*, int32_t)`; ported as
// `void* QTaskList_Register(void* task_list, void* task)`.
// Appends `task` when the count is below the capacity and returns the list;
// otherwise calls Debug_Error("QTaskList is full") and spins in the retail's
// `jmp $` self-loop.
void* NATIVE_DECL QTaskList_Register(void* task_list, void* task);

// Native: QTaskList_Remove; retail VA 0x004C17C0.
// Signature: x86 cdecl `int32_t(int32_t*, int32_t)`; ported as
// `int32_t QTaskList_Remove(void* task_list, void* task)`.
// Linear search by task pointer; on a hit the count is decremented and the
// removed slot is overwritten with the last live slot, then the removed index
// is returned. On a miss calls Debug_Error with the retail's message and
// spins in the retail's self-loop.
std::int32_t NATIVE_DECL QTaskList_Remove(void* task_list, void* task);

} // extern "C"
