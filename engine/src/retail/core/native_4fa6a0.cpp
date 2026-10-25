// Native: GraphStream_WriteTag7Pair_4FA6A0; retail VA 0x004FA6A0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA6A0-0x004FA6DF (64 bytes, 1 block, 22 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA6A0-0x004FA6DF; radare2 6.2.0 afij (64 bytes, 1 block, 22 instructions); called from 0x004F9EB0 case 0x7 at 0x004F9F78.
// Behavior/CFG: one block, no branch. Stores the nine-byte record {0x07, dword arg2, dword arg3} in its twelve-byte frame, invokes driver slot 0x004B17C0, then writes the record with 0x004B1700(context, record, 9). Always reports 1 in AL.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 22-instruction listing; runtime UNOBSERVED. Link-closed: 0x004B17C0 and 0x004B1700 are transcribed in engine/src/retail/qfile/. Both dwords land at record+1 and record+5, deliberately unaligned, as retail stores them.

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
std::uint8_t NATIVE_DECL GraphStream_WriteTag7Pair_4FA6A0(void* stream, std::int32_t first, std::int32_t second)
{
    NATIVE_LOG_ENTER("GraphStream_WriteTag7Pair_4FA6A0", "0x004FA6A0");
    struct
    {
        std::uint32_t low;                                                      // retail's twelve-byte frame (sub $0xc,%esp)
        std::uint32_t middle;
        std::uint32_t high;
    } record;

    *reinterpret_cast<std::uint8_t*>(&record) = 7U;                             // 0x004FA6B0: movb $0x7,0x4(%esp)
    std::uint32_t const first_value = static_cast<std::uint32_t>(first);         // 0x004FA6A3: mov 0x14(%esp),%eax
    std::uint32_t const second_value = static_cast<std::uint32_t>(second);       // 0x004FA6A7: mov 0x18(%esp),%ecx
    std::memcpy(reinterpret_cast<std::uint8_t*>(&record) + 1U, &first_value, 4U);   // 0x004FA6B5: mov %eax,0x5(%esp)
    std::memcpy(reinterpret_cast<std::uint8_t*>(&record) + 5U, &second_value, 4U);  // 0x004FA6BC: mov %ecx,0x9(%esp)

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);       // 0x004FA6B9: mov 0x4(%esi),%edx
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));  // 0x004FA6C0/0x004FA6C1

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FA6C6: mov 0x4(%esi),%ecx
    // 0x004FA6C9: lea 0x8(%esp),%eax (record); 0x004FA6CD: push $0x9; 0x004FA6CF: push %eax; 0x004FA6D0: push %ecx; 0x004FA6D1: call 0x4b1700.
    // Retail pushes three words here (length, record, handle) and a fourth word 0x004B1700 never reads. This stub's signature takes four and
    // forwards its second and third words to the driver, so the record and the length sit in positions two and three; position four is the dead
    // slot, and the literal 0 there stands for the word retail pushes but the callee never touches (0x004B1700 reads arg1/arg2/arg3 only).
    QFile_InvokeDriverSlot_4B1700(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&record)),
                                  9,                                                             // 0x004FA6CD: push the record length
                                  0);
    return 1U;                                                                  // 0x004FA6D9: mov $0x1,%al
}                                                                               // 0x004FA6DF: ret
} // extern "C"

#undef NATIVE_DECL
