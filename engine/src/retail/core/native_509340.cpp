// Native: AIGraph_PopulateBuffer_509340; retail VA 0x00509340 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x00509340-0x00509384; radare2 6.2.0 pd/afij (69 bytes, 7 blocks, 33 instructions); called only from 0x005092E0 at 0x00509329 right after the +0xB8 buffer allocation.
// Behavior/CFG: seven blocks, a row loop and a column loop. Walks row = 0..side-1 and, for each row, column = 0..side-1 (side = the signed word at object+0x58, re-read on every bound check), gets each element address through 0x00509390, and writes {index = -1, float = -1.0f} into its two dwords (0xFFFFFFFF and the immediate 0xBF800000).
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 33-instruction listing; runtime UNOBSERVED. Link-closed: its only callee 0x00509390
//                is transcribed in native_509390.cpp and is a leaf. The negative side word skips both loops (`test %eax,%eax; jle` at 0x0050934B and
//                0x0050935A), so the buffer stays untouched in that case.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00509390: the element address inside the +0xB8 buffer, transcribed in native_509390.cpp.
void* NATIVE_DECL AIGraph_ElementPointer_509390(void* object, std::int32_t row, std::int32_t column);

void NATIVE_DECL AIGraph_PopulateBuffer_509340(void* object)
{
    std::uint8_t* const fields = static_cast<std::uint8_t*>(object);

    if (*reinterpret_cast<std::int32_t*>(fields + 0x58U) > 0)                        // 0x00509348/0x0050934B/0x0050934D: test %eax,%eax; jle 0x509382
    {
        for (std::int32_t row = 0; row < *reinterpret_cast<std::int32_t*>(fields + 0x58U); ++row)   // 0x00509378..0x0050937E: mov 0x58(%edi),%eax; inc %ebx; cmp %ebx,%eax; jl
        {
            for (std::int32_t column = 0; column < *reinterpret_cast<std::int32_t*>(fields + 0x58U); ++column) // 0x00509358..0x00509376: mov 0x58(%edi),%eax; inc %esi; cmp %esi,%eax; jl
            {
                void* const element = AIGraph_ElementPointer_509390(object, row, column);          // 0x0050935C..0x0050935F: push %esi; push %ebx; push %edi; call
                *reinterpret_cast<std::uint32_t*>(element) = 0xFFFFFFFFU;                          // 0x00509367: movl $0xffffffff,(%eax)
                *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(element) + 0x04U)
                    = 0xBF800000U;                                                                 // 0x0050936D: mov %ebp,0x4(%eax), ebp = 0xbf800000
            }
        }
    }
}                                                                                    // 0x00509382/0x00509384: pop %edi; pop %ebx; ret
} // extern "C"

#undef NATIVE_DECL
