// Native: GraphStream_AddNode_4F9EB0; retail VA 0x004F9EB0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F9EB0-0x004F9FBA (267 bytes, 14 blocks, 117 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004F9EB0-0x004F9FBA; radare2 6.2.0 afij (267 bytes, 14 blocks, 117 instructions); called from 0x00509520 at 0x00509559 with five arguments.
// Behavior/CFG: fourteen blocks: hashes arg2 with 0x004F9E70, writes the hash through 0x004FA5E0 and the selector through 0x004FA4E0, then an eleven-entry dispatch through the table at 0x004F9FBC (verified in the image as 0x004F9EE5, 0x4F9EF8, 0x4F9F0B, 0x4F9F1F, 0x4F9F33, 0x4F9F46, 0x4F9F59, 0x4F9F6C, 0x4F9F83, 0x4F9F94, 0x4F9FA5) on arg3, each case writing one typed record built from arg4 (and arg5 for selector 0x0A); selectors above 0x0A jump to the shared 0x004F9FB8 tail, which returns the 1 left in AL by 0x004FA4E0.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 117-instruction listing and the 11-entry table read from the image; runtime UNOBSERVED. Link-closed: 0x004F9E70, 0x004FA4E0, 0x004FA520, 0x004FA560, 0x004FA5A0, 0x004FA5E0, 0x004FA620, 0x004FA660, 0x004FA6A0, 0x004FA6E0, 0x004FA730 and 0x004FA7A0 are all transcribed. Cases 0x0 to 0x6 read only the low byte or word of arg4 in retail (partial-register loads leave the upper bits undefined); the port passes the zero-extended value, and every callee stores only that low byte or word, so nothing observable differs. The switch has no default label, matching the out-of-range jump to 0x004F9FB8.

// 0x004F9E70: graph-stream name hash, transcribed in engine/src/retail/core/native_4f9e70.cpp (one argument).
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

#include <cstdint>
extern "C" {
std::uint32_t NATIVE_DECL GraphStream_HashName_4F9E70(char const* name);
}

#include <cstring>


