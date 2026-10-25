// Native: GraphStream_WriteTag8Block_4FA6E0; retail VA 0x004FA6E0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA6E0-0x004FA720 (65 bytes, 1 block, 24 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA6E0-0x004FA720; radare2 6.2.0 afij (65 bytes, 1 block, 24 instructions); called from 0x004F9EB0 case 0x8 at 0x004F9F89.
// Behavior/CFG: one block, no branch. Copies the 24 bytes at arg2 into a 25-byte record {0x08, 24 payload bytes} in its 0x1c-byte frame, invokes driver slot 0x004B17C0, then writes the record with 0x004B1700(context, record, 0x19). Always reports 1 in AL.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 24-instruction listing; runtime UNOBSERVED. Link-closed: 0x004B17C0 and 0x004B1700 are transcribed in engine/src/retail/qfile/. The 24-byte copy is retail rep movsl $0x6, kept as memcpy; the frame is scalars rather than an array so the host compiler emits no __stack_chk pair.

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
std::uint8_t NATIVE_DECL GraphStream_WriteTag8Block_4FA6E0(void* stream, void const* block)
{
    NATIVE_LOG_ENTER("GraphStream_WriteTag8Block_4FA6E0", "0x004FA6E0");
    struct
    {
        std::uint32_t w0;                                                       // retail's 0x1c-byte frame (sub $0x1c,%esp);
        std::uint32_t w1;                                                       // the 25-byte record occupies its first 25 bytes
        std::uint32_t w2;
        std::uint32_t w3;
        std::uint32_t w4;
        std::uint32_t w5;
        std::uint32_t w6;
    } record;

    *reinterpret_cast<std::uint8_t*>(&record) = 8U;                             // 0x004FA6F2: movb $0x8,0x8(%esp)
    std::memcpy(reinterpret_cast<std::uint8_t*>(&record) + 1U, block, 0x18U);    // 0x004FA6E9/0x004FA6F7: mov $0x6,%ecx; rep movsl

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);       // 0x004FA6F9: mov 0x28(%esp),%esi
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));  // 0x004FA700/0x004FA701

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FA706: mov 0x4(%esi),%edx
    // 0x004FA709: lea 0xc(%esp),%ecx (record); 0x004FA70D: push $0x19; 0x004FA70F: push %ecx; 0x004FA710: push %edx; 0x004FA711: call 0x4b1700.
    // Retail pushes three words here (length, record, handle) and a fourth word 0x004B1700 never reads. This stub's signature takes four and
    // forwards its second and third words to the driver, so the record and the length sit in positions two and three; position four is the dead
    // slot, and the literal 0 there stands for the word retail pushes but the callee never touches (0x004B1700 reads arg1/arg2/arg3 only).
    QFile_InvokeDriverSlot_4B1700(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&record)),
                                  25,                                                            // 0x004FA70D: push the record length
                                  0);
    return 1U;                                                                  // 0x004FA719: mov $0x1,%al
}                                                                               // 0x004FA720: ret
} // extern "C"

#undef NATIVE_DECL
