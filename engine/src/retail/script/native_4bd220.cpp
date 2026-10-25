// Native: Qvm_LoadCompiledProgram_4BD220 (address-suffixed placeholder: the tree already uses the retail name Qvm_LoadCompiledProgram for the host-backed port in script/qvm_compiled_program.cpp, whose signature is std::span<const std::byte> and which three call sites use, so this retail transcription carries the VA suffix until that port is replaced; Binary Ninja label Sys_LoadBytecode_2). retail VA 0x004BD220-0x004BD373 (340 bytes, 20 blocks, 24 edges, 126 instructions in the reachable set).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a ret) over the exact image; the extent is bounded by the `ret` at 0x004BD373, and twelve alignment nops `90` x12 follow it (next entry 0x004BD380). Exact bytes 56 57 8b 7c 24 0c 8d 44 24 0c 50 57 e8 cf 8c ff ff 8b f0 ... 5f 8b c2 5e c3 (340 bytes, sha256 5e31c3277cb0fb6c62df46c97e00e3d754ff47113c68e5c4e94db974a0f52067 of the exact range). Every call displacement resolves as v+5+rel32: 0x004BD22C + 0xFFFF8CCF = 0x004B5F00 (Resource_Load); 0x004BD267 + 0xFFFF9114 = 0x004B6380 (Resource_Unload); 0x004BD2BF + 0xFFFF399C = 0x004B0C60 (Mem_Alloc). Call census from the image's 0xE8 enumeration: 0x004B80B0 (Qvm_Load) is the only caller. The header gate compares the loaded bytes against the four ASCII bytes 4C 4F 4F 50 ("LOOP") at +0, the dword 8 at +4 and the dword 5 at +8 with `jge`.
// Behavior/CFG: three cdecl arguments - (char const* resource_name, int32_t arg2, int32_t arg3) - returning the allocated context pointer. 0x004BD226 takes the address of its OWN FIRST ARGUMENT WORD as the out-size slot, so Resource_Load(name, &arg1) overwrites the name slot with the byte size and the gate at 0x004BD23A (`cmp dword [esp+0xC],0x3C` / `jb`) reads that size. On a null handle, a size below 0x3C, a bad magic, +4 != 8 or +8 < 5 it calls Resource_Unload(name) and returns null (the size/magic failures unload, the null handle returns immediately). Otherwise it derives two counts: arg2, else the header dword at +0x34, else 0x800; and arg3, else the header dword at +0x38, else 0x100. It rounds the first count up to dwords (`add ebp,3` / `shr ebp,2`), computes the byte size `((first + header[0x14]*2 + second) * 4) + 0x46C`, calls Mem_Alloc(size, 4) (the first argument is the LAST push: `push 4` then `push edi` = the size), zeroes the whole block with the exact `rep stosd`/`rep stosb` pair (dwords, then `size & 3` bytes), and installs 21 pointer/count fields into it, all measured: +0x00 = resource + header[0x2C]; +0x04 = header[0x30]; +0x08 = first count; +0x0C = second count; +0x18 = first count; +0x1C = second count; +0x24 = context+0x46C; +0x28 = context+0x46C+first*4; +0x60 = that +second*4; +0x64 = that +header[0x14]*4; +0x68 = 0 (byte); +0x268 = 0 (byte); +0x468 = the loaded resource base. When header[0x14] is nonzero it also installs +0x58 = resource+header[0x0C], +0x5C = resource+header[0x10], +0x50 = header[0x14], +0x54 = header[0x18]; when header[0x24] is nonzero it installs +0x40 = resource+header[0x1C], +0x44 = resource+header[0x20], +0x38 = header[0x24], +0x3C = header[0x28]. This matches the tree's existing description of the same VA in qvm_compiled_program.cpp ("installs base + fields +0x0C/+0x10 when the gate at +0x14 is nonzero, and base + fields +0x1C/+0x20 when +0x24 is nonzero"), and the +0x24/+0x28 value/type stack pointers plus the +0x68 name slot line up with the QvmNativeStatePrefix layout in qvm_native_state.hpp and with 0x004BD080's reads.
// Status/limits: TRANSCRIBED; LINKABLE (its three callees 0x004B5F00, 0x004B6380 and 0x004B0C60 are all tree natives, so it adds no new dependency); STRUCTURALLY_VERIFIED against the 126-instruction reachable listing (20 blocks, 24 edges); runtime UNOBSERVED. THE NAME CARRIES A VA SUFFIX ON PURPOSE: the unsuffixed retail name is already taken by an incompatible host-backed port in the same folder, and renaming either one is the Lead's call, not this file's. The header layout beyond the measured offsets is not interpreted: every access is a literal byte offset with the retail instruction that produced it, and no structure type is invented for the loaded resource. Domain: script/, because the resource is the compiled QVM program and the only caller is Qvm_Load.

#include <cstdint>