extern "C" {
// 0x004FA4E0: writes the two-byte record {0x00, byte}, transcribed in native_4fa4e0.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_WriteTag0Byte_4FA4E0(void* stream, std::int32_t value);

// 0x004FA520: writes the two-byte record {0x01, byte}, transcribed in native_4fa520.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_WriteTag1Byte_4FA520(void* stream, std::int32_t value);

// 0x004FA560: writes the three-byte record {0x02, word}, transcribed in native_4fa560.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_WriteTag2Word_4FA560(void* stream, std::int32_t value);

// 0x004FA5A0: writes the three-byte record {0x03, word}, transcribed in native_4fa5a0.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_WriteTag3Word_4FA5A0(void* stream, std::int32_t value);

// 0x004FA5E0: writes the five-byte record {0x04, dword}, transcribed in native_4fa5e0.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_WriteTag4Dword_4FA5E0(void* stream, std::int32_t value);

// 0x004FA620: writes the five-byte record {0x05, dword}, transcribed in native_4fa620.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_WriteTag5Dword_4FA620(void* stream, std::int32_t value);

// 0x004FA660: writes the five-byte record {0x06, dword}, transcribed in native_4fa660.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_WriteTag6Dword_4FA660(void* stream, std::int32_t value);

// 0x004FA6A0: writes the nine-byte record {0x07, dword, dword}, transcribed in native_4fa6a0.cpp (three arguments).
std::uint8_t NATIVE_DECL GraphStream_WriteTag7Pair_4FA6A0(void* stream, std::int32_t first, std::int32_t second);

// 0x004FA6E0: writes the 25-byte record {0x08, 24 bytes}, transcribed in native_4fa6e0.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_WriteTag8Block_4FA6E0(void* stream, void const* block);

// 0x004FA730: writes the length-prefixed string record, transcribed in native_4fa730.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_WriteTag9String_4FA730(void* stream, char const* text);

// 0x004FA7A0: writes the length-prefixed blob record, transcribed in native_4fa7a0.cpp (three arguments).
std::uint8_t NATIVE_DECL GraphStream_WriteTagABlob_4FA7A0(void* stream, void const* data, std::int32_t size);

std::uint8_t NATIVE_DECL GraphStream_AddNode_4F9EB0(void* stream, char const* name, std::int32_t selector,
                                                   void const* value, std::int32_t size)
{
    // 0x004F9EB0: mov 0x8(%esp),%eax (arg2); 0x004F9EB6: push %eax; 0x004F9EB7: call 0x4f9e70.
    std::uint32_t const hash = GraphStream_HashName_4F9E70(name);

    // 0x004F9EC0: push %eax (hash); 0x004F9EC1: push %esi (stream); 0x004F9EC2: call 0x4fa5e0.
    GraphStream_WriteTag4Dword_4FA5E0(stream, static_cast<std::int32_t>(hash));

    // 0x004F9ECB: push %edi (arg3); 0x004F9ECC: push %esi; 0x004F9ECD: call 0x4fa4e0.
    GraphStream_WriteTag0Byte_4FA4E0(stream, selector);

    // 0x004F9ED5: cmp $0xa,%edi; 0x004F9ED8: ja 0x4f9fb8; 0x004F9EDE: jmp *0x4f9fbc(,%edi,4).
    switch (selector)
    {
    case 0x0:                                                                   // 0x004F9EE5..0x004F9EF7
        return GraphStream_WriteTag0Byte_4FA4E0(stream, *static_cast<std::uint8_t const*>(value));
    case 0x1:                                                                   // 0x004F9EF8..0x004F9F0A
        return GraphStream_WriteTag1Byte_4FA520(stream, *static_cast<std::uint8_t const*>(value));
    case 0x2:                                                                   // 0x004F9F0B..0x004F9F1E
        return GraphStream_WriteTag2Word_4FA560(stream, *static_cast<std::uint16_t const*>(value));
    case 0x3:                                                                   // 0x004F9F1F..0x004F9F32
        return GraphStream_WriteTag3Word_4FA5A0(stream, *static_cast<std::uint16_t const*>(value));
    case 0x4:                                                                   // 0x004F9F33..0x004F9F45
        return GraphStream_WriteTag4Dword_4FA5E0(stream, static_cast<std::int32_t>(*static_cast<std::uint32_t const*>(value)));
    case 0x5:                                                                   // 0x004F9F46..0x004F9F58
        return GraphStream_WriteTag5Dword_4FA620(stream, static_cast<std::int32_t>(*static_cast<std::uint32_t const*>(value)));
    case 0x6:                                                                   // 0x004F9F59..0x004F9F6B
        return GraphStream_WriteTag6Dword_4FA660(stream, static_cast<std::int32_t>(*static_cast<std::uint32_t const*>(value)));
    case 0x7:                                                                   // 0x004F9F6C..0x004F9F82
        return GraphStream_WriteTag7Pair_4FA6A0(
            stream,
            static_cast<std::int32_t>(*static_cast<std::uint32_t const*>(value)),
            static_cast<std::int32_t>(*(static_cast<std::uint32_t const*>(value) + 1)));
    case 0x8:                                                                   // 0x004F9F83..0x004F9F93
        return GraphStream_WriteTag8Block_4FA6E0(stream, value);
    case 0x9:                                                                   // 0x004F9F94..0x004F9FA4
        return GraphStream_WriteTag9String_4FA730(stream, static_cast<char const*>(value));
    case 0xA:                                                                   // 0x004F9FA5..0x004F9FB7
        return GraphStream_WriteTagABlob_4FA7A0(stream, value, size);
    default:
        break;                                                                  // 0x004F9ED8: ja 0x4f9fb8
    }
    return 1U;                                                                  // 0x004F9FB8: pop %edi; pop %esi; ret - AL still holds the 0x004FA4E0 result
}
} // extern "C"

#undef NATIVE_DECL
