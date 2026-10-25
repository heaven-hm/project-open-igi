// Native: GraphStream_ReadTag9String_4FA950; retail VA 0x004FA950 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA950-0x004FA9C2 (115 bytes, 1 block, 39 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA950-0x004FA9C2; radare2 6.2.0 afij (115 bytes, 1 block, 39 instructions); called from 0x004F9CD0 case 0x9 with three arguments.
// Behavior/CFG: one block, no branch. Invokes driver slot 0x004B17C0, reads the two-byte header with 0x004B16D0(context, frame, 2), takes the length from the header byte at frame+1, invokes slot 0x004B17C0 again, reads that many bytes into frame+2, then copies them through arg2. Always reports 1 in AL.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 39-instruction listing; runtime UNOBSERVED. Link-closed: 0x004B17C0 and 0x004B16D0 are transcribed in engine/src/retail/qfile/. The third argument is pushed by the 0x004F9CD0 call site and never read: the length comes from the record header, exactly as retail does. The copy is retail rep movsd + rep movsb, kept as memcpy, and the 0x104-byte frame is scalar chunks rather than an array so the host compiler emits no __stack_chk pair.

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
std::uint8_t NATIVE_DECL GraphStream_ReadTag9String_4FA950(void* stream, void* out, std::int32_t unused_size)
{
    NATIVE_LOG_ENTER("GraphStream_ReadTag9String_4FA950", "0x004FA950");
    (void)unused_size;                                                          // pushed by 0x004F9CD0 at 0x004F9DA1, never read by this body

    struct Chunk                                                                // 64 bytes, scalars only: retail's frame is 0x104 bytes
    {
        std::uint32_t w0;
        std::uint32_t w1;
        std::uint32_t w2;
        std::uint32_t w3;
        std::uint32_t w4;
        std::uint32_t w5;
        std::uint32_t w6;
        std::uint32_t w7;
        std::uint32_t w8;
        std::uint32_t w9;
        std::uint32_t w10;
        std::uint32_t w11;
        std::uint32_t w12;
        std::uint32_t w13;
        std::uint32_t w14;
        std::uint32_t w15;
    };
    struct
    {
        Chunk first;
        Chunk second;
        Chunk third;
        Chunk fourth;
        std::uint32_t tail;                                                     // 4 * 64 + 4 = 0x104
    } frame = {};

    std::uint8_t* const frame_bytes = reinterpret_cast<std::uint8_t*>(&frame);

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);       // 0x004FA95F: mov 0x4(%edi),%eax
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));  // 0x004FA962/0x004FA963

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FA968: mov 0x4(%edi),%edx
    QFile_DeviceSlot10Call_4B16D0(reinterpret_cast<void*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&frame)),
                                  2);                                                               // 0x004FA96F..0x004FA973: push $0x2; push %ecx; push %edx; call 0x4b16d0

    std::uint32_t const length = static_cast<std::uint32_t>(frame_bytes[1]);      // 0x004FA97B/0x004FA980: mov 0x19(%esp),%esi; and $0xff,%esi

    std::uint32_t const handle_third =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FA978: mov 0x4(%edi),%eax
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle_third)));  // 0x004FA97F/0x004FA986

    std::uint32_t const handle_fourth =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FA98B: mov 0x4(%edi),%edx
    QFile_DeviceSlot10Call_4B16D0(reinterpret_cast<void*>(static_cast<std::uintptr_t>(handle_fourth)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(frame_bytes + 2U)),
                                  static_cast<std::int32_t>(length));                               // 0x004FA992..0x004FA995: push %esi; push %ecx; push %edx; call 0x4b16d0

    std::memcpy(out, frame_bytes + 2U, length);                                 // 0x004FA9A1..0x004FA9B8: mov %esi,%ecx; rep movsd; and $0x3,%ecx; rep movsb
    return 1U;                                                                  // 0x004FA9B6: mov $0x1,%al
}                                                                               // 0x004FA9C2: ret
} // extern "C"

#undef NATIVE_DECL
