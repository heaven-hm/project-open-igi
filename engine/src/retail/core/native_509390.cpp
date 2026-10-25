// Native: AIGraph_ElementPointer_509390; retail VA 0x00509390 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x00509390-0x005093AB; radare2 6.2.0 pd/afij (28 bytes, 1 block, 8 instructions); called only from 0x00509340 at 0x0050935F (`push %esi; push %ebx; push %edi; call 0x509390`).
// Behavior/CFG: one block, no branch. Returns the address of the 8-byte element at (row + side * column) inside the buffer word at object+0xB8, with the side word at object+0x58: `imul` of the side by the third argument, `add` of the second argument, then `lea (base,index*8)`.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 8-instruction listing; runtime UNOBSERVED. Leaf: no call, no global beyond the
//                object itself. Argument order is retail's: arg1 object, arg2 row, arg3 column.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void* NATIVE_DECL AIGraph_ElementPointer_509390(void* object, std::int32_t row, std::int32_t column)
{
    std::uint8_t* const fields = static_cast<std::uint8_t*>(object);
    std::uint32_t index = static_cast<std::uint32_t>(*reinterpret_cast<std::int32_t*>(fields + 0x58U))
                        * static_cast<std::uint32_t>(column);                       // 0x00509398/0x0050939B: mov 0x58(%eax),%ecx; imul 0xc(%esp),%ecx
    index += static_cast<std::uint32_t>(row);                                       // 0x005093A0: add %edx,%ecx
    void* const buffer = *reinterpret_cast<void**>(fields + 0xB8U);                  // 0x005093A2: mov 0xb8(%eax),%edx
    return static_cast<std::uint8_t*>(buffer) + index * 8U;                         // 0x005093A8: lea (%edx,%ecx,8),%eax
}                                                                                    // 0x005093AB: ret
} // extern "C"

#undef NATIVE_DECL
