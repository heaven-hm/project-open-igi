// Native: GraphNode_Init_4F90B0; retail VA 0x004F90B0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F90B0-0x004F9157 (168 bytes, 3 blocks, 60 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004F90B0-0x004F9157; radare2 6.2.0 pd/afij (168 bytes, 3 blocks, 60 instructions); the list header 0x00A77248 and the type word read at [object+0x14]+0x1C are the only data it touches besides its callees.
// Behavior/CFG: three blocks, one branch on the 0x00401CF0 result. Initialises the field block 0x20..0x6C, allocates a 0x0C-byte node (alignment 4), stores the 0x00401AE0 slot address at node+0x08, clears the node's two link words and inserts it into the 0x00A77248 list; then reads the 16-bit type word at [object+0x14]+0x1C into DI, re-clears the 0x20..0x34 field group, calls 0x00401CF0(type word in DI with EDI's original high half, 0x004C48C0()) and sets byte object+0x64 when it reports true; finally wires the object's 0x70/0x74/0x78 links to each other. The second argument is never read.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 60-instruction listing; runtime UNOBSERVED. Link-blocked on 0x00401AE0
//                (61 bytes; transcribed in native_401ae0.cpp) and 0x00401CF0 (49 bytes; transcribed in native_401cf0.cpp). 0x004B0C60 is
//                already transcribed as igi::retail::mem::Mem_Alloc. Two retail quirks are preserved: 0x004F9117 `mov %di,[eax+0x1c]` keeps EDI's high half (the node pointer) instead of
//                zero-extending the type word, and 0x004F9107 stores the slot address as a 32-bit dword at node+0x08.
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

// 0x004AF8F0: the list-tail insert half, transcribed as Sys_InsertListTail_4AF8F0 in engine/src/retail/sys/native_4af8f0.cpp. It is
// declared inside the extern "C" block below because that transcription now exports C linkage; the namespace wrapper it used to carry
// was removed on 2026-10-25 so that every retail VA has one linkage convention.

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void* Sys_InsertListTail_4AF8F0(void* head, void* node);   // 0x004AF8F0, engine/src/retail/sys/native_4af8f0.cpp

// 0x004B0C60: the engine allocator, already transcribed as igi::retail::mem::Mem_Alloc (engine/src/retail/mem/mem_alloc.cpp).

// 0x00401AE0: the QHash slot-table insert (its release counterpart 0x00401B20 is transcribed in core/native_401b20.cpp, which documents
// 0x00401AE0 as the routine that returns the slot address). NOT transcribed yet - declared only.
void* QHashSlotTable_Insert_401AE0(void* object);

// 0x00401CF0: walks the task-type parent chain from arg1's low word and reports whether arg2 appears before the 0x0181 root; it reads only
// AX of the first argument and CX of the second. NOT transcribed yet - declared only.
std::uint8_t TaskType_Inherits_401CF0(std::uint32_t type, std::uint16_t ancestor);

// 0x004C48C0: transcribed in engine/src/retail/sys/native_4c48c0.cpp (a no-argument 16-bit getter).
std::uint16_t Sys_GetGlobal16_4C48C0();

// 0x00A77248: the node list header this routine inserts into. Only +0x00, +0x04 and +0x08 are touched by the transcribed code, so the extent
// is provisional; retail leaves it zero-filled (no file bytes).
void* g_a77248[3] = {nullptr, nullptr, nullptr};

