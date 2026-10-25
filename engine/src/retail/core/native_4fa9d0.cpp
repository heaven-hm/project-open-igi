// Native: GraphList_Insert_4FA9D0; retail VA 0x004FA9D0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA9D0-0x004FAA0F (64 bytes, 1 block, 26 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA9D0-0x004FAA0F; radare2 6.2.0 afij (64 bytes, 1 block, 26 instructions); called from 0x004F9CD0 case 0xA and from 0x00509450 at 0x00509473.
// Behavior/CFG: one block, no branch. Invokes driver slot 0x004B17C0, reads the two-byte header through 0x004FA7E0, reads the five-byte header through 0x004FA850, invokes slot 0x004B17C0 again, then reads arg3 raw bytes through 0x004B16D0(context, arg2, arg3). Always reports 1 in AL.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 26-instruction listing; runtime UNOBSERVED. Link-closed: 0x004FA7E0, 0x004FA850, 0x004B17C0 and 0x004B16D0 are all transcribed. Both header reads land in retail dead slots - the first through the address of its own argument slot (0x004FA9D6 lea 0xc(%esp),%eax) and the second in its own pushed local (0x004FA9E1 lea 0xc(%esp),%ecx) - so the port gives each a private frame and the header values are still consumed from the stream in the same order.

// 0x004B17C0: qfile driver slot invoke, transcribed in engine/src/retail/qfile/native_4b17c0.cpp (one argument).
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

#include "dev_log.hpp"
#include <cstdint>
extern "C" {
std::int32_t NATIVE_DECL QFile_InvokeDriverSlot_4B17C0(std::int32_t* context);

// 0x004B16D0: qfile device slot 10 call, transcribed in engine/src/retail/qfile/native_4b16d0.cpp (three arguments).
std::int32_t NATIVE_DECL QFile_DeviceSlot10Call_4B16D0(void* context, std::int32_t argument2, std::int32_t argument3);

// 0x004FA850: reads a five-byte record and returns the dword at frame+1, transcribed in native_4fa850.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphList_PopHead_4FA850(void* list, std::int32_t* out);
}

#include <cstring>


extern "C" {
// 0x004FA7E0: reads the two-byte header, transcribed in native_4fa7e0.cpp (two arguments).
std::uint8_t NATIVE_DECL GraphStream_ReadTag0Byte_4FA7E0(void* stream, std::uint8_t* out);

std::uint8_t NATIVE_DECL GraphList_Insert_4FA9D0(void* list, void* value, std::uint32_t size)
{
    NATIVE_LOG_ENTER("GraphList_Insert_4FA9D0", "0x004FA9D0");
    struct
    {
        std::uint32_t slot;                                                     // retail reads the two-byte header into the address of its own argument slot
    } header_frame = {0U};
    struct
    {
        std::uint32_t low;                                                      // retail reads the five-byte header into its own pushed local
        std::uint32_t high;
    } length_frame = {0U, 0U};

    GraphStream_ReadTag0Byte_4FA7E0(list, reinterpret_cast<std::uint8_t*>(&header_frame));  // 0x004FA9DA/0x004FA9DC: push %eax; push %esi; call 0x4fa7e0
    GraphList_PopHead_4FA850(list, reinterpret_cast<std::int32_t*>(&length_frame));          // 0x004FA9E5/0x004FA9E7: push %ecx; push %esi; call 0x4fa850

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(list) + 0x04U);       // 0x004FA9EC: mov 0x4(%esi),%edx
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));  // 0x004FA9EF/0x004FA9F0

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(list) + 0x04U);                     // 0x004FA9FD: mov 0x4(%esi),%edx
    QFile_DeviceSlot10Call_4B16D0(reinterpret_cast<void*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(value)),
                                  static_cast<std::int32_t>(size));                                 // 0x004FAA00..0x004FAA03: push %eax (size); push %ecx (value); push %edx; call 0x4b16d0
    return 1U;                                                                  // 0x004FAA0B: mov $0x1,%al
}                                                                               // 0x004FAA0F: ret
} // extern "C"

#undef NATIVE_DECL
