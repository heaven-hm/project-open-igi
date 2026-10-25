// Native: GraphStream_WriteTag1Byte_4FA520; retail VA 0x004FA520 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA520-0x004FA553 (52 bytes, 1 block, 20 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA520-0x004FA553; radare2 6.2.0 afij (52 bytes, 1 block, 20 instructions); called from 0x004F9EB0 case 0x1.
// Behavior/CFG: one block, no branch. Stores the two-byte record {0x01, low byte of arg2} in its four-byte frame, invokes driver slot 0x004B17C0, then writes the record with 0x004B1700(context, record, 2). Always reports 1 in AL.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 20-instruction listing; runtime UNOBSERVED. Link-closed: 0x004B17C0 and 0x004B1700 are transcribed in engine/src/retail/qfile/. The record is two scalars rather than a byte array because an address-taken array makes the host compiler emit a __stack_chk pair retail never calls.

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

// 0x004B1700: qfile driver slot invoke, transcribed in engine/src/retail/qfile/native_4b1700.cpp (four arguments).
std::int32_t NATIVE_DECL QFile_InvokeDriverSlot_4B1700(std::int32_t* context, std::int32_t argument2,
                                                      std::int32_t argument3, std::int32_t argument4);
}

#include <cstring>


extern "C" {
std::uint8_t NATIVE_DECL GraphStream_WriteTag1Byte_4FA520(void* stream, std::int32_t value)
{
    struct
    {
        std::uint32_t slot;                                                     // retail's four-byte frame (push %ecx)
    } record;

    *reinterpret_cast<std::uint8_t*>(&record) = 1U;                            // the record tag byte
    *(reinterpret_cast<std::uint8_t*>(&record) + 1U) =
        static_cast<std::uint8_t>(value);                                       // the value byte

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);       // the driver handle at stream+0x04
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);
    // the record pointer and the two-byte length are pushed last, the handle before them.
    // Retail pushes three words here (length, record, handle) and a fourth word 0x004B1700 never reads. This stub's signature takes four and
    // forwards its second and third words to the driver, so the record and the length sit in positions two and three; position four is the dead
    // slot, and the literal 0 there stands for the word retail pushes but the callee never touches (0x004B1700 reads arg1/arg2/arg3 only).
    QFile_InvokeDriverSlot_4B1700(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&record)),
                                  2,                                                             // 0x004FA543: push the record length
                                  0);
    return 1U;
}
} // extern "C"

#undef NATIVE_DECL
