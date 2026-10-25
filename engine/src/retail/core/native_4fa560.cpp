// Native: GraphStream_WriteTag2Word_4FA560; retail VA 0x004FA560 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA560-0x004FA595 (54 bytes, 1 block, 20 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA560-0x004FA595; radare2 6.2.0 afij (54 bytes, 1 block, 20 instructions); called from 0x004F9EB0 case 0x2.
// Behavior/CFG: one block, no branch. Stores the three-byte record {0x02, word from arg2} in its four-byte frame, invokes driver slot 0x004B17C0, then writes the record with 0x004B1700(context, record, 3). Always reports 1 in AL.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 20-instruction listing; runtime UNOBSERVED. Link-closed: 0x004B17C0 and 0x004B1700 are transcribed in engine/src/retail/qfile/. The word lands at record+1, deliberately unaligned, as retail stores it.

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
std::uint8_t NATIVE_DECL GraphStream_WriteTag2Word_4FA560(void* stream, std::int32_t value)
{
    NATIVE_LOG_ENTER("GraphStream_WriteTag2Word_4FA560", "0x004FA560");
    struct
    {
        std::uint32_t slot;                                                     // retail's four-byte frame (push %ecx)
    } record;

    *reinterpret_cast<std::uint8_t*>(&record) = 2U;                            // the record tag byte
    std::uint16_t const word = static_cast<std::uint16_t>(value);                // the value is read as a word in retail
    std::memcpy(reinterpret_cast<std::uint8_t*>(&record) + 1U, &word, 2U);       // unaligned word at record+1

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);
    // the record pointer and the three-byte length are pushed last, the handle before them.
    // Retail pushes three words here (length, record, handle) and a fourth word 0x004B1700 never reads. This stub's signature takes four and
    // forwards its second and third words to the driver, so the record and the length sit in positions two and three; position four is the dead
    // slot, and the literal 0 there stands for the word retail pushes but the callee never touches (0x004B1700 reads arg1/arg2/arg3 only).
    QFile_InvokeDriverSlot_4B1700(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&record)),
                                  3,                                                             // 0x004FA585: push the record length
                                  0);
    return 1U;
}
} // extern "C"

#undef NATIVE_DECL
