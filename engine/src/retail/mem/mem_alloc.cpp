#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/core/retail_error.hpp"

#include <cstdlib>

extern "C" void* Sys_InsertListTail_4AF8F0(void* head, void* node);   // 0x004AF8F0, engine/src/retail/sys/native_4af8f0.cpp

namespace igi::retail::mem {
namespace {

MemGlobals g_mem_globals{};

} // namespace

std::int32_t& g_942300 = g_mem_globals.alloc_count;
std::uint32_t& g_9422fc = g_mem_globals.heap_init;
void*& g_a960f0 = g_mem_globals.free_list_head;
void*& g_a960f4 = g_mem_globals.sentinel;
void*& g_a960f8 = g_mem_globals.free_list_back;
std::int32_t& g_a960e0 = g_mem_globals.state;
// 0x009422FD is the second byte of the retail dword at 0x009422FC, so this reference
// aliases heap_init instead of owning separate storage.
std::uint8_t& g_9422fd = reinterpret_cast<std::uint8_t*>(&g_mem_globals.heap_init)[1];
std::uint8_t (&g_9422e8)[16] = g_mem_globals.qhash_stack;
std::uint32_t& g_936250 = g_mem_globals.bucket_sentinel;
NewHandlerFn& g_936254 = g_mem_globals.new_handler;
std::uint32_t& g_c3242c = g_mem_globals.heap_mode;
void*& g_c32428 = g_mem_globals.heap_handle;
std::int32_t& g_c321e8 = g_mem_globals.pool_segment;
std::uint32_t& g_c321e4 = g_mem_globals.pool_slot_count;
void*& g_c321ec = g_mem_globals.pool_block;
std::uint32_t& g_c321f0 = g_mem_globals.pool_used;
std::uint32_t& g_c321f4 = g_mem_globals.pool_next;
void*& g_c321f8 = g_mem_globals.pool_base;
std::uint32_t& g_c321fc = g_mem_globals.pool_threshold;
std::uint32_t& g_5466d4 = g_mem_globals.free_list_bound;

MemGlobals& Mem_GetGlobals()
{
    return g_mem_globals;
}

void Mem_ResetGlobalsForTest()
{
    g_mem_globals = MemGlobals{};
}

// Host primitive backing the retail HeapAlloc(g_c32428, 0, dwBytes) tail of Camera_ApplyPostProcess_20.
void* Mem_HeapAlloc(std::uint32_t num_bytes)
{
    return std::malloc(static_cast<std::size_t>(num_bytes));
}

void Mem_HeapFree(void* pointer)
{
    std::free(pointer);
}

// Native: QHash_ValueSet; retail VA 0x004B0D40; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x004B0D40-0x004B0D47, 8 bytes; CLEAN/qhash/004B0D40-QHash_ValueSet.c.
// Behavior/CFG: sets `g_9422fd` (`0x009422FD`) to `1`. radare2 1 block/0 edges; Binary Ninja 1 block; 100.0% structural agreement.
// Status/limits: VERIFIED; high for single byte store.
int QHash_ValueSet()
{
    g_9422fd = 1;
    return 0;
}

// Native: Mem_InitHeapState; retail VA 0x004B0BA0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B0BA0-0x004B0BD5, 54 bytes; CLEAN/sys/004B0BA0-Mem_InitHeapState.c.
// Behavior/CFG: initializes the heap free-list header triplet (`g_a960f0 = &g_a960f4`, `g_a960f8 = &g_a960f0`, `g_a960f4 = 0`), sets `g_9422fc = 1`, calls `QHash_ValueSet()`, and sets `g_a960e0 = 0x10`. radare2 1 block/0 edges; Binary Ninja 1 block; 100.0% structural agreement.
// Status/limits: VERIFIED; high for exact global stores and callee order.
int Mem_InitHeapState()
{
    g_a960f0 = &g_a960f4;
    g_a960f8 = &g_a960f0;
    g_a960f4 = nullptr;
    g_9422fc = 1;
    QHash_ValueSet();
    g_a960e0 = 0x10;
    return 0;
}

// Native: Mem_InvokeNewHandler; retail VA 0x004AD722; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004AD722-0x004AD73F, 30 bytes; CLEAN/camera/004AD722-Camera_ApplyHandiCam_28.c.
// Behavior/CFG: invokes the optional CRT new-handler callback at `g_936254` (`0x00936254`) with `size`; returns `1` if the callback is non-null and returns non-zero, else `0`. radare2 4 blocks/4 edges; Binary Ninja 4 blocks; 100.0% structural agreement.
// Status/limits: VERIFIED; high for exact new-handler check and return value.
int Mem_InvokeNewHandler(std::uint32_t size)
{
    NewHandlerFn hook = g_936254;
    if (hook != nullptr && hook(size) != 0)
    {
        return 1;
    }
    return 0;
}

namespace {
void* Camera_ApplyPostProcess_21(std::uint32_t) { return nullptr; }
void* Camera_ApplyWideScreen_22(std::uint32_t) { return nullptr; }
} // namespace

// Native: Mem_AllocateRawBlock; retail VA 0x004AA6AE; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004AA6AE-0x004AA724, 119 bytes; CLEAN/camera/004AA6AE-Camera_ApplyPostProcess_20.c.
// Behavior/CFG: dispatches heap allocation by mode `g_c3242c` (`0x00C3242C`): tries small-block pool in mode 3 (when `size <= g_c321fc`), tries bucket pool in mode 2 (when 16-byte rounded size `<= g_5466d4`), and otherwise rounds `max(size, 1)` up to a 16-byte multiple and calls `HeapAlloc`. radare2 16 blocks/22 edges; Binary Ninja 16 blocks; 100.0% structural agreement.
// Status/limits: VERIFIED; high for mode branches, 16-byte rounding, and HeapAlloc fallback.
void* Mem_AllocateRawBlock(std::uint32_t size)
{
    const std::uint32_t data_5 = g_c3242c;
    std::uint32_t source_val = size;

    if (data_5 == 3)
    {
        if (source_val <= g_c321fc)
        {
            void* data_val = Camera_ApplyPostProcess_21(source_val);
            if (data_val != nullptr)
            {
                return data_val;
            }
        }
    }

    std::uint32_t dw_bytes;

    if (data_5 != 2)
    {
        if (source_val == 0)
        {
            source_val = 1;
        }

        dw_bytes = (source_val + 0xfU) & 0xfffffff0U;
    }
    else
    {
        dw_bytes = size != 0 ? (size + 0xfU) & 0xfffffff0U : 0x10U;

        if (dw_bytes <= g_5466d4)
        {
            void* data_4 = Camera_ApplyWideScreen_22(dw_bytes >> 4);
            if (data_4 != nullptr)
            {
                return data_4;
            }
        }
    }

    return Mem_HeapAlloc(dw_bytes);
}

// Native: Mem_AllocWithRetry; retail VA 0x004AA682; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004AA682-0x004AA6AD, 44 bytes; CLEAN/camera/004AA682-Camera_ApplyWideScreen_20.c.
// Behavior/CFG: rejects `size > 0xFFFFFFE0`; otherwise loops calling `Mem_AllocateRawBlock(size)` until a non-null pointer (or `sentinel` match) is returned or `Mem_InvokeNewHandler(size)` returns `0`. radare2 6 blocks/9 edges; Binary Ninja 6 blocks; 100.0% structural agreement.
// Status/limits: VERIFIED; high for size guard `0xFFFFFFE0` and retry loop.
void* Mem_AllocWithRetry(std::uint32_t size, std::uint32_t sentinel)
{
    if (size <= 0xffffffe0U)
    {
        void* result;
        do
        {
            result = Mem_AllocateRawBlock(size);

            if (result != nullptr ||
                sentinel == static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(result)))
            {
                return result;
            }
        } while (Mem_InvokeNewHandler(size) != 0);
    }

