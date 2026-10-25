// Native: AIGraph_Handler_5093C0; retail VA 0x005093C0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x005093C0-0x0050943B (124 bytes, 7 blocks, 35 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x005093C0-0x0050943B; radare2 6.2.0 afij (124 bytes, 7 blocks, 35 instructions); image doubles at 0x00533998 (100.0) and 0x005334B0 (1/4096 = 0.000244140625); linked by the AIGraph registrar 0x00508FA0 at 0x00509216 (0x00401400 event 0x004F1A70()).
// Behavior/CFG: seven blocks, no loop. When the object is not found in the list that 0x004F1050 scans, it releases the first free index the pair 0x004F1240/0x004F1090 produces. It then clamps the two doubles at object+0x80 and object+0x88 with the retail rule `value >= 100.0` (fcom against the double at 0x00533998, fnstsw/test $0x41 - C0 or C2 set skips, so unordered also skips) by scaling them by the double at 0x005334B0, and finally calls 0x004F9180(object, arg2).
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 35-instruction listing; runtime UNOBSERVED. Link-closed: 0x004F1050 (native_4f1050.cpp), 0x004F1240 (native_4f1240.cpp), 0x004F1090 (native_4f1090.cpp) and 0x004F9180 (native_4f9180.cpp) are all transcribed. The scaling is exact because 1/4096 is a power of two, so the x87 extended-precision multiply and a C++ double multiply agree.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004F1050: scans the 0x00A71890..0x00A75890 table for the first entry equal to the argument and returns its index, or -1 (39 bytes,
// 6 blocks, 16 instructions). NOT transcribed yet - declared only.
std::int32_t NATIVE_DECL List_FindIndex_4F1050(void* object);

// 0x004F1240: returns the first index whose 0x004F1030 lookup is null (34 bytes, 3 blocks, 16 instructions; transcribed in
// native_4f1240.cpp). It takes NO argument: the 0x005093D3 `push %esi` is not its argument, it is 0x004F1090's second argument pushed
// early, and the shared `add $0x8,%esp` at 0x005093DF clears that word together with 0x004F1240's return value.
std::int32_t NATIVE_DECL List_FirstFree_4F1240(void);

// 0x004F1090: releases the 0x00A71890 table entry for an index, or raises through 0x004AF7B0 (68 bytes, 6 blocks, 22 instructions;
// transcribed in native_4f1090.cpp). Arguments: the index from 0x004F1240's return value, then the object word pushed at 0x005093D3.
void NATIVE_DECL List_Release_4F1090(std::int32_t index, void* object);

// 0x004F9180: the per-handler update that runs after the clamp (115 bytes, 7 blocks, 44 instructions; calls 0x004C48C0 and 0x00401CF0).
// NOT transcribed yet.
void NATIVE_DECL Graph_Update_4F9180(void* object, void* arg2);

void NATIVE_DECL AIGraph_Handler_5093C0(void* object, void* arg2)
{
    // 0x005093C6: call 0x4F1050; 0x005093CE: cmp $0xffffffff,%eax; 0x005093D1: jne 0x5093E2 - not found means "allocate a slot".
    if (List_FindIndex_4F1050(object) == -1)
    {
        // 0x005093D3/0x005093D4: push %esi; call 0x4F1240 - the object word is this call's second argument, already on the stack.
        List_Release_4F1090(List_FirstFree_4F1240(), object);                  // 0x005093D9/0x005093DA: push %eax; call 0x4F1090
    }

    // 0x005093E2..0x005093F3: fldl 0x80(%esi); fcoml 0x533998; fnstsw %ax; test $0x41,%ah; jne 0x509407.
    if (*reinterpret_cast<double*>(static_cast<std::uint8_t*>(object) + 0x80U) >= 100.0) // C0|C2 set (less-than or unordered) skips
    {
        // 0x005093F5..0x00509401: fldl 0x80(%esi); fmull 0x5334B0; fstpl 0x80(%esi).
        *reinterpret_cast<double*>(static_cast<std::uint8_t*>(object) + 0x80U) =
            *reinterpret_cast<double*>(static_cast<std::uint8_t*>(object) + 0x80U) * 0.000244140625;
    }

    // 0x00509407..0x00509418: the same comparison and scale for the second double.
    if (*reinterpret_cast<double*>(static_cast<std::uint8_t*>(object) + 0x88U) >= 100.0)
    {
        // 0x0050941A..0x00509426: fldl 0x88(%esi); fmull 0x5334B0; fstpl 0x88(%esi).
        *reinterpret_cast<double*>(static_cast<std::uint8_t*>(object) + 0x88U) =
            *reinterpret_cast<double*>(static_cast<std::uint8_t*>(object) + 0x88U) * 0.000244140625;
    }

    // 0x0050942C: mov 0xc(%esp),%eax; 0x00509430: push %eax; 0x00509431: push %esi; 0x00509432: call 0x4F9180.
    Graph_Update_4F9180(object, arg2);                                        // 0x00509437: add $0x8,%esp
}                                                                             // 0x0050943B: ret
} // extern "C"

#undef NATIVE_DECL