void NATIVE_DECL GraphNode_Init_4F90B0(void* object, void* arg2)
{
    (void)arg2;                                                               // retail never reads its second argument
    std::uint8_t* const fields = static_cast<std::uint8_t*>(object);

    // 0x004F90BB..0x004F90F1: the field stores. Retail interleaves the two argument pushes (0x004F90B9 `push $0x4`, 0x004F90C4 `push $0xc`)
    // with these stores; the store order below is retail's.
    *reinterpret_cast<std::uint32_t*>(fields + 0x20U) = 0U;                   // 0x004F90BB
    *reinterpret_cast<std::uint32_t*>(fields + 0x28U) = 0U;                   // 0x004F90BE
    *reinterpret_cast<std::uint32_t*>(fields + 0x30U) = 0U;                   // 0x004F90C1
    *reinterpret_cast<std::uint32_t*>(fields + 0x50U) = 0xFFFFFFFFU;          // 0x004F90C6
    *reinterpret_cast<std::uint32_t*>(fields + 0x54U) = 0U;                   // 0x004F90CD
    *reinterpret_cast<std::uint32_t*>(fields + 0x58U) = 0U;                   // 0x004F90D0
    *reinterpret_cast<std::uint32_t*>(fields + 0x5CU) = 0U;                   // 0x004F90D3
    *reinterpret_cast<std::uint32_t*>(fields + 0x60U) = 0U;                   // 0x004F90D6
    *reinterpret_cast<std::uint8_t*>(fields + 0x65U) = 0U;                    // 0x004F90D9
    *reinterpret_cast<std::uint8_t*>(fields + 0x68U) = 0U;                    // 0x004F90DC
    *reinterpret_cast<std::uint8_t*>(fields + 0x69U) = 0U;                    // 0x004F90DF
    *reinterpret_cast<std::uint8_t*>(fields + 0x66U) = 0U;                    // 0x004F90E2
    *reinterpret_cast<std::uint8_t*>(fields + 0x67U) = 0U;                    // 0x004F90E5
    *reinterpret_cast<std::uint32_t*>(fields + 0x24U) = 0U;                   // 0x004F90E8
    *reinterpret_cast<std::uint32_t*>(fields + 0x2CU) = 0U;                   // 0x004F90EB
    *reinterpret_cast<std::uint32_t*>(fields + 0x34U) = 0U;                   // 0x004F90EE
    *reinterpret_cast<std::uint32_t*>(fields + 0x6CU) = 0U;                   // 0x004F90F1

    // 0x004F90F4: call 0x4b0c60 - size 0x0C in the last push (0x004F90C4), alignment 4 in the first (0x004F90B9).
    std::uint8_t* const node = static_cast<std::uint8_t*>(igi::retail::mem::Mem_Alloc(0x0C, 4));

    // 0x004F90F9/0x004F90FC: push %esi; mov %edi,%eax; call 0x401ae0 - (object) returns the slot address.
    void* const slot = QHashSlotTable_Insert_401AE0(object);
    *reinterpret_cast<std::uint32_t*>(node + 0x08U) = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(slot)); // 0x004F9107
    *reinterpret_cast<std::uint32_t*>(node + 0x04U) = 0U;                     // 0x004F910A: mov %ebx,0x4(%edi)
    *reinterpret_cast<std::uint32_t*>(node) = 0U;                             // 0x004F910D: mov %ebx,(%edi)

    // 0x004F9101/0x004F9102/0x004F910F: push %edi; push $0xa77248; call 0x4af8f0 - insert the node at the tail of the 0x00A77248 list.
    Sys_InsertListTail_4AF8F0(static_cast<void*>(g_a77248), node);

    // 0x004F9114: mov 0x14(%esi),%eax; 0x004F9117: mov %di,0x1c(%eax) - the low half of EDI (the node pointer) is replaced by the type word
    // and the high half survives, so the value handed to 0x00401CF0 mixes both. 0x00401CF0 reads only AX, but retail's value is reproduced.
    std::uint32_t const type_word = *reinterpret_cast<std::uint16_t*>(
        static_cast<std::uint8_t*>(*reinterpret_cast<void**>(fields + 0x14U)) + 0x1CU);
    std::uint32_t const hybrid = (static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(node)) & 0xFFFF0000U) | type_word;

    // 0x004F911B..0x004F912A: the second field clear group, then 0x004F912D: mov $0x0,0x64(%esi).
    *reinterpret_cast<std::uint32_t*>(fields + 0x20U) = 0U;                   // 0x004F911B
    *reinterpret_cast<std::uint32_t*>(fields + 0x28U) = 0U;                   // 0x004F911E
    *reinterpret_cast<std::uint32_t*>(fields + 0x30U) = 0U;                   // 0x004F9121
    *reinterpret_cast<std::uint32_t*>(fields + 0x24U) = 0U;                   // 0x004F9124
    *reinterpret_cast<std::uint32_t*>(fields + 0x2CU) = 0U;                   // 0x004F9127
    *reinterpret_cast<std::uint32_t*>(fields + 0x34U) = 0U;                   // 0x004F912A
    *reinterpret_cast<std::uint8_t*>(fields + 0x64U) = 0U;                    // 0x004F912D

    // 0x004F9130: call 0x4c48c0; 0x004F9135: push %eax; 0x004F9136: push %edi; 0x004F9137: call 0x401cf0.
    if (TaskType_Inherits_401CF0(hybrid, Sys_GetGlobal16_4C48C0()) != 0U)     // 0x004F913F/0x004F9141: test %al,%al; je 0x4f9147
    {
        *reinterpret_cast<std::uint8_t*>(fields + 0x64U) = 1U;                // 0x004F9143: mov $0x1,0x64(%esi)
    }

    // 0x004F9147: lea 0x74(%esi),%eax; 0x004F914A: lea 0x70(%esi),%ecx; 0x004F914D: mov %ecx,0x78(%esi);
    // 0x004F9151: mov %ebx,(%eax); 0x004F9154: mov %eax,(%ecx).
    std::uint8_t* const link = fields + 0x74U;
    std::uint8_t* const head = fields + 0x70U;
    *reinterpret_cast<std::uint32_t*>(fields + 0x78U) = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(head));
    *reinterpret_cast<std::uint32_t*>(link) = 0U;
    *reinterpret_cast<std::uint32_t*>(head) = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(link));
}                                                                             // 0x004F9157: ret
} // extern "C"

#undef NATIVE_DECL
