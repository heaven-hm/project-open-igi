// Native: Sys_QvmStandaloneRelease_4BD380 (address-suffixed placeholder; the catalog alias Sys_CheckHeap_28 is wrong - this body releases a QVM context, it checks no heap, and the same alias is already attached to 0x004BC960 in this tree's notes). retail VA 0x004BD380-0x004BD3C1 (66 bytes, 5 blocks, 7 edges, 27 instructions in the reachable set).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a ret) over the exact image; the extent is bounded by the `ret` at 0x004BD3C1, and fourteen alignment nops `90` x14 follow it (next entry 0x004BD3D0). Exact bytes 56 57 8b 7c 24 0c 33 f6 8b 47 50 85 c0 7e 1b 8b 47 64 8b 04 b0 85 c0 74 09 50 e8 01 d9 ff ff 83 c4 04 8b 47 50 46 3b f0 7c e5 8b 8f 68 04 00 00 51 e8 1a 90 ff ff 57 e8 54 39 ff ff 83 c4 08 5f 5e c3 = `push %esi` / `push %edi` / `mov edi,[esp+0xC]` / `xor esi,esi` / `mov eax,[edi+0x50]` / `test eax,eax` / `jle 0x004BD3AA` / loop `mov eax,[edi+0x64]` / `mov eax,[eax+esi*4]` / `test eax,eax` / `je` / `push eax` / `call 0x004BACA0` / `add esp,4` / `mov eax,[edi+0x50]` / `inc esi` / `cmp esi,eax` / `jl` / `mov ecx,[edi+0x468]` / `push ecx` / `call 0x004B63D0` / `push edi` / `call 0x004B0D10` / `add esp,8` / `pop edi` / `pop esi` / `ret`. Every call displacement resolves as v+5+rel32: 0x004BD39A + 0xFFFFD901 = 0x004BACA0; 0x004BD3B1 + 0xFFFF901A = 0x004B63D0; 0x004BD3B7 + 0x00003954 = 0x004B0D10. Call census from the image's 0xE8 enumeration: 0x004B83D0 (Qvm_Cleanup) is the only caller, on the branch where qvm[0x23] (context+0x8C) is nonzero.
// Behavior/CFG: one cdecl pointer argument (the QVM context), no frame. The handle count at context+0x50 is read and compared with a SIGNED `jle`, then a signed loop over `i` walks the handle map pointer at context+0x64: each non-null entry map[i] is released through 0x004BACA0 (this chain's node release), and the count at +0x50 is re-read on every iteration rather than hoisted (`mov eax,[edi+0x50]` appears both before the loop at 0x004BD388 and inside it at 0x004BD3A2). Then the dword at context+0x468 (the loaded resource base installed by 0x004BD220 at 0x004BD2F5) is flushed through 0x004B63D0, and the context itself is freed through 0x004B0D10. 0x004BD3BC `add esp,8` is the cdecl cleanup for the last two argument dwords.
// Status/limits: TRANSCRIBED; NOT LINKABLE - undefined in tree: Resource_Flush. The tree has NO declaration and NO definition of 0x004B63D0 anywhere under engine/ (measured: `grep -rn Resource_Flush engine/` returns nothing), and the manifest points its implementation at scripts/GameNatives/resource/004B63D0-Resource_Flush.cpp, outside the tree, so the NATIVE GAP declaration below is a real symbol reference rather than a cast and this file cannot link until that native lands. Its signature is derived from THIS call site only (one 32-bit argument, cdecl, return value unused here) and is therefore a call-site inference, not a retail-confirmed signature. Measured properties of the missing native for whoever transcribes it: 25 instructions, extent 0x004B63D0-0x004B6413, string 0x00547150 "Resource_Flush() : Couldn't locate resource at %p.", a fatal self-jump `jmp 0x004B63FB` on the not-found path, callees 0x004AF7B0 Debug_Error (tree) and 0x004B5C90 Resource_FreeNode (tree, resource/sys_resource.cpp), and it reads the global 0x00943E38 which currently has no owner anywhere in the tree. Also: retail leaves EAX at whatever 0x004B0D10 returned and 0x004B83D0 returns that value, but the tree's Mem_DeAllocFn is declared void, so this port returns 0 and the propagated value is NOT reproduced - recorded as a gap, not papered over. The signature is std::int32_t(void*) to match the tree's existing boundary type SysQvmStandaloneReleaseFn (qvm_native_loader.hpp:20), which is how this native is currently reached. STRUCTURALLY_VERIFIED against the 27-instruction listing (5 blocks, 7 edges; radare2 and Binary Ninja independently report 5 blocks/7 edges for this VA, and both agree with this source exactly). BLOCK/EDGE CORRECTION: an earlier provenance line in this file said "3 blocks, 3 edges" - those were estimates, not measurements. The corrected figures above are measured by splitting the reachable listing at every branch target and after every branch, and the walk's last instruction is the `ret` at 0x004BD3C1, so the 27 instructions fill exactly this function's 66-byte extent; runtime UNOBSERVED. Domain: script/, because the context is the QVM context released by Qvm_Cleanup.

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

