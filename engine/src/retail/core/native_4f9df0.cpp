// Native: GraphStream_RegisterNode_4F9DF0; retail VA 0x004F9DF0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F9DF0-0x004F9E67 (120 bytes, 7 blocks, 40 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) is desynchronised at this VA by the adjacent jump table, so the body comes from radare2 6.2.0 pd/afij (120 bytes, 7 blocks, 40 instructions); the failure literal at 0x0054E3E8 is "GraphStream - collision in hash for %s"; caller in scope: the AIGraph registrar 0x00508FA0 at 0x0050928E ("RouteTable", 0xA, 0x005092A0).
// Behavior/CFG: seven blocks (hash, bucket walk, empty-free-list return, fill+insert tail, collision), no loop except the bucket walk. Hashes the name with 0x004F9E70, walks the bucket list the 0x00A774A0 header points at and reports a collision through 0x004AF810 when a node already carries the hash; otherwise takes the free node at 0x00A76D18, stores hash/+0xC/+0x10, unlinks it with 0x004AF960, clears its two link words and inserts it at the tail of the 0x00A774A0 list through 0x004AF8F0.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 40-instruction radare2 listing; runtime UNOBSERVED. 0x00A774A0 and 0x00A76D18 sit in .data's zero-fill tail (no file bytes), so they are defined here zeroed exactly as the loader leaves them; the non-null free-list node is established by the untranscribed graph-stream initialiser, and retail would fault on a null one because it dereferences the word before testing it.
#include <cstdint>

// 0x004AF8F0: the list-tail insert half, transcribed as Sys_InsertListTail_4AF8F0 in engine/src/retail/sys/native_4af8f0.cpp. It is
// declared inside the extern "C" block below because that transcription now exports C linkage; the namespace wrapper it used to carry
// was removed on 2026-10-25. 0x004AF960 is a different case and stays qualified: its transcription still exports C++ linkage.

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void* Sys_InsertListTail_4AF8F0(void* head, void* node);   // 0x004AF8F0, engine/src/retail/sys/native_4af8f0.cpp

std::uint32_t NATIVE_DECL GraphStream_HashName_4F9E70(char const* name);   // 0x004F9E70, engine/src/retail/core/native_4f9e70.cpp

// 0x004AF810: Script_WarningLog, transcribed in engine/src/retail/error/native_4af810.cpp (no header declares it yet).
void Script_WarningLog(char const* format, ...);



// 0x004AF960: the list unlink half, transcribed in engine/src/retail/sys/native_4af960.cpp; C linkage since 2026-10-25, so the
// declaration is unqualified here like the other native callees. The legacy HeapList_Remove copy is deleted.
void* Sys_UnlinkListNode_4AF960(void* node);

// 0x00A774A0: the graph-stream node list header. Only +0x00 (head), +0x04 (back-link field) and +0x08 (tail) are touched by the transcribed
// code - the full extent is not established. Zero-fill in the retail image.
void* g_a774a0[3] = {nullptr, nullptr, nullptr};

// 0x00A76D18: the graph-stream free-node pointer. Zero-fill in the retail image; written by the untranscribed stream initialiser.
void* g_a76d18 = nullptr;

void NATIVE_DECL GraphStream_RegisterNode_4F9DF0(char const* name, std::uint32_t value, std::uint32_t arg3)
{
    std::uint32_t const hash = GraphStream_HashName_4F9E70(name);             // 0x004F9DF6: push %esi; call 0x4f9e70; add $0x4,%esp

    // 0x004F9DFB: mov 0xa774a0,%edx; 0x004F9E04: mov (%edx),%ecx; 0x004F9E06: test %ecx,%ecx; 0x004F9E08: je 0x4f9e17.
    std::uint8_t* node = static_cast<std::uint8_t*>(g_a774a0[0]);
    while (node != nullptr)                                                   // 0x004F9E04..0x004F9E15
    {
        if (*reinterpret_cast<std::uint32_t*>(node + 0x08U) == hash)           // 0x004F9E0A: cmp %eax,0x8(%edx); 0x004F9E0D: je 0x4f9e58
        {
            // 0x004F9E58: push %esi (the name); 0x004F9E59: push $0x54e3e8; 0x004F9E5E: call 0x4af810.
            Script_WarningLog("GraphStream - collision in hash for %s", name);
            return;                                                           // 0x004F9E66: pop %esi; 0x004F9E67: ret
        }
        node = *reinterpret_cast<std::uint8_t**>(node);                        // 0x004F9E0F/0x004F9E11: mov %ecx,%edx; mov (%ecx),%ecx
    }

    // 0x004F9E17: mov 0xa76d18,%esi; 0x004F9E1D: cmpl $0x0,(%esi); 0x004F9E20: je 0x4f9e56 - the word is dereferenced before the test, so a
    // null free-list pointer faults exactly like retail.
    std::uint8_t* const fresh = static_cast<std::uint8_t*>(g_a76d18);
    if (*reinterpret_cast<void**>(fresh) == nullptr)                          // 0x004F9E1D/0x004F9E20
        return;                                                               // 0x004F9E56: pop %esi; 0x004F9E57: ret

    *reinterpret_cast<std::uint32_t*>(fresh + 0x08U) = hash;                  // 0x004F9E28: mov %eax,0x8(%esi)
    *reinterpret_cast<std::uint32_t*>(fresh + 0x0CU) = value;                 // 0x004F9E30: mov %eax,0xc(%ecx)  (eax = [esp+0xc] = arg2)
    *reinterpret_cast<std::uint32_t*>(fresh + 0x10U) = arg3;                  // 0x004F9E33: mov %edx,0x10(%ecx)  (edx = [esp+0x10] = arg3)
    Sys_UnlinkListNode_4AF960(static_cast<void*>(fresh)); // 0x004F9E36: push %esi; call 0x4af960
    *reinterpret_cast<void**>(fresh + 0x04U) = nullptr;                       // 0x004F9E41: movl $0x0,0x4(%esi)
    *reinterpret_cast<void**>(fresh) = nullptr;                               // 0x004F9E48: movl $0x0,(%esi)
    Sys_InsertListTail_4AF8F0(static_cast<void*>(g_a774a0), fresh); // 0x004F9E3B..0x004F9E4E: push %esi; push $0xa774a0; call 0x4af8f0
    // 0x004F9E53: add $0xc,%esp; 0x004F9E56/0x004F9E57: pop %esi; ret.
}
} // extern "C"

#undef NATIVE_DECL
