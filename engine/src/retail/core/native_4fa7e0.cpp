// Native: GraphStream_ReadTag0Byte_4FA7E0; retail VA 0x004FA7E0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA7E0-0x004FA80E (47 bytes, 1 block, 18 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA7E0-0x004FA80E; radare2 6.2.0 afij (47 bytes, 1 block, 18 instructions); called from 0x004F9CD0 cases 0x0 and 0x1, and from 0x004FA9D0 at 0x004FA9DC.
// Behavior/CFG: one block, no branch. Invokes driver slot 0x004B17C0, reads two bytes with 0x004B16D0(context, frame, 2), then stores the byte at frame+1 through arg2. Always reports 1 in AL.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 18-instruction listing; runtime UNOBSERVED. Link-closed: 0x004B17C0 and 0x004B16D0 are transcribed in engine/src/retail/qfile/. Retail reads the record into its own argument slot (0x004FA7F1 lea 0xc(%esp),%ecx is the arg1 slot) and reads the value back from frame+1 (0x004FA802 mov 0x19(%esp),%cl); the port uses a private four-byte frame, which is the same two bytes and leaves the caller slot untouched.

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
std::uint8_t NATIVE_DECL GraphStream_ReadTag0Byte_4FA7E0(void* stream, std::uint8_t* out)
{
    struct
    {
        std::uint32_t slot;                                                     // the two-byte record; retail reads it into its own argument slot
    } frame = {0U};

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);       // 0x004FA7E5: mov 0x4(%esi),%eax
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));  // 0x004FA7E8/0x004FA7E9

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FA7EE: mov 0x4(%esi),%edx
    QFile_DeviceSlot10Call_4B16D0(reinterpret_cast<void*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&frame)),
                                  2);                                                               // 0x004FA7F5..0x004FA7F9: push $0x2; push %ecx; push %edx; call 0x4b16d0

    *out = *(reinterpret_cast<std::uint8_t*>(&frame) + 1U);                      // 0x004FA802/0x004FA809: mov 0x19(%esp),%cl; mov %cl,(%eax)
    return 1U;                                                                  // 0x004FA80B: mov $0x1,%al
}                                                                               // 0x004FA80E: ret
} // extern "C"

#undef NATIVE_DECL
