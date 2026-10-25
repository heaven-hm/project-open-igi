// Native: GraphStream_WriteTagABlob_4FA7A0; retail VA 0x004FA7A0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA7A0-0x004FA7D8 (57 bytes, 1 block, 24 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA7A0-0x004FA7D8; radare2 6.2.0 afij (57 bytes, 1 block, 24 instructions); called from 0x004F9EB0 case 0xA at 0x004F9FB0.
// Behavior/CFG: one block, no branch. Writes the tag byte 0x0A through 0x004FA4E0, writes the length through 0x004FA620, then invokes driver slot 0x004B17C0 and writes the raw arg3 bytes at arg2 with 0x004B1700(context, arg2, arg3). Always reports 1 in AL.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 24-instruction listing; runtime UNOBSERVED. Link-closed: 0x004FA4E0, 0x004FA620, 0x004B17C0 and 0x004B1700 are all transcribed. The blob itself is written straight from arg2, with no copy through the frame.

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

// 0x004B1700: qfile driver slot invoke, transcribed in engine/src/retail/qfile/native_4b1700.cpp (four arguments).
std::int32_t NATIVE_DECL QFile_InvokeDriverSlot_4B1700(std::int32_t* context, std::int32_t argument2,
                                                      std::int32_t argument3, std::int32_t argument4);
}

#include <cstring>


extern "C" {
// 0x004FA4E0: writes the two-byte record {0x00, byte}, transcribed in native_4fa4e0.cpp.
std::uint8_t NATIVE_DECL GraphStream_WriteTag0Byte_4FA4E0(void* stream, std::int32_t value);

// 0x004FA620: writes the five-byte record {0x05, dword}, transcribed in native_4fa620.cpp.
std::uint8_t NATIVE_DECL GraphStream_WriteTag5Dword_4FA620(void* stream, std::int32_t value);

std::uint8_t NATIVE_DECL GraphStream_WriteTagABlob_4FA7A0(void* stream, void const* data, std::int32_t size)
{
    NATIVE_LOG_ENTER("GraphStream_WriteTagABlob_4FA7A0", "0x004FA7A0");
    GraphStream_WriteTag0Byte_4FA4E0(stream, 0xA);                              // 0x004FA7A6/0x004FA7A9: push $0xa; push %esi; call 0x4fa4e0
    GraphStream_WriteTag5Dword_4FA620(stream, size);                            // 0x004FA7AE..0x004FA7B4: mov 0x1c(%esp),%edi; push %edi; push %esi; call 0x4fa620

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);       // 0x004FA7B9: mov 0x4(%esi),%eax
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));  // 0x004FA7BC/0x004FA7BD

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FA7C2: mov 0x4(%esi),%edx
    // 0x004FA7C9: push %edi (arg3, the byte count); 0x004FA7CA: push %ecx (arg2, the blob); 0x004FA7CB: push %edx; 0x004FA7CC: call 0x4b1700.
    // Retail pushes three words here (length, record, handle) and a fourth word 0x004B1700 never reads. This stub's signature takes four and
    // forwards its second and third words to the driver, so the record and the length sit in positions two and three; position four is the dead
    // slot, and the literal 0 there stands for the word retail pushes but the callee never touches (0x004B1700 reads arg1/arg2/arg3 only).
    QFile_InvokeDriverSlot_4B1700(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(data)),
                                  size,                                                          // 0x004FA7C9: push the record length
                                  0);
    return 1U;                                                                  // 0x004FA7D4: mov $0x1,%al
}                                                                               // 0x004FA7D8: ret
} // extern "C"

#undef NATIVE_DECL
