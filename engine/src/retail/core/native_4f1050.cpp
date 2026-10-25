// Native: List_FindIndex_4F1050; retail VA 0x004F1050 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F1050-0x004F1076 (39 bytes, 6 blocks, 16 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004F1050-0x004F1076; radare2 6.2.0 pd/afij (39 bytes, 6 blocks, 16 instructions); the table bounds 0x00A71890..0x00A75890 are the immediate at 0x004F106A and the base at 0x004F1057, matching the 0x1000-dword clear in 0x004EE982.
// Behavior/CFG: six blocks, one loop over at most 0x1000 dwords. Walks the table at 0x00A71890, skips null entries, compares the dword the entry points at with the argument and returns the entry index on a match; when the scan passes the 0x00A75890 bound it returns -1.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 16-instruction listing; runtime UNOBSERVED. Leaf: no call, and the only global
//                it touches (0x00A71890) is already defined by engine/src/retail/script/native_4ee970_globals.cpp, so this file has no missing
//                callee. 0x004F1090 is the matching release half and stays untranscribed.
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

std::int32_t NATIVE_DECL List_FindIndex_4F1050(void* object)
{
    std::int32_t index = 0;                                                   // 0x004F1055: xor %eax,%eax
    std::uint32_t const* slot = g_a71890;                                     // 0x004F1057: mov $0xa71890,%ecx
    do
    {
        void* const entry = *reinterpret_cast<void* const*>(slot);             // 0x004F105C: mov (%ecx),%edx
        // 0x004F105E/0x004F1060: test %edx,%edx; je 0x4f1066 - a null entry skips to the increment, it does not end the scan.
        if (entry != nullptr
            && *static_cast<void**>(entry) == object)                          // 0x004F1062: cmp %esi,(%edx); 0x004F1064: je 0x4f1075
        {
            return index;                                                     // 0x004F1075/0x004F1076: pop %esi; ret
        }
        ++slot;                                                               // 0x004F1066: add $0x4,%ecx
        ++index;                                                              // 0x004F1069: inc %eax
    } while (slot < &g_a71890[0x1000]);                                       // 0x004F106A: cmp $0xa75890,%ecx; 0x004F1070: jl 0x4f105c
    return -1;                                                                // 0x004F1072: or $0xffffffff,%eax
}
} // extern "C"

#undef NATIVE_DECL
