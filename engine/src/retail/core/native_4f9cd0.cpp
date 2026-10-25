// Native: GraphNode_Process_4F9CD0; retail VA 0x004F9CD0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F9CD0-0x004F9DC2 (243 bytes, 14 blocks, 102 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004F9CD0-0x004F9DC2; radare2 6.2.0 afij (243 bytes, 14 blocks, 102 instructions); called from 0x005092A0 at 0x005092CF with three arguments.
// Behavior/CFG: fourteen blocks: one 0x004FAA10 peek, then an eleven-entry dispatch through the table at 0x004F9DC4 (verified in the image as 0x004F9CF3, 0x4F9D05, 0x4F9D17, 0x4F9D29, 0x4F9D3B, 0x4F9D4D, 0x4F9D5F, 0x4F9D71, 0x4F9D83, 0x4F9D95, 0x4F9DAC) on the selector at node+0x10, each case re-reading the stream word at node+0x00 and reading one typed record into arg2; selectors above 0x0A jump to the shared 0x004F9DC1 tail, which reports the 0x004FAA10 result still in EAX.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 102-instruction listing and the 11-entry table read from the image; runtime UNOBSERVED. Link-closed: 0x004FAA10, 0x004FA7E0, 0x004FA810, 0x004FA850, 0x004FA890, 0x004FA8D0, 0x004FA910, 0x004FA950 and 0x004FA9D0 are all transcribed. Every case re-reads node+0x00 rather than reusing the peeked value, as retail does. The switch has no default label: retail jumps out of range to 0x004F9DC1 (pop %esi; ret), which is the same fall-through the port takes.

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

#include <cstdint>
#include <cstring>


extern "C" {
// 0x004FAA10: peeks one byte, transcribed in native_4faa10.cpp (one argument).
std::uint8_t NATIVE_DECL GraphStream_PeekByte_4FAA10(void* stream);

// 0x004FA7E0: reads the two-byte header, transcribed in native_4fa7e0.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_ReadTag0Byte_4FA7E0(void* stream, std::uint8_t* out);

// 0x004FA810: reads the three-byte header, transcribed in native_4fa810.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_ReadTag2Word_4FA810(void* stream, std::uint16_t* out);

// 0x004FA850: reads the five-byte header, transcribed in native_4fa850.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphList_PopHead_4FA850(void* list, std::int32_t* out);

// 0x004FA890: reads the five-byte float record, transcribed in native_4fa890.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_ReadTag4Float_4FA890(void* stream, float* out);

// 0x004FA8D0: reads the nine-byte double record, transcribed in native_4fa8d0.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_ReadTag8Double_4FA8D0(void* stream, double* out);

// 0x004FA910: reads the 25-byte block record, transcribed in native_4fa910.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_ReadTag8Block_4FA910(void* stream, void* out);

// 0x004FA950: reads the length-prefixed string record, transcribed in native_4fa950.cpp (three arguments, the third unread).
std::uint8_t NATIVE_DECL GraphStream_ReadTag9String_4FA950(void* stream, void* out, std::int32_t unused_size);

// 0x004FA9D0: reads two headers and then a raw block, transcribed in native_4fa9d0.cpp (three arguments).
std::uint8_t NATIVE_DECL GraphList_Insert_4FA9D0(void* list, void* value, std::uint32_t size);

std::uint8_t NATIVE_DECL GraphNode_Process_4F9CD0(void* node, void* arg2, std::uint32_t size)
{
    std::uint8_t* const fields = static_cast<std::uint8_t*>(node);

    // 0x004F9CD5: mov (%esi),%eax; 0x004F9CD7: push %eax; 0x004F9CD8: call 0x4faa10.
    std::uint8_t const peeked = GraphStream_PeekByte_4FAA10(
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(fields))));

    // 0x004F9CDD: mov 0x10(%esi),%eax; 0x004F9CE0: add $0x4,%esp; 0x004F9CE3: cmp $0xa,%eax; 0x004F9CE6: ja 0x4f9dc1; 0x004F9CEC: jmp *0x4f9dc4(,%eax,4).
    switch (*reinterpret_cast<std::uint32_t*>(fields + 0x10U))
    {
    case 0x0:                                                                   // 0x004F9CF3..0x004F9D04
        return GraphStream_ReadTag0Byte_4FA7E0(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(fields))),
            static_cast<std::uint8_t*>(arg2));
    case 0x1:                                                                   // 0x004F9D05..0x004F9D16
        return GraphStream_ReadTag0Byte_4FA7E0(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(fields))),
            static_cast<std::uint8_t*>(arg2));
    case 0x2:                                                                   // 0x004F9D17..0x004F9D28
        return GraphStream_ReadTag2Word_4FA810(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(fields))),
            static_cast<std::uint16_t*>(arg2));
    case 0x3:                                                                   // 0x004F9D29..0x004F9D3A
        return GraphStream_ReadTag2Word_4FA810(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(fields))),
            static_cast<std::uint16_t*>(arg2));
    case 0x4:                                                                   // 0x004F9D3B..0x004F9D4C
        return GraphList_PopHead_4FA850(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(fields))),
            static_cast<std::int32_t*>(arg2));
    case 0x5:                                                                   // 0x004F9D4D..0x004F9D5E
        return GraphList_PopHead_4FA850(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(fields))),
            static_cast<std::int32_t*>(arg2));
    case 0x6:                                                                   // 0x004F9D5F..0x004F9D70
        return GraphStream_ReadTag4Float_4FA890(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(fields))),
            static_cast<float*>(arg2));
    case 0x7:                                                                   // 0x004F9D71..0x004F9D82
        return GraphStream_ReadTag8Double_4FA8D0(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(fields))),
            static_cast<double*>(arg2));
    case 0x8:                                                                   // 0x004F9D83..0x004F9D94
        return GraphStream_ReadTag8Block_4FA910(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(fields))), arg2);
    case 0x9:                                                                   // 0x004F9D95..0x004F9DAB
        return GraphStream_ReadTag9String_4FA950(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(fields))), arg2,
            static_cast<std::int32_t>(size));
    case 0xA:                                                                   // 0x004F9DAC..0x004F9DC0
        return GraphList_Insert_4FA9D0(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(fields))), arg2,
            size);
    default:
        break;                                                                  // 0x004F9CE6: ja 0x4f9dc1
    }
    return peeked;                                                              // 0x004F9DC1: pop %esi; ret - EAX still holds the 0x004FAA10 result
}
} // extern "C"

#undef NATIVE_DECL
