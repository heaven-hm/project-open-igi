// Native: List_Release_4F1090; retail VA 0x004F1090 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004F1090-0x004F10D3; radare2 6.2.0 pd/afij (68 bytes, 6 blocks, 22 instructions); the fatal path raises through 0x004AF7B0 with the string at 0x0054E0A0; called only from 0x005093DA.
// Behavior/CFG: six blocks. Reads 0x00A71890[index] with NO bounds check; when the entry is non-null and the object pointer stored at the entry is also non-null it reports "LevelQTaskID %d already defined" through 0x004AF7B0 and then spins forever (`jmp $` at 0x004F10B3); otherwise it releases the entry through 0x00401B20 and stores the new slot address from 0x00401AE0 back into the table.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 22-instruction listing; runtime UNOBSERVED. Link-closed: 0x004AF7B0 is the
//                already-transcribed Debug_Error (core/retail_error.cpp), 0x00401B20 is QHashSlotTable_Release (core/native_401b20.cpp) and
//                0x00401AE0 is transcribed in native_401ae0.cpp. The two-argument shape is retail's: the index arrives in the 0x005093D9 push and
//                the object in the 0x005093D3 push pushed before the 0x004F1240 call. The string literal is byte-identical to 0x0054E0A0.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

extern std::uint32_t g_a71890[0x1000];   // 0x00A71890, defined in engine/src/retail/script/native_4ee970_globals.cpp

// 0x004AF7B0: the fatal-error report/dispatch, transcribed as Debug_Error in engine/src/retail/core/retail_error.cpp (cdecl, (const char*, ...)).
int Debug_Error(char const* format, ...);

// 0x00401B20: the QHash slot-table release half, transcribed in engine/src/retail/core/native_401b20.cpp.
void QHashSlotTable_Release(void* slot_address);

// 0x00401AE0: the QHash slot-table insert half, transcribed in native_401ae0.cpp; returns the slot address.
void* QHashSlotTable_Insert_401AE0(void* object);

void NATIVE_DECL List_Release_4F1090(std::int32_t index, void* object)
{
    void* const entry = reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_a71890[index]));  // 0x004F1095: mov 0xa71890(,%esi,4),%eax
    if (entry != nullptr)                                                             // 0x004F109C/0x004F109E: test %eax,%eax; je 0x4f10be
    {
        if (*static_cast<std::uint32_t*>(entry) != 0U)                                // 0x004F10A0/0x004F10A3: cmp $0x0,(%eax); je 0x4f10b5
        {
            Debug_Error("LevelQTaskID %d already defined", index);                     // 0x004F10A5..0x004F10AB
            for (;;)                                                                  // 0x004F10B3: jmp 0x4f10b3 (jmp $)
            {
            }
        }
        QHashSlotTable_Release(entry);                                                // 0x004F10B5/0x004F10B6: push %eax; call 0x401b20
    }
    g_a71890[index] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        QHashSlotTable_Insert_401AE0(object)));                                       // 0x004F10BE..0x004F10CB: mov 0xc(%esp),%eax; push %eax; call 0x401ae0; mov %eax,0xa71890(,%esi,4)
}                                                                                     // 0x004F10D3: ret
} // extern "C"

#undef NATIVE_DECL