    return nullptr;
}

// Native: Mem_AllocChunk; retail VA 0x004AA670; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004AA670-0x004AA681, 18 bytes; CLEAN/camera/004AA670-Camera_ApplyHandiCam_20.c.
// Behavior/CFG: forwards `(size, g_936250)` (`0x00936250`) to `Mem_AllocWithRetry`. radare2 1 block/0 edges; Binary Ninja 1 block; 100.0% structural agreement.
// Status/limits: VERIFIED; high for direct tail-call thunk.
void* Mem_AllocChunk(std::uint32_t size)
{
    return Mem_AllocWithRetry(size, g_936250);
}

// Native: Mem_Alloc; retail VA 0x004B0C60; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: records `{result, total_bytes, user_bytes}` at header slots `2..4`, and on allocation Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B0C60-0x004B0D39, 218 bytes, digest 2b9694a3f11f74ab26f0de07cf380ec2a235e63578a53d919529ff80d22ce245; CLEAN/mem/004B0C60-Mem_Alloc.c.
// Behavior/CFG: increments `g_942300` (`0x00942300`), allocates `context + arg2 + 0x1C` bytes via `Camera_ApplyHandiCam_20` (0x004AA670), aligns the returned payload pointer to `arg2`, stores the raw block header pointer at `(result - 4) & 0xFFFFFFFC`, lazily initializes the heap list via `Mem_InitHeapState` (0x004B0BA0) when `g_9422fc == 0`, links the block into `g_a960f0` (`0x00A960F0`) via `Camera_SetFieldOfView_33` (0x004AF8F0), failure walks `g_a960f0` summing `i_1[3]` and `i_1[4]` before calling `Debug_Error` (0x004AF7B0). radare2 8 blocks/11 edges; Binary Ninja 8 blocks; 100.0% structural agreement. Terrain_TextureFileNotOfRequ, DynObj_LocalTexturesSTex, and Qvm_Load; exercised by mem_alloc_test.cpp.
// Status/limits: VERIFIED; high for header layout, alignment formula, lazy init, list insert, and OOM walk.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
void* Mem_Alloc(std::int32_t context, std::int32_t arg2)
{
    g_942300 += 1;

    if (sizeof(void*) != 4 && context < 0)
    {
        return nullptr;
    }

    // Retail uses the caller's alignment verbatim and wraps the LEA sum to 32 bits.
    const std::int32_t safe_arg2 = sizeof(void*) == 4 ? arg2 : (arg2 > 0 ? arg2 : 4);
    const std::int32_t total_size = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(context) + static_cast<std::uint32_t>(safe_arg2) + 0x1cU);
    // On 32-bit retail ABI the 5 header slots + 1 back-pointer slot occupy 6 * 4 = 24 (0x18) bytes
    // before the aligned payload (`+ 0x1b`). On a 64-bit host 6 pointer slots occupy 48 (0x30) bytes;
    // reserve the extra 32 bytes in the underlying host block while recording the exact retail
    // `context + safe_arg2 + 0x1c` value in `source_2[3]`.
    constexpr std::uint32_t kHeaderLead = sizeof(void*) == 4 ? 0x1bU : 0x33U;
    constexpr std::uint32_t kHostExtra = sizeof(void*) == 4 ? 0U : 0x20U;
    const std::uint32_t align_u = static_cast<std::uint32_t>(safe_arg2);
    const std::uint32_t alloc_arg = static_cast<std::uint32_t>(total_size) + kHostExtra;

    void** source_2 = static_cast<void**>(Mem_AllocChunk(alloc_arg));

    if (source_2 != nullptr && (sizeof(void*) == 4 || alloc_arg >= 5 * sizeof(void*)))
    {

        const std::uintptr_t raw_addr = reinterpret_cast<std::uintptr_t>(source_2);
        const std::uintptr_t mask = static_cast<std::uintptr_t>(align_u - 1U);
        const std::uintptr_t result_addr = (raw_addr + align_u + kHeaderLead) & ~mask;
        void* result = reinterpret_cast<void*>(result_addr);

        // Retail: *((result - 4) & 0xfffffffc) = source_2;
        const std::uintptr_t backptr_addr =
            (result_addr - sizeof(void*)) & ~(sizeof(void*) - 1U);
        *reinterpret_cast<void***>(backptr_addr) = source_2;

        source_2[1] = nullptr;
        *source_2 = nullptr;

        if (g_9422fc == 0)
        {
            Mem_InitHeapState();
        }

        Sys_InsertListTail_4AF8F0(&g_a960f0, source_2);

        source_2[2] = result;
        source_2[3] = reinterpret_cast<void*>(static_cast<std::intptr_t>(total_size));
        source_2[4] = reinterpret_cast<void*>(static_cast<std::intptr_t>(context));
        return result;
    }

    auto** i_1 = static_cast<void**>(g_a960f0);
    std::int32_t temp_val = 0;
    std::int32_t user_total = 0;
    void** i = i_1 != nullptr ? static_cast<void**>(*i_1) : nullptr;

    while (i != nullptr)
    {
        const auto dest_1 =
            static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(i_1[3]));
        const auto context_1 =
            static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(i_1[4]));
        i_1 = i;
        i = static_cast<void**>(*i);
        temp_val += dest_1;
        user_total += context_1;
    }

    Debug_Error("Mem_Alloc() failed to allocate %d bytes.\r\nTotal allocated by user: %d."
                      "\r\nTotal by Mem_Alloc():%d.",
                      total_size, user_total, temp_val);
    for (;;)
    {
    }
}

} // namespace igi::retail::mem

