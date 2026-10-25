// Native: Sys_InitFloatPair_4F1C70; retail VA 0x004F1C70 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F1C70-0x004F1C89 (26 bytes, 1 block, 0 edges).
// Tools/evidence: radare2 6.2.0 pd over the exact image; raw bytes read from the image with p8 26 @ 0x004F1C70 are c705e06ba70000000000 c705e46ba7000000803f b8e06ba700 c3. The four instructions are `mov dword [0x00A76BE0],0` (10 bytes, 0x004F1C70), `mov dword [0x00A76BE4],0x3F800000` (10 bytes, 0x004F1C7A), `mov eax,0x00A76BE0` (5 bytes, 0x004F1C84) and `ret` (1 byte, 0x004F1C89); 10+10+5+1 = 26, so the function ends at the ret and 0x004F1C8A is the first nop of the next routine.
// Behavior/CFG: stores the dword 0 at 0x00A76BE0 and the dword 0x3F800000 (1.0f as stored) at 0x00A76BE4, then returns the address 0x00A76BE0 in eax. No arguments, no branch, no callee, no other memory reference. This is the first of three structural siblings that initialise consecutive dword pairs in the same region: 0x004F1C90 (transcribed, sys/native_4f1c90.cpp) does the same two stores at 0x00A76BF0/0x00A76BF4 with 0 and 0x40000000 (2.0f) returning 0x00A76BF0, and 0x004F1CB0 (transcribed, sys/native_4f1cb0.cpp) does 0x00A76BE8/0x00A76BEC with 0 and 0x42C80000 (100.0f) returning 0x00A76BE8. The three differ only in the pair of globals and the second dword's bit pattern.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The two dwords are written as raw bit patterns because that is what retail stores; nothing in the routine establishes whether callers read the second dword as a float. The returned pointer is the address of the first dword. Storage ownership: 0x00A76BE0 is the first dword AFTER the 64-slot descriptor array g_a758e0[0x4C0] owned by 0x004F1840 (script/native_4f1bc0.cpp), which spans 0x00A758E0 to 0x00A76BE0 exclusive - 64 slots x 19 dwords - so this pair does not overlap it, exactly as the sibling pair at 0x00A76BE8 sits 8 bytes further on. Neither 0x00A76BE0 nor 0x00A76BE4 has any other owner or reference in the tree. Domain: sys/, because the two already transcribed siblings live there.
#include <cstdint>

#define NATIVE_DECL __attribute__((cdecl))

extern "C" {

std::uint32_t g_a76be0 = 0U; // retail 0x00A76BE0: first dword of the pair (image value 0)
std::uint32_t g_a76be4 = 0U; // retail 0x00A76BE4: second dword of the pair (image value 0)

void* NATIVE_DECL Sys_InitFloatPair_4F1C70(void)
{
    ::g_a76be0 = 0U;          // 0x004F1C70: mov dword [0x00A76BE0],0
    ::g_a76be4 = 0x3F800000U; // 0x004F1C7A: mov dword [0x00A76BE4],0x3F800000 (1.0f as stored)
    return &::g_a76be0;       // 0x004F1C84: mov eax,0x00A76BE0; 0x004F1C89: ret
}

} // extern "C"
