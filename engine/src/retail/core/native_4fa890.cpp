// Native: GraphStream_ReadTag4Float_4FA890; retail VA 0x004FA890 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA890-0x004FA8C4 (53 bytes, 1 block, 20 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA890-0x004FA8C4; radare2 6.2.0 afij (53 bytes, 1 block, 20 instructions); called from 0x004F9CD0 case 0x6.
// Behavior/CFG: one block, no branch. Invokes driver slot 0x004B17C0, reads five bytes with 0x004B16D0(context, frame, 5), then stores the float at frame+1 through arg2. Always reports 1 in AL.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 20-instruction listing; runtime UNOBSERVED. Link-closed: 0x004B17C0 and 0x004B16D0 are transcribed in engine/src/retail/qfile/. Retail loads the value with flds 0x15(%esp) and stores it with fstps (%eax); a four-byte copy is the same single-precision transfer.

// 0x004B17C0: qfile driver slot invoke, transcribed in engine/src/retail/qfile/native_4b17c0.cpp (one argument).
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

#include <cstdint>
extern "C" {
std::int32_t NATIVE_DECL QFile_InvokeDriverSlot_4B17C0(std::int32_t* context);

// 0x004B16D0: qfile device slot 10 call, transcribed in engine/src/retail/qfile/native_4b16d0.cpp (three arguments).
std::int32_t NATIVE_DECL QFile_DeviceSlot10Call_4B16D0(void* context, std::int32_t argument2, std::int32_t argument3);
}

#include <cstring>


extern "C" {
std::uint8_t NATIVE_DECL GraphStream_ReadTag4Float_4FA890(void* stream, float* out)
{
    struct
    {
        std::uint32_t low;                                                      // retail's eight-byte frame (sub $0x8,%esp);
        std::uint32_t high;                                                     // the five-byte record occupies its first five bytes
    } frame = {0U, 0U};

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);       // 0x004FA898: mov 0x4(%esi),%eax
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));  // 0x004FA89B/0x004FA89C

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FA8A1: mov 0x4(%esi),%edx
    QFile_DeviceSlot10Call_4B16D0(reinterpret_cast<void*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&frame)),
                                  5);                                                               // 0x004FA8A8..0x004FA8AC: push $0x5; push %ecx; push %edx; call 0x4b16d0

    float value = 0.0F;
    std::memcpy(&value, reinterpret_cast<std::uint8_t*>(&frame) + 1U, 4U);       // 0x004FA8B1: flds 0x15(%esp)
    *out = value;                                                               // 0x004FA8BC: fstps (%eax)
    return 1U;                                                                  // 0x004FA8BE: mov $0x1,%al
}                                                                               // 0x004FA8C4: ret
} // extern "C"

#undef NATIVE_DECL
