// Native: GraphStream_ReadTag8Block_4FA910; retail VA 0x004FA910 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA910-0x004FA94B (60 bytes, 1 block, 23 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA910-0x004FA94B; radare2 6.2.0 afij (60 bytes, 1 block, 23 instructions); called from 0x004F9CD0 case 0x8.
// Behavior/CFG: one block, no branch. Invokes driver slot 0x004B17C0, reads 25 bytes with 0x004B16D0(context, frame, 0x19), then copies the 24 bytes at frame+1 through arg2. Always reports 1 in AL.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 23-instruction listing; runtime UNOBSERVED. Link-closed: 0x004B17C0 and 0x004B16D0 are transcribed in engine/src/retail/qfile/. The 24-byte copy is retail rep movsl $0x6, kept as memcpy; the 0x1c-byte frame is scalars rather than an array so the host compiler emits no __stack_chk pair.

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
std::uint8_t NATIVE_DECL GraphStream_ReadTag8Block_4FA910(void* stream, void* out)
{
    NATIVE_LOG_ENTER("GraphStream_ReadTag8Block_4FA910", "0x004FA910");
    struct
    {
        std::uint32_t w0;                                                       // retail's 0x1c-byte frame (sub $0x1c,%esp);
        std::uint32_t w1;                                                       // the 25-byte record occupies its first 25 bytes
        std::uint32_t w2;
        std::uint32_t w3;
        std::uint32_t w4;
        std::uint32_t w5;
        std::uint32_t w6;
    } frame = {0U, 0U, 0U, 0U, 0U, 0U, 0U};

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);       // 0x004FA914: mov 0x24(%esp),%esi
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));  // 0x004FA91C/0x004FA91D

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FA922: mov 0x4(%esi),%edx
    QFile_DeviceSlot10Call_4B16D0(reinterpret_cast<void*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&frame)),
                                  0x19);                                                            // 0x004FA929..0x004FA92D: push $0x19; push %ecx; push %edx; call 0x4b16d0

    std::memcpy(out, reinterpret_cast<std::uint8_t*>(&frame) + 1U, 0x18U);       // 0x004FA936/0x004FA942: mov $0x6,%ecx; rep movsl
    return 1U;                                                                  // 0x004FA945: mov $0x1,%al
}                                                                               // 0x004FA94B: ret
} // extern "C"

#undef NATIVE_DECL
