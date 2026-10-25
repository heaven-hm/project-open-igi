// Native: QFile_ReleaseBlock_4B0B70; retail VA 0x004B0B70 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0B70-0x004B0B87 (24 bytes, 1 block, 0 edges, 9 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes (56 8b742408 8b06 50 e893010000 83c404 c70600000000 5e c3); 33 direct call sites in .text.
// Behavior/CFG: reads the block pointer stored at *argument, releases it through Mem_DeAllocFn (0x004B0D10), then stores null back into *argument. No branch; writes only through the caller's pointer.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Mem_DeAllocFn is transcribed in engine/src/retail/mem/native_4b0d10.cpp but has no shared header, so this file declares it the way the other landed callers do.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void Mem_DeAllocFn(void* const pointer);   // 0x004B0D10, engine/src/retail/mem/native_4b0d10.cpp

void NATIVE_DECL QFile_ReleaseBlock_4B0B70(void** block)
{
    NATIVE_LOG_ENTER("QFile_ReleaseBlock_4B0B70", "0x004B0B70");
    void* const pointer = *block;          // 0x004B0B75: mov eax,[esi]
    Mem_DeAllocFn(pointer);                // 0x004B0B78: push eax; call 0x004B0D10; add esp,4
    *block = nullptr;                      // 0x004B0B80: mov dword [esi],0
}

} // extern "C"

#undef NATIVE_DECL
