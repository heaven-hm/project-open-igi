#pragma once

#include <cstddef>
#include <cstdint>

namespace igi::retail::mem {

using NewHandlerFn = std::int32_t (*)(std::uint32_t size);

// Port-owned storage for the retail engine heap globals. The working set matches the
// addresses documented in CLEAN (mem/004B0C60-Mem_Alloc.c and dependents). Field names embed the
// retail VA so the mapping stays auditable.
struct MemGlobals {
    std::int32_t alloc_count_942300 = 0;            // 0x00942300 (g_942300)
    std::uint32_t heap_init_9422fc = 0;             // 0x009422FC (g_9422fc)
    void* free_list_head_a960f0 = nullptr;          // 0x00A960F0 (g_a960f0)
    void* sentinel_a960f4 = nullptr;                // 0x00A960F4 (g_a960f4)
    void* free_list_back_a960f8 = nullptr;          // 0x00A960F8 (g_a960f8)
    std::int32_t state_a960e0 = 0;                  // 0x00A960E0 (g_a960e0)
    std::uint32_t qhash_flag_9422fd = 0;            // 0x009422FD (g_9422fd)
    std::uint32_t bucket_sentinel_936250 = 0;       // 0x00936250 (g_936250)
    NewHandlerFn new_handler_936254 = nullptr;      // 0x00936254 (g_936254)
    std::uint32_t heap_mode_c3242c = 0;             // 0x00C3242C (g_c3242c)
    void* heap_handle_c32428 = nullptr;             // 0x00C32428 (g_c32428)
    std::uint32_t pool_threshold_c321fc = 0;        // 0x00C321FC (g_c321fc)
    std::uint32_t free_list_bound_5466d4 = 0;       // 0x005466D4 (g_5466d4)
};

[[nodiscard]] MemGlobals& Mem_GetGlobals();
void Mem_ResetGlobalsForTest();

// Retail global symbol references backed by Mem_GetGlobals() so transcribed CLEAN bodies
// referencing g_a960f0, g_942300, g_9422fc, etc. resolve without undeclared identifier errors.
extern std::int32_t& g_942300;
extern std::uint32_t& g_9422fc;
extern void*& g_a960f0;
extern void*& g_a960f4;
extern void*& g_a960f8;
extern std::int32_t& g_a960e0;
extern std::uint32_t& g_9422fd;
extern std::uint32_t& g_936250;
extern NewHandlerFn& g_936254;
extern std::uint32_t& g_c3242c;
extern void*& g_c32428;
extern std::uint32_t& g_c321fc;
extern std::uint32_t& g_5466d4;

// Native: QHash_ValueSet() - VA 0x004B0D40 - complete port.
// Behavior: sets `g_9422fd` (`0x009422FD`) to `1`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x004B0D40-0x004B0D47, 8 bytes; CLEAN/qhash/004B0D40-QHash_ValueSet.c.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for single byte store.
// Signature: x86 cdecl int(void); ported as int QHash_ValueSet().
// Usage/tests: called by Mem_InitHeapState during heap initialization; exercised by mem_alloc_test.cpp.
int QHash_ValueSet();

// Native: Mem_InitHeapState() - VA 0x004B0BA0 - complete port.
// Behavior: initializes the heap free-list header triplet (`g_a960f0 = &g_a960f4`,
//           `g_a960f8 = &g_a960f0`, `g_a960f4 = 0`), sets `g_9422fc = 1`, calls
//           `QHash_ValueSet()`, and sets `g_a960e0 = 0x10`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B0BA0-0x004B0BD5, 54 bytes;
//                 CLEAN/sys/004B0BA0-Mem_InitHeapState.c.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact global stores and callee order.
// Signature: x86 cdecl int(void); ported as int Mem_InitHeapState().
// Usage/tests: called lazily on first Mem_Alloc when `g_9422fc == 0`; exercised by mem_alloc_test.cpp.
int Mem_InitHeapState();

// Host primitive backing the retail HeapAlloc(g_c32428, 0, dwBytes) tail of Mem_AllocateRawBlock.
void* Mem_HeapAlloc(std::uint32_t num_bytes);
void Mem_HeapFree(void* pointer);

// Native: Mem_InvokeNewHandler() - VA 0x004AD722 - complete port.
// Behavior: invokes the optional CRT new-handler callback at `g_936254` (`0x00936254`) with
//           `size`; returns `1` if the callback is non-null and returns non-zero, else `0`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004AD722-0x004AD73F, 30 bytes;
//                 CLEAN/camera/004AD722-Camera_ApplyHandiCam_28.c.
// CFG: radare2 3 blocks/3 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Confidence: high for exact new-handler check and return value.
// Signature: x86 cdecl int(uint32_t size); ported as int Mem_InvokeNewHandler(std::uint32_t size).
// Usage/tests: retry predicate for Mem_AllocWithRetry; exercised by mem_alloc_test.cpp.
int Mem_InvokeNewHandler(std::uint32_t size);
inline int Camera_ApplyHandiCam_28(std::uint32_t size) { return Mem_InvokeNewHandler(size); }

// Native: Mem_AllocateRawBlock() - VA 0x004AA6AE - complete port.
// Behavior: dispatches heap allocation by mode `g_c3242c` (`0x00C3242C`): tries small-block pool
//           in mode 3 (when `size <= g_c321fc`), tries bucket pool in mode 2
//           (when 16-byte rounded size `<= g_5466d4`), and otherwise rounds `max(size, 1)` up to a
//           16-byte multiple and calls `HeapAlloc`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004AA6AE-0x004AA724, 119 bytes;
//                 CLEAN/camera/004AA6AE-Camera_ApplyPostProcess_20.c.
// CFG: radare2 9 blocks/12 edges; Binary Ninja 9 blocks; 100% structural agreement.
// Confidence: high for mode branches, 16-byte rounding, and HeapAlloc fallback.
// Signature: x86 cdecl void*(uint32_t size); ported as void* Mem_AllocateRawBlock(std::uint32_t size).
// Usage/tests: called by Mem_AllocWithRetry; exercised by mem_alloc_test.cpp.
void* Mem_AllocateRawBlock(std::uint32_t size);
inline void* Camera_ApplyPostProcess_20(std::uint32_t size) { return Mem_AllocateRawBlock(size); }

// Native: Mem_AllocWithRetry() - VA 0x004AA682 - complete port.
// Behavior: rejects `size > 0xFFFFFFE0`; otherwise loops calling `Mem_AllocateRawBlock(size)`
//           until a non-null pointer (or `sentinel` match) is returned or `Mem_InvokeNewHandler(size)`
//           returns `0`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004AA682-0x004AA6AD, 44 bytes;
//                 CLEAN/camera/004AA682-Camera_ApplyWideScreen_20.c.
// CFG: radare2 5 blocks/6 edges; Binary Ninja 5 blocks; 100% structural agreement.
// Confidence: high for size guard `0xFFFFFFE0` and retry loop.
// Signature: x86 cdecl void*(uint32_t size, uint32_t sentinel); ported as void* Mem_AllocWithRetry(std::uint32_t size, std::uint32_t sentinel).
// Usage/tests: called by Mem_AllocChunk; exercised by mem_alloc_test.cpp.
void* Mem_AllocWithRetry(std::uint32_t size, std::uint32_t sentinel);
inline void* Camera_ApplyWideScreen_20(std::uint32_t size, std::uint32_t sentinel)
{
    return Mem_AllocWithRetry(size, sentinel);
}

// Native: Mem_AllocChunk() - VA 0x004AA670 - complete port.
// Behavior: forwards `(size, g_936250)` (`0x00936250`) to `Mem_AllocWithRetry`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004AA670-0x004AA681, 18 bytes;
//                 CLEAN/camera/004AA670-Camera_ApplyHandiCam_20.c.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for direct tail-call thunk.
// Signature: x86 cdecl void*(uint32_t size); ported as void* Mem_AllocChunk(std::uint32_t size).
// Usage/tests: called by Mem_Alloc; exercised by mem_alloc_test.cpp.
void* Mem_AllocChunk(std::uint32_t size);
inline void* Camera_ApplyHandiCam_20(std::uint32_t size) { return Mem_AllocChunk(size); }

// Native: Mem_Alloc() - VA 0x004B0C60 - complete port.
// Behavior: increments `g_942300` (`0x00942300`), allocates `context + arg2 + 0x1C` bytes via
//           `Camera_ApplyHandiCam_20` (0x004AA670), aligns the returned payload pointer to `arg2`,
//           stores the raw block header pointer at `(result - 4) & 0xFFFFFFFC`, lazily initializes
//           the heap list via `Mem_InitHeapState` (0x004B0BA0) when `g_9422fc == 0`, links the
//           block into `g_a960f0` (`0x00A960F0`) via `Camera_SetFieldOfView_33` (0x004AF8F0),
//           records `{result, total_bytes, user_bytes}` at header slots `2..4`, and on allocation
//           failure walks `g_a960f0` summing `i_1[3]` and `i_1[4]` before calling `Debug_Error` (0x004AF7B0).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B0C60-0x004B0D39, 218 bytes,
//                 digest 2b9694a3f11f74ab26f0de07cf380ec2a235e63578a53d919529ff80d22ce245;
//                 CLEAN/mem/004B0C60-Mem_Alloc.c.
// CFG: radare2 7 blocks/8 edges; Binary Ninja 7 blocks; 100% structural agreement.
// Confidence: high for header layout, alignment formula, lazy init, list insert, and OOM walk.
// Signature: x86 cdecl void*(int context, int arg2); ported as void* Mem_Alloc(std::int32_t context, std::int32_t arg2).
// Usage/tests: retail engine heap allocator used by DynObj_LocalModelsSMef, DynObj_LoadModelTexturePackage,
//              Terrain_TextureFileNotOfRequ, DynObj_LocalTexturesSTex, and Qvm_Load; exercised by mem_alloc_test.cpp.
void* Mem_Alloc(std::int32_t context, std::int32_t arg2);

} // namespace igi::retail::mem