// NATIVE GAP: 0x004B63D0 has no implementation file in this tree. It is declared here so the call below is a real
// symbol reference rather than a cast, and THIS FILE CANNOT LINK UNTIL IT LANDS. One 32-bit argument, cdecl.
void NATIVE_DECL Resource_Flush(void* data);

// 0x004BACA0: unlinks the node, returns it to the free list and decrements the live count (script/native_4baca0.cpp).
void NATIVE_DECL Script_QHashNodeRelease_4BACA0(void* node);
// 0x004B0D10: deallocation entry (mem/native_4b0d10.cpp).
void Mem_DeAllocFn(void* const pointer);

std::int32_t NATIVE_DECL Sys_QvmStandaloneRelease_4BD380(void* standalone)
{
    NATIVE_LOG_ENTER("Sys_QvmStandaloneRelease_4BD380", "0x004BD380");
    std::uint8_t* const context = static_cast<std::uint8_t*>(standalone);
    // 0x004BD388 `mov eax,[edi+0x50]` / 0x004BD38B `test eax,eax` / 0x004BD38D `jle 0x004BD3AA` - SIGNED, and the
    // count is re-read inside the loop at 0x004BD3A2, so it is not hoisted here.
    if (static_cast<std::int32_t>(*reinterpret_cast<std::uint32_t*>(context + 0x50)) > 0)
    {
        for (std::int32_t index = 0; // 0x004BD386 `xor esi,esi`
             index < static_cast<std::int32_t>(*reinterpret_cast<std::uint32_t*>(context + 0x50)); // 0x004BD3A6
             ++index)                // 0x004BD3A5 `inc esi`
        {
            // 0x004BD38F `mov eax,[edi+0x64]` - the handle map base.
            std::uint8_t* const handle_map = reinterpret_cast<std::uint8_t*>(
                static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(context + 0x64)));
            // 0x004BD392 `mov eax,[eax+esi*4]` - the handle map entry.
            void* const handle = reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                *reinterpret_cast<std::uint32_t*>(handle_map + index * 4U)));
            // 0x004BD395 `test eax,eax` / 0x004BD397 `je 0x004BD3A2` - a null entry is skipped.
            if (handle != nullptr)
            {
                Script_QHashNodeRelease_4BACA0(handle);   // 0x004BD399 `push eax` / 0x004BD39A `call 0x004BACA0`
            }
        }
    }
    // 0x004BD3AA `mov ecx,[edi+0x468]` / 0x004BD3B0 `push ecx` / 0x004BD3B1 `call 0x004B63D0` - the loaded resource.
    Resource_Flush(reinterpret_cast<void*>(static_cast<std::uintptr_t>(
        *reinterpret_cast<std::uint32_t*>(context + 0x468))));
    // 0x004BD3B6 `push edi` / 0x004BD3B7 `call 0x004B0D10` / 0x004BD3BC `add esp,8`.
    Mem_DeAllocFn(standalone);
    // 0x004BD3BF `pop edi` / 0x004BD3C0 `pop esi` / 0x004BD3C1 `ret`. Retail's EAX is 0x004B0D10's result, which the
    // tree's void Mem_DeAllocFn cannot express, so this port returns 0 and the propagated value is a recorded gap.
    return 0;
}

} // extern "C"
