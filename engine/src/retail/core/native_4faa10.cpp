// Native: GraphStream_PeekByte_4FAA10; retail VA 0x004FAA10 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FAA10-0x004FAA51 (66 bytes, 1 block, 24 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FAA10-0x004FAA51; radare2 6.2.0 afij (66 bytes, 1 block, 24 instructions); called from 0x004F9CD0 at 0x004F9CD8 with one argument.
// Behavior/CFG: one block, no branch. Invokes driver slot 0x004B17C0, reads one byte with 0x004B16D0(context, frame, 1), invokes slot 0x004B17C0 again and seeks back one byte with 0x004B1730(context, -1, 1), then reports the byte in AL. The stream position is unchanged.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 24-instruction listing; runtime UNOBSERVED. Link-closed: 0x004B17C0, 0x004B16D0 and 0x004B1730 are transcribed in engine/src/retail/qfile/. Retail reads the byte into its own argument slot (0x004FAA21 lea 0xc(%esp),%ecx) and then returns 0x28(%esp), a slot one word further out that the single-argument call never fills, so retail returns a stale stack byte; the port returns the byte it just read. The only call site (0x004F9CD0) discards the result, so the difference is unobservable there.

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

// 0x004B1730: qfile driver slot invoke, transcribed in engine/src/retail/qfile/native_4b1730.cpp (three arguments).
std::int32_t NATIVE_DECL QFile_InvokeDriverSlot_4B1730(std::int32_t* context, std::int32_t argument2,
                                                      std::int32_t argument3);
}

#include <cstring>


extern "C" {
std::uint8_t NATIVE_DECL GraphStream_PeekByte_4FAA10(void* stream)
{
    NATIVE_LOG_ENTER("GraphStream_PeekByte_4FAA10", "0x004FAA10");
    struct
    {
        std::uint32_t slot;                                                     // the single byte retail reads into its own argument slot
    } frame = {0U};

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);       // 0x004FAA15: mov 0x4(%esi),%eax
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));  // 0x004FAA18/0x004FAA19

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FAA1E: mov 0x4(%esi),%edx
    QFile_DeviceSlot10Call_4B16D0(reinterpret_cast<void*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&frame)),
                                  1);                                                               // 0x004FAA25..0x004FAA29: push $0x1; push %ecx; push %edx; call 0x4b16d0

    std::uint32_t const handle_third =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FAA2E: mov 0x4(%esi),%eax
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle_third)));  // 0x004FAA31/0x004FAA32

    std::uint32_t const handle_fourth =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FAA37: mov 0x4(%esi),%ecx
    QFile_InvokeDriverSlot_4B1730(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle_fourth)),
                                  -1,
                                  1);                                                               // 0x004FAA3A..0x004FAA3F: push $0x1; push $0xffffffff; push %ecx; call 0x4b1730

    return *reinterpret_cast<std::uint8_t*>(&frame);                             // 0x004FAA44/0x004FAA4B: mov 0x28(%esp),%eax; and $0xff,%eax
}                                                                               // 0x004FAA51: ret
} // extern "C"

#undef NATIVE_DECL
