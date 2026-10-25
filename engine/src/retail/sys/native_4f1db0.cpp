// Native: Sys_NewListRecord_4F1DB0; retail VA 0x004F1DB0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F1DB0-0x004F1DF6 (71 bytes, 1 block, 0 edges, 22 instructions).
// Tools/evidence: radare2 6.2.0 af/afij and the build/retail-dis.txt bytes 56 6a04 680c480000 e8a3eefbff 8bf0 8b442414 50 898608480000 8d8608280000 8d4e08 50 51 c7460400000000 c70600000000 e8e9dbfbff 8b54241c 56 52 e8fedafbff 83c41c 5e c3; callees 0x004B0C60 (transcribed as igi::retail::mem::Mem_Alloc), 0x004AF9D0 (NATIVE GAP, declared here) and 0x004AF8F0 (transcribed as Sys_InsertListTail_4AF8F0 in sys/native_4af8f0.cpp).
// Behavior/CFG: one basic block, no branch of its own. Allocates 0x480C bytes with alignment 4 through 0x004B0C60, stores its second argument at record+0x4808, clears record+0x04 and record+0x00, initialises the record's two sub-objects at +0x08 and +0x2808 from that same second argument through 0x004AF9D0, then links the record into its first argument through 0x004AF8F0. 0x004AF9D0's three arguments are the two sub-object addresses pushed at 0x004F1DD3/0x004F1DD4 and the pattern pushed once at 0x004F1DC3 and left in place, so the third argument is the same value stored at +0x4808. Retail's `add $0x1c,%esp` at 0x004F1DF2 cleans the saved ESI plus exactly the six argument dwords pushed.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 22-instruction listing and afij (71 B / 1 block / 0 edges); runtime UNOBSERVED. 0x004AF9D0 is a NATIVE GAP declared here (1190 bytes / 68 blocks / 97 edges / 329 instructions; its only callees are 0x004AFFB0 and 0x004AFF90), so this file cannot link until it lands; its name is a placeholder derived from the call shape, not a retail symbol. The name and signature of this native match the declaration engine/src/retail/core/native_47cf20.cpp already carries for this VA, and 0x004AF8F0 is referenced by name rather than duplicated. The record's extent is not established beyond the fields this body writes, and the pattern is typed as a string pointer because the same pointer is stored at +0x4808 and handed to 0x004AF9D0; no byte of it is read here. Retail leaves EAX as 0x004AF8F0's return value, which the sole caller discards, so this file returns void as that caller's declaration states.
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004AF8F0: the list-tail insert half, transcribed as Sys_InsertListTail_4AF8F0 in engine/src/retail/sys/native_4af8f0.cpp.
void Sys_InsertListTail_4AF8F0(void* head, void* node);

// 0x004AF9D0: transcribed in sys/native_4af9d0.cpp (1190 bytes, 68 blocks, 97 edges, 329 instructions). The declaration that
// stood here read Sys_InitRecordPattern_4AF9D0 and returned void; both are corrected to match the definition. The name follows
// the owner file, which is evidence-derived: its two switches are a metacharacter class table (^ $ . * ? [ ] ( ) { } / \) and a
// backslash-escape handler (N/n, R/r, T/t), so the function compiles the pattern into 0x14-byte nodes in its first argument. The
// return type is std::int32_t because the body returns 1 on success and 0 at its fail block 0x004AFE69 - a void declaration
// would have dropped that result silently. Argument 2 is the class-record area this call passes as record+0x2808, which matches
// the owner's description of it as the class-record array handed to 0x004AFF90. The result is unused at this call site in retail
// too (the next instruction at 0x004F1DE7 reads 0x1c(%esp) without testing eax), so discarding it here is faithful.
std::int32_t Sys_CompilePattern_4AF9D0(std::uint8_t* node_array, std::uint8_t* class_records, char const* pattern);

void NATIVE_DECL Sys_NewListRecord_4F1DB0(void* head, char const* pattern)
{
    std::uint8_t* const record = static_cast<std::uint8_t*>(igi::retail::mem::Mem_Alloc(0x480C, 4)); // 0x004F1DB1: push $0x4; 0x004F1DB3: push $0x480C; 0x004F1DB8: call 0x004B0C60
    *reinterpret_cast<char const**>(record + 0x4808U) = pattern;           // 0x004F1DC3: push %eax; 0x004F1DC4: mov %eax,0x4808(%esi)
    *reinterpret_cast<std::uint32_t*>(record + 0x0004U) = 0U;              // 0x004F1DD5: movl $0x0,0x4(%esi)
    *reinterpret_cast<std::uint32_t*>(record + 0x0000U) = 0U;              // 0x004F1DDC: movl $0x0,(%esi)
    Sys_CompilePattern_4AF9D0(record + 0x0008U, record + 0x2808U, pattern); // 0x004F1DD0/0x004F1DD3/0x004F1DD4: lea 0x8(%esi),%ecx; lea 0x2808(%esi),%eax; push %eax; push %ecx; 0x004F1DE2: call 0x004AF9D0
    Sys_InsertListTail_4AF8F0(head, record);                               // 0x004F1DE7: mov 0x1c(%esp),%edx; 0x004F1DEB: push %esi; 0x004F1DEC: push %edx; 0x004F1DED: call 0x004AF8F0
}                                                                          // 0x004F1DF2: add $0x1c,%esp; 0x004F1DF5: pop %esi; 0x004F1DF6: ret

} // extern "C"

#undef NATIVE_DECL
