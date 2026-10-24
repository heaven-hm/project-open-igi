// Native: Qvm_CheckParameterCount (placeholder until VERIFIED); retail VA 0x004F1C20-0x004F1C68 (73 bytes, 6 blocks, 8 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (53 55 8b6c2410 56 57 8b7c2414 33f6 8b1f 85ed 7e19 ... ebfe); callers 0x004EF86F and 0x004EF9E8.
// Behavior/CFG: walks the requested number of parameters in a four-field context and faults when the script asked for more parameters than the caller supplied. Fields: [0] base index, [4] POINTER to the available count, [8] source file text, [0xC] line number. context[1] is passed straight to 0x4B8B00, which dereferences it.
//   for (i = 0; i < count; i += 1) { if (!(base + i < available)) { Debug_Error("Too few parameters in line %d of file '%s'", line, file); for (;;) {} } }
//   Note the argument order at 0x004F1C58/0x004F1C59: retail pushes the file pointer first and the line number second, so line is the vararg that binds %d and file binds %s.
// Status/limits: TRANSCRIBED (static only); depends on transcribed Qvm_IndexBelowCount (0x004B8B00) and Debug_Error (0x004AF7B0); the terminal spin is retail's `jmp $` and is preserved as an empty for(;;). Known residual: retail does `add esp,0xc` after Debug_Error and the candidate omits it because that path never returns, so nothing observes the stack; recorded rather than emulated. runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"

#include <cstdint>

extern "C" {

std::int32_t Qvm_IndexBelowCount(const std::int32_t* const limit, std::int32_t index);

void Qvm_CheckParameterCount(std::int32_t* const context, std::int32_t count)
{
    const std::int32_t base = context[0];                // 0x004F1C2E: mov ebx,[edi]
    for (std::int32_t index = 0; index < count; index += 1) // 0x004F1C30/0x004F1C49: test ebp,ebp; jle; cmp esi,ebp; jl
    {
        // context[1] is a POINTER to the available-count dword, not the count itself: 0x004F1C34 loads it
        // and passes it straight through to 0x4B8B00, which dereferences it (`mov esi,[edx]`). The reload is
        // inside the loop because retail reloads it there.
        const std::int32_t* const available = reinterpret_cast<const std::int32_t*>(
            static_cast<std::uintptr_t>(context[1]));          // 0x004F1C34: mov ecx,[edi+4]
        if (Qvm_IndexBelowCount(available, index + base) == 0) // 0x004F1C37..0x004F1C46: lea eax,[esi+ebx]; push eax; push ecx; call 0x4b8b00; test eax,eax; je
        {
            Debug_Error("Too few parameters in line %d of file '%s'", context[3], reinterpret_cast<const char*>(static_cast<std::uintptr_t>(context[2]))); // 0x004F1C52..0x004F1C64: edx=[edi+8], eax=[edi+0xc], push edx, push eax, push str.Too_few_parameters, call 0x4af7b0
            for (;;)                                     // 0x004F1C67: jmp 0x4F1C67
            {
            }
        }
    }
}

} // extern "C"
