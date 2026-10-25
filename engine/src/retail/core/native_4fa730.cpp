// Native: GraphStream_WriteTag9String_4FA730; retail VA 0x004FA730 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA730-0x004FA79C (109 bytes, 1 block, 41 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA730-0x004FA79C; radare2 6.2.0 afij (109 bytes, 1 block, 41 instructions); called from 0x004F9EB0 case 0x9 at 0x004F9F9F.
// Behavior/CFG: one block, no branch. Measures arg2 with a byte scan, then stores the record {0x09, length byte = strlen+1, the text including its terminator} in its 0x104-byte frame, invokes driver slot 0x004B17C0 and writes the record with 0x004B1700(context, record, strlen+3). Always reports 1 in AL.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 41-instruction listing; runtime UNOBSERVED. Link-closed: 0x004B17C0 and 0x004B1700 are transcribed in engine/src/retail/qfile/. The scan is retail repnz scasb (no libc strlen call) and the copy is rep movsd + rep movsb; the 0x104-byte frame is scalar chunks rather than an array so the host compiler emits no __stack_chk pair. The length byte counts the terminator, so the written record is strlen+3 bytes.

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
std::uint8_t NATIVE_DECL GraphStream_WriteTag9String_4FA730(void* stream, char const* text)
{
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
    } record;

    std::uint32_t length = 0U;                                                  // 0x004FA742..0x004FA750: or $0xffffffff,%ecx; xor %eax,%eax; repnz scasb; not %ecx; dec %ecx
    while (text[length] != '\0')
    {
        ++length;
    }
    ++length;                                                                   // 0x004FA755/0x004FA757: mov %ecx,%ebx; inc %ebx - the length byte counts the terminator

    std::uint8_t* const record_bytes = reinterpret_cast<std::uint8_t*>(&record);
    record_bytes[0] = 9U;                                                       // 0x004FA747: movb $0x9,0xc(%esp)
    record_bytes[1] = static_cast<std::uint8_t>(length);                        // 0x004FA75A: mov %bl,0xd(%esp)
    std::memcpy(record_bytes + 2U, text, length);                               // 0x004FA763/0x004FA76A: rep movsd + rep movsb of %ebx bytes

    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);       // 0x004FA76C: mov 0x114(%esp),%esi
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));  // 0x004FA776/0x004FA777

    std::uint32_t const handle_again =
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(stream) + 0x04U);                   // 0x004FA77C: mov 0x4(%esi),%eax
    std::uint32_t const record_size = length + 2U;                              // 0x004FA77F: add $0x2,%ebx
    // 0x004FA782: lea 0x10(%esp),%edx (record); 0x004FA786: push %ebx; 0x004FA787: push %edx; 0x004FA788: push %eax; 0x004FA789: call 0x4b1700.
    // Retail pushes three words here (length, record, handle) and a fourth word 0x004B1700 never reads. This stub's signature takes four and
    // forwards its second and third words to the driver, so the record and the length sit in positions two and three; position four is the dead
    // slot, and the literal 0 there stands for the word retail pushes but the callee never touches (0x004B1700 reads arg1/arg2/arg3 only).
    QFile_InvokeDriverSlot_4B1700(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&record)),
                                  static_cast<std::int32_t>(record_size),                        // 0x004FA786: push the record length
                                  0);
    return 1U;                                                                  // 0x004FA791: mov $0x1,%al
}                                                                               // 0x004FA79C: ret
} // extern "C"

#undef NATIVE_DECL
