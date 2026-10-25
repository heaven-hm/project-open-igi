// Native: Cursor_InitState_424C10; retail VA 0x00424C10 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00424C10-0x00424C3E (47 bytes, 1 block, 0 edges, 12 instructions).
// Tools/evidence: radare2 6.2.0 af/pd over the exact bytes; pushed as the phase-0 handler at 0x00424BA6 by the Cursor registrar 0x00424B90.
// Behavior/CFG: two stack arguments read at entry ([esp+4], [esp+8]) with no prologue and a plain ret; the phase-0 table slot is dispatched with two words by 0x004012A0 (site 0x004012DE: push context, push record, clean 8), not by the one-word site 0x00402481. Straight-line stores in retail order: *[arg1+0x20]=arg2, *[arg1+0x34]=0, *[arg1+0x24]=0x140, *[arg1+0x28]=0xF0, *[arg1+0x2C]=0xFFFFFFFE, *[arg1+0x30]=0, *[arg1+0x38]=0, *[arg1+0x3C]=0; no call, no branch.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. +0x24/+0x28 are the cursor position fields the reference candidate for 0x00424CE0 reads, so the two 0x140/0xF0 defaults are the initial 320x240 position; the meanings of +0x20 (the dispatch context word), +0x2C (0xFFFFFFFE) and +0x30..+0x3C are not established by any reader in the image and stay uninterpreted. EAX still holds arg1 at the ret (the first load is never overwritten), but the dispatch ignores the return, so the routine is declared void.
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

void NATIVE_DECL Cursor_InitState_424C10(void* cursor_record, std::uint32_t context)
{
    NATIVE_LOG_ENTER("Cursor_InitState_424C10", "0x00424C10");
    std::uint8_t* const record = static_cast<std::uint8_t*>(cursor_record);

    *reinterpret_cast<std::uint32_t*>(record + 0x20) = context;      // 0x00424C14/0x00424C18: mov ecx,[esp+8]; mov [eax+0x20],ecx
    *reinterpret_cast<std::uint32_t*>(record + 0x34) = 0U;           // 0x00424C1B/0x00424C1D: xor ecx,ecx; mov [eax+0x34],ecx
    *reinterpret_cast<std::uint32_t*>(record + 0x24) = 0x140U;       // 0x00424C20: movl $0x140,[eax+0x24]
    *reinterpret_cast<std::uint32_t*>(record + 0x28) = 0xF0U;        // 0x00424C27: movl $0xf0,[eax+0x28]
    *reinterpret_cast<std::uint32_t*>(record + 0x2C) = 0xFFFFFFFEU;  // 0x00424C2E: movl $0xfffffffe,[eax+0x2c]
    *reinterpret_cast<std::uint32_t*>(record + 0x30) = 0U;           // 0x00424C35: mov [eax+0x30],ecx
    *reinterpret_cast<std::uint32_t*>(record + 0x38) = 0U;           // 0x00424C38: mov [eax+0x38],ecx
    *reinterpret_cast<std::uint32_t*>(record + 0x3C) = 0U;           // 0x00424C3B: mov [eax+0x3c],ecx
}                                                                     // 0x00424C3E: ret

} // extern "C"

#undef NATIVE_DECL