#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void* NATIVE_DECL Qvm_LoadCompiledProgram_4BD220(char const* resource_name, std::int32_t arg2, std::int32_t arg3)
{
    // 0x004BD222 `mov edi,[esp+0xC]` - the name. 0x004BD226 `lea eax,[esp+0xC]` is the address of THIS BODY'S OWN
    // FIRST ARGUMENT WORD, so Resource_Load's out-size overwrites the name slot and 0x004BD23A reads it back as the size.
    std::int32_t size = 0;
    void* const resource = igi::retail::resource::Resource_Load(resource_name, &size); // 0x004BD22C
    // 0x004BD236 `test esi,esi` / 0x004BD238 `je 0x004BD26F` - a null handle returns null WITHOUT unloading.
    if (resource == nullptr)
    {
        return nullptr;                                                                 // 0x004BD26F-0x004BD273
    }
    // 0x004BD23A `cmp dword [esp+0xC],0x3C` / 0x004BD23F `jb` - the size gate.
    if (size < 0x3C)
    {
        (void)igi::retail::resource::Resource_Unload(resource_name);                     // 0x004BD267
        return nullptr;
    }
    std::uint8_t* const base = static_cast<std::uint8_t*>(resource);
    // 0x004BD241-0x004BD258: `cmp byte [esi],0x4C` ('L') / `mov cl,[esi+1]` / `mov al,0x4F` ('O') / `cmp cl,al` /
    // `cmp byte [esi+2],al` ('O') / `cmp byte [esi+3],0x50` ('P') - the four-byte magic, byte by byte.
    if (base[0] != 0x4C || base[1] != 0x4F || base[2] != 0x4F || base[3] != 0x50)
    {
        (void)igi::retail::resource::Resource_Unload(resource_name);                     // 0x004BD267
        return nullptr;
    }
    // 0x004BD25A `cmp dword [esi+4],8` / 0x004BD25E `jne` and 0x004BD260 `cmp dword [esi+8],5` / 0x004BD264 `jge`.
    if (*reinterpret_cast<std::int32_t*>(base + 0x04) != 8 ||
        *reinterpret_cast<std::int32_t*>(base + 0x08) < 5)
    {
        (void)igi::retail::resource::Resource_Unload(resource_name);                     // 0x004BD267
        return nullptr;
    }

    // 0x004BD276 `mov ebp,[esp+0x18]` = arg2; 0x004BD27E `mov ebp,[esi+0x34]`; 0x004BD294 `mov ebp,0x800`.
    std::uint32_t first_count = static_cast<std::uint32_t>(arg2);
    if (first_count == 0U)
    {
        first_count = *reinterpret_cast<std::uint32_t*>(base + 0x34);
    }
    if (first_count == 0U)
    {
        first_count = 0x800U;
    }
    // 0x004BD281 `mov eax,[esp+0x1C]` = arg3; 0x004BD289 `mov eax,[esi+0x38]`; 0x004BD29D `mov eax,0x100`.
    std::uint32_t second_count = static_cast<std::uint32_t>(arg3);
    if (second_count == 0U)
    {
        second_count = *reinterpret_cast<std::uint32_t*>(base + 0x38);
    }
    if (second_count == 0U)
    {
        second_count = 0x100U;
    }

    // 0x004BD2A6 `mov ecx,[esi+0x14]`; 0x004BD2A9 `add ebp,3` / 0x004BD2AC `shr ebp,2` - the first count rounded up
    // to dwords with a LOGICAL shift; 0x004BD2B1 `lea edx,[ebp+ecx*2]`; 0x004BD2B5 `add edx,eax`; 0x004BD2B7
    // `lea edi,[edx*4+0x46C]` - the byte size. 0x004BD2AF `push 4` / 0x004BD2BE `push edi` / 0x004BD2BF `call`.
    std::uint32_t const header_14 = *reinterpret_cast<std::uint32_t*>(base + 0x14);
    std::uint32_t const rounded_first = (first_count + 3U) >> 2;
    std::int32_t const block_size =
        static_cast<std::int32_t>(((rounded_first + header_14 * 2U + second_count) * 4U) + 0x46CU);
    std::uint8_t* const context = static_cast<std::uint8_t*>(
        igi::retail::mem::Mem_Alloc(block_size, 4));  // 0x004BD2BF: first argument is the last push (the size), then 4

    // 0x004BD2C4-0x004BD2DD: `mov ecx,edi` (the byte size) / `mov edx,eax` (the block) / `mov ebx,ecx` / `xor eax,eax` /
    // `mov edi,edx` / `shr ecx,2` / `rep stosd` / `mov ecx,ebx` / `and ecx,3` / `rep stosb` - zero the whole block.
    for (std::int32_t word = 0; word < block_size / 4; ++word)
    {
        reinterpret_cast<std::uint32_t*>(context)[word] = 0U;
    }
    for (std::int32_t byte = 0; byte < (block_size & 3); ++byte)
    {
        context[(block_size & ~3) + byte] = 0U;
    }

    // The field installs below are retail's order (0x004BD2DF-0x004BD326), each with its instruction.
    *reinterpret_cast<std::uint32_t*>(context + 0x00) =                     // 0x004BD2E4: `mov [edx],eax`
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base) + *reinterpret_cast<std::uint32_t*>(base + 0x2C));
    *reinterpret_cast<std::uint32_t*>(context + 0x04) =                     // 0x004BD2EF: `mov [edx+4],ecx`
        *reinterpret_cast<std::uint32_t*>(base + 0x30);
    *reinterpret_cast<std::uint32_t*>(context + 0x24) =                     // 0x004BD2F2: `mov [edx+0x24],eax`
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(context) + 0x46CU);
    *reinterpret_cast<std::uint32_t*>(context + 0x468) =                    // 0x004BD2F5: `mov [edx+0x468],esi`
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base));
    std::uint32_t const stack_cursor =                                      // 0x004BD2FB: `lea ecx,[eax+ebp*4]`
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(context) + 0x46CU) + rounded_first * 4U;
    *reinterpret_cast<std::uint32_t*>(context + 0x28) = stack_cursor;       // 0x004BD302: `mov [edx+0x28],ecx`
    context[0x268] = 0U;                                                    // 0x004BD305: `mov byte [edx+0x268],bl`
    std::uint32_t const after_second = stack_cursor + second_count * 4U;    // 0x004BD30B: `lea ecx,[ecx+eax*4]`
    context[0x68] = 0U;                                                     // 0x004BD30E: `mov byte [edx+0x68],bl`
    *reinterpret_cast<std::uint32_t*>(context + 0x60) = after_second;       // 0x004BD311: `mov [edx+0x60],ecx`
    *reinterpret_cast<std::uint32_t*>(context + 0x18) = rounded_first;      // 0x004BD317: `mov [edx+0x18],ebp`
    *reinterpret_cast<std::uint32_t*>(context + 0x1C) = second_count;       // 0x004BD31A: `mov [edx+0x1C],eax`
    *reinterpret_cast<std::uint32_t*>(context + 0x64) =                     // 0x004BD323: `mov [edx+0x64],ecx`
        after_second + header_14 * 4U;                                      // 0x004BD31D: `lea ecx,[ecx+edi*4]`
    *reinterpret_cast<std::uint32_t*>(context + 0x08) = rounded_first;      // 0x004BD320: `mov [edx+8],ebp`
    *reinterpret_cast<std::uint32_t*>(context + 0x0C) = second_count;       // 0x004BD326: `mov [edx+0xC],eax`

    // 0x004BD329 `cmp dword [esi+0x14],ebx` (ebx = 0) / 0x004BD32C `je 0x004BD34A`.
    if (header_14 != 0U)
    {
        *reinterpret_cast<std::uint32_t*>(context + 0x58) =                 // 0x004BD333: `mov [edx+0x58],eax`
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base) + *reinterpret_cast<std::uint32_t*>(base + 0x0C));
        *reinterpret_cast<std::uint32_t*>(context + 0x5C) =                 // 0x004BD33B: `mov [edx+0x5C],ecx`
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base) + *reinterpret_cast<std::uint32_t*>(base + 0x10));
        *reinterpret_cast<std::uint32_t*>(context + 0x50) =                 // 0x004BD341: `mov [edx+0x50],eax`
            *reinterpret_cast<std::uint32_t*>(base + 0x14);
        *reinterpret_cast<std::uint32_t*>(context + 0x54) =                 // 0x004BD347: `mov [edx+0x54],ecx`
            *reinterpret_cast<std::uint32_t*>(base + 0x18);
    }
    // 0x004BD34A `mov eax,[esi+0x24]` / 0x004BD34E `cmp eax,ebx` / 0x004BD351 `je 0x004BD36F`.
    if (*reinterpret_cast<std::uint32_t*>(base + 0x24) != 0U)
    {
        *reinterpret_cast<std::uint32_t*>(context + 0x40) =                 // 0x004BD358: `mov [edx+0x40],eax`
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base) + *reinterpret_cast<std::uint32_t*>(base + 0x1C));
        *reinterpret_cast<std::uint32_t*>(context + 0x44) =                 // 0x004BD360: `mov [edx+0x44],ecx`
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base) + *reinterpret_cast<std::uint32_t*>(base + 0x20));
        *reinterpret_cast<std::uint32_t*>(context + 0x38) =                 // 0x004BD366: `mov [edx+0x38],eax`
            *reinterpret_cast<std::uint32_t*>(base + 0x24);
        *reinterpret_cast<std::uint32_t*>(context + 0x3C) =                 // 0x004BD36C: `mov [edx+0x3C],ecx`
            *reinterpret_cast<std::uint32_t*>(base + 0x28);
    }
    // 0x004BD36F `pop edi` / 0x004BD370 `mov eax,edx` / 0x004BD372 `pop esi` / 0x004BD373 `ret` - the block is returned.
    return context;
}

} // extern "C"
