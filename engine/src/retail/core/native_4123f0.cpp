// Native: Task_LookupRecord_4123F0 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004123F0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004123F0-0x00412412 (35 bytes, 4 blocks, 4 edges, 12 instructions: 8b4c2408 / 8b01 / 85c0 / 7c11 / 83f828 / 7d0c / c1e004 / 0520875300 / 894104 / c3 / c7410400000000 / c3; the next byte 0x00412413 is nop padding).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the raw bytes decode as `mov ecx,[esp+8]` (the second argument), `mov eax,[ecx]` (a signed index), `test eax,eax; jl 0x0041240b`, `cmp eax,0x28; jge 0x0041240b`, `shl eax,4; add eax,0x00538720`, `mov [ecx+4],eax`, `ret`, and the rejection path `mov dword [ecx+4],0; ret`. The addend `20 87 53 00` in the encoded instruction is the table base 0x00538720. The table is 0x28 records of 0x10 bytes at 0x00538720-0x0053899F; its image contents are 640 bytes with 164 non-zero, and the records I sampled each hold a pointer into the 0x00538Dxx string cluster, then two zero dwords and the float 1.0f (0x3F800000). 0x0040FE30 links this address as a HumanPlayer handler through 0x00401400 with the event id from Script_PhaseIdGetE4_4F1A60 (0x004F1A60).
// Behavior/CFG: reads the signed 32-bit index at the second argument, and when 0 <= index < 0x28 stores the address of record `index` of the 16-byte-stride table at 0x00538720 into the dword at argument+4, otherwise stores 0 there; the first argument is never read; four blocks, the two rejection edges and one join.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The second argument slot is real: the phase-0 dispatchers 0x004012CD and 0x004013DD both push two arguments, so handlers linked through 0x00401400 receive two. The table at 0x00538720 is not transcribed and has no owner anywhere in the tree, so this file uses the base as a raw immediate, exactly as core/native_40fe30.cpp uses the handler addresses, and the table's 640 bytes still need an owner before any other reader hardcodes the address; the record layout recorded above is what the image bytes show, not a declared C++ type, and the meaning of the index and of the records is not established by this body. Domain: core/, because it is a handler of the 0x00402930 startup registrar 0x0040FE30. Codegen note: GCC merges the two rejection paths into one unsigned compare - `mov ecx,[edx]; lea 0x53872(ecx),eax; shl eax,4; cmp ecx,0x28; mov ecx,0; cmovae ecx,eax; mov [edx+4],eax; ret` - so the compiled candidate is one block with no edges where retail has four blocks and four edges. The behaviour is identical, negative indices included: an unsigned compare against 0x28 rejects them exactly as retail's `jl` does, and the wrapped value that `lea`/`shl` produces for a negative index is discarded by the cmov.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void NATIVE_DECL Task_LookupRecord_4123F0(void*, std::int32_t* entry) // 0x004123F0: the first argument is never read
{
    const std::int32_t index = entry[0];    // 0x004123F4: mov eax,[ecx]
    if (index >= 0 && index < 0x28)         // 0x004123F8: jl 0x0041240b; 0x004123FD: jge 0x0041240b
    {
        const std::uint32_t record = 0x00538720U + static_cast<std::uint32_t>(index) * 0x10U; // 0x004123FF: shl $0x4; 0x00412402: add $0x538720
        entry[1] = static_cast<std::int32_t>(record); // 0x00412407: mov [ecx+4],eax
    }
    else
    {
        entry[1] = 0;                       // 0x0041240B: mov dword [ecx+4],0
    }
}

} // extern "C"

#undef NATIVE_DECL
