// Native: Script_TaskContainer_Grow_4F1980 (placeholder until VERIFIED); retail VA 0x004F1980-0x004F19B3 (57 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 disassembly (8b4c2408 53 8b5c2408 56 57 8b03 03c1 50 e8bbffffff 8b0b 8b7308 8b7808 83c404 c1e104 8bd1 c1e902 f3a5 8bca 83e103 f3a4); callee 0x004F1950.
// Behavior/CFG: allocates a container sized *base + count (via 0x4F1950), then copies the OLD payload (*base * 16 bytes) from base[8] to the new container's payload pointer with rep movsd (dwords) followed by rep movsb (remaining 1-3 bytes), exactly retail's 4/1 split. Returns the new container pointer. Single block.
// Status/limits: TRANSCRIBED (static only); the copy length uses the OLD element count read after the allocation, and the new count comes from the allocator argument; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

void* Script_TaskContainer_Alloc_4F1950(std::int32_t count);

void* Script_TaskContainer_Grow_4F1980(std::int32_t* const container, std::int32_t const count)
{
    std::uint8_t* const fresh = static_cast<std::uint8_t*>(    // 0x004F1990: call 0x4F1950
        Script_TaskContainer_Alloc_4F1950(*container + count)); // 0x004F198B..0x004F198F: mov eax,[ebx]; add eax,ecx; push eax
    std::int32_t const old_count = *container;                 // 0x004F1995: mov ecx,[ebx]
    std::uint8_t* const source = *reinterpret_cast<std::uint8_t**>(reinterpret_cast<std::uint8_t*>(container) + 8);   // 0x004F1997
    std::uint8_t* const dest = *reinterpret_cast<std::uint8_t**>(fresh + 8);                                            // 0x004F199A
    std::int32_t dwords = old_count << 4;                      // 0x004F19A0: shl ecx,4
    std::int32_t const total = dwords;
    dwords >>= 2;                                              // 0x004F19A5: shr ecx,2
    for (std::int32_t i = 0; i < dwords; ++i)                  // 0x004F19A8: rep movsd
        *reinterpret_cast<std::int32_t*>(dest + i * 4) = *reinterpret_cast<std::int32_t const*>(source + i * 4);
    std::int32_t const tail = total & 3;                       // 0x004F19AC: and ecx,3
    for (std::int32_t i = 0; i < tail; ++i)                    // 0x004F19AF: rep movsb
        dest[dwords * 4 + i] = source[dwords * 4 + i];
    return fresh;                                              // eax retained through the copies
}

} // extern "C"
