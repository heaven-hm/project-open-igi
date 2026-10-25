// Native: GraphStream_ReadTag2Word_4FA810; retail VA 0x004FA810 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA810-0x004FA840 (49 bytes, 1 block, 18 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA810-0x004FA840; radare2 6.2.0 afij (49 bytes, 1 block, 18 instructions); called from 0x004F9CD0 cases 0x2 and 0x3.
// Behavior/CFG: one block, no branch. Invokes driver slot 0x004B17C0, reads three bytes with 0x004B16D0(context, frame, 3), then stores the word at frame+1 through arg2. Always reports 1 in AL.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 18-instruction listing; runtime UNOBSERVED. Link-closed: 0x004B17C0 and 0x004B16D0 are transcribed in engine/src/retail/qfile/. The word is read from frame+1 and stored through arg2 as retail does; the port keeps the same deliberately unaligned offsets.

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
}

#include <cstring>


extern "C" {
std::uint8_t NATIVE_DECL GraphStream_ReadTag2Word_4FA810(void* stream, std::uint16_t* out)
{
    NATIVE_LOG_ENTER("GraphStream_ReadTag2Word_4FA810", "0x004FA810");
    struct
    {
        std::uint32_t slot;                                                     // the three-byte record in retail's four-byte argument slot
    } frame = {0U};

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);       // 0x004FA815: mov 0x4(%esi),%eax
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));  // 0x004FA818/0x004FA819

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FA81E: mov 0x4(%esi),%edx
    QFile_DeviceSlot10Call_4B16D0(reinterpret_cast<void*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&frame)),
                                  3);                                                               // 0x004FA825..0x004FA829: push $0x3; push %ecx; push %edx; call 0x4b16d0

    std::uint16_t word = 0U;
    std::memcpy(&word, reinterpret_cast<std::uint8_t*>(&frame) + 1U, 2U);        // 0x004FA832: mov 0x19(%esp),%cx
    *out = word;                                                                // 0x004FA83A: mov %cx,(%eax)
    return 1U;                                                                  // 0x004FA83D: mov $0x1,%al
}                                                                               // 0x004FA840: ret
} // extern "C"

#undef NATIVE_DECL
