// Native: DynObj_OpenModelTexturePackage_4CEFA0 (address-suffixed placeholder; the retail symbol is unknown and this name is derived from the measured behavior). retail VA 0x004CEFA0-0x004CF043 (0xA4 bytes, 1 block, 0 edges, 51 instructions in the reachable set).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 decode from the verified entry over the exact image. The extent is bounded by the `ret` at 0x004CF043, and TWELVE alignment nops `90` x12 follow it (the next entry 0x004CF050 starts `push %esi`). Exact bytes 83 EC 40 56 57 6A 04 6A 4C E8 B2 1C FE FF 8B F0 56 68 60 EC A4 00 C7 46 04 00 00 00 00 C7 06 00 00 00 00 C7 46 48 00 00 00 00 E8 21 09 FE FF 8D 46 14 8D 4E 10 8B 7C 24 5C 89 4E 18 89 01 C7 00 00 00 00 00 8D 46 2C 8D 4E 28 89 4E 30 C7 46 20 00 00 00 00 89 01 C7 00 00 00 00 00 8D 46 20 8D 4E 1C 57 68 6C ED A4 00 89 01 8D 44 24 20 68 64 82 54 00 50 89 4E 24 E8 97 63 FD FF 8D 4C 24 28 6A 00 51 E8 D8 6E FE FF 6A 01 57 56 89 46 08 E8 4C 00 00 00 83 C4 34 E8 14 00 00 00 8B C6 5F 5E 83 C4 40 C3 = `sub $0x40,%esp` / `push %esi` / `push %edi` / `push $0x4` / `push $0x4C` / `call 0x004B0C60` / `mov %eax,%esi` / `push %esi` / `push $0x00A4EC60` / `movl $0x0,0x4(%esi)` / `movl $0x0,(%esi)` / `movl $0x0,0x48(%esi)` / `call 0x004AF8F0` / `lea 0x14(%esi),%eax` / `lea 0x10(%esi),%ecx` / `mov 0x5C(%esp),%edi` / `mov %ecx,0x18(%esi)` / `mov %eax,(%ecx)` / `movl $0x0,(%eax)` / `lea 0x2C(%esi),%eax` / `lea 0x28(%esi),%ecx` / `mov %ecx,0x30(%esi)` / `movl $0x0,0x20(%esi)` / `mov %eax,(%ecx)` / `movl $0x0,(%eax)` / `lea 0x20(%esi),%eax` / `lea 0x1C(%esi),%ecx` / `push %edi` / `push $0x00A4ED6C` / `mov %eax,(%ecx)` / `lea 0x20(%esp),%eax` / `push $0x00548264` / `push %eax` / `mov %ecx,0x24(%esi)` / `call 0x004A53B3` / `lea 0x28(%esp),%ecx` / `push $0x0` / `push %ecx` / `call 0x004B5F00` / `push $0x1` / `push %edi` / `push %esi` / `mov %eax,0x8(%esi)` / `call 0x004CF080` / `add $0x34,%esp` / `call 0x004CF050` / `mov %esi,%eax` / `pop %edi` / `pop %esi` / `add $0x40,%esp` / `ret`. Every call displacement resolves as v+5+rel32: 0x004CEFA9 + 0xFFFF1CB2 = 0x004B0C60; 0x004CEFCA + 0xFFFE0921 = 0x004AF8F0; 0x004CF017 + 0xFFFD6397 = 0x004A53B3 (the CRT sprintf); 0x004CF023 + 0xFFFE6ED8 = 0x004B5F00; 0x004CF02F + 0x0000004C = 0x004CF080; 0x004CF037 + 0x00000014 = 0x004CF050. Call census from the image's 0xE8 enumeration: 0x004CEFA0 has exactly TWO call sites, 0x00415C9E and 0x00415CE1, both inside the phase-0 level-start handler (core/game_flow.cpp) - the tree's note for that handler records only the first of the two.
// Behavior/CFG: one cdecl argument (char const* name) returning the new package record; no frame beyond a 0x40-byte local buffer, and one single cleanup `add esp,0x34` at 0x004CF034 for all THIRTEEN argument dwords pushed since the frame was opened (0x34 = 52 = 13 dwords, measured against the push census, so no callee cleans its own arguments). The record is Mem_Alloc(0x4C,4) - 76 bytes - and the first three stores clear +0x04, +0x00 and +0x48 in that order; then 0x004AF8F0(&g_a4ec60, record) appends it to the global package list at 0x00A4EC60. Three embedded twelve-byte sentinels are then initialised in place: at +0x10 {first=+0x10 -> +0x14, middle=+0x14 = 0, back=+0x18 -> +0x10}, at +0x28 {first=+0x28 -> +0x2C, middle=+0x2C = 0, back=+0x30 -> +0x28}, and at +0x1C {first=+0x1C -> +0x20, back=+0x24 -> +0x1C}, the last one's middle dword +0x20 being the `movl $0x0,0x20(%esi)` written earlier at 0x004CEFED - the same {first,middle,back} shape the script symbol table uses, and the same three lists 0x004CF080 fills through HeapList_Insert. The name is then built by sprintf(0x004A53B3) into the 0x40-byte frame local with the format at 0x00548264 = "%s%s.mtp", whose FIRST %s is the global prefix buffer 0x00A4ED6C and whose SECOND is this function's argument (the caller passes a name buffer, `lea edx,[edi+0x444]` at 0x00415C97); the result is loaded through 0x004B5F00 (Resource_Load) with a NULL out-size slot and the handle is stored at record+0x08, the store sitting between 0x004CF080's argument pushes and its call. Then 0x004CF080(record, name, 1) - the already-transcribed DynObj_LoadModelTexturePackage, whose ONLY call site image-wide is 0x004CF02F inside this body - and finally 0x004CF050() with no arguments and its result discarded. The record pointer is the return value (`mov %esi,%eax` at 0x004CF03C); the caller stores it into `[0x0057BABC]+0x24`. Straight-line: 51 instructions, 1 block, 0 edges, no branches at all. NOTE: the buffer at 0x00A4ED6C holds zero bytes in the image (BSS) and the tree initialises its owner copy to "LOCAL:"; this transcription reads the global rather than inlining the literal so that 0x004CF080's animation path and this opener read the same buffer, which is the §10.14 rule-10 alternative that was considered and rejected here for that reason.
// Status/limits: TRANSCRIBED; NOT LINKABLE - undefined in tree, measured: 0x004CF050 (no manifest record, no declaration, no implementation under engine/) and the two globals 0x00A4EC60 / 0x00A4ED6C, which dynobj/mtp_package_loader.cpp defines INSIDE AN UNNAMED NAMESPACE, so their definitions have internal linkage and the `extern "C"` declarations below cannot resolve to them. No callee was stubbed to make this link. STRUCTURALLY_VERIFIED against the 51-instruction listing (1 block, 0 edges) and BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile, rc=0, zero warning lines); runtime UNOBSERVED. The store order inside each sentinel initialisation is written in retail's order, but the compiler is free to schedule independent stores, so byte-identical code generation is not claimed. Domain: dynobj/, because the record is the MTP package the dynobj loader 0x004CF080 fills.

#include "dev_log.hpp"
#include <cstdint>
#include <cstdio>

#include "igi/retail/dynobj/mtp_package_loader.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004AF8F0: appends `node` to the tail of the list whose head is `head` (sys/native_4af8f0.cpp).
void* NATIVE_DECL Sys_InsertListTail_4AF8F0(void* head, void* node);

// NATIVE GAP: 0x004CF050 has no implementation file in this tree (measured: the manifest has no record for this VA
// and no declaration exists under engine/). Declared here so the call below is a real symbol reference rather than a
// cast, and THIS FILE CANNOT LINK UNTIL IT LANDS. Measured: 13 instructions, extent 0x004CF050-0x004CF072 bounded by
// its `ret` with thirteen nops after, no arguments, and its result is discarded at this call site (retail overwrites
// eax at 0x004CF03C). It sweeps 0x48-byte records from 0x00A443A0 up to (not including) 0x00A4EC60 and calls
// 0x004CED50 on each record whose dword at +0x14 is non-null; its other call site is 0x004CFB1F.
void NATIVE_DECL DynObj_SweepPackageRecords_4CF050();

// NATIVE GAP (two retail globals, no owner visible to this translation unit): 0x00A4EC60 is the head of the global
// package list passed to Sys_InsertListTail_4AF8F0, and 0x00A4ED6C is the 0x8C-byte path-prefix buffer the sprintf
// below reads as its first %s. dynobj/mtp_package_loader.cpp holds both names but defines them in an unnamed
// namespace ("Port-owned storage for shared package pointer 0x00A4EC60" / "package path prefix 0x00A4ED6C"), so they
// have internal linkage: these declarations are undefined at link until that file exposes them. Names match the
// tree's own names for the same addresses so there is one name per address.
extern "C" void* g_a4ec60;
extern "C" char g_a4ed6c[0x8C];

void* NATIVE_DECL DynObj_OpenModelTexturePackage_4CEFA0(char const* name)
{
    NATIVE_LOG_ENTER("DynObj_OpenModelTexturePackage_4CEFA0", "0x004CEFA0");
    // 0x004CEFA5/0x004CEFA7/0x004CEFA9: push 4; push 0x4C; call 0x004B0C60 - the 0x4C-byte package record.
    std::uint8_t* const package = static_cast<std::uint8_t*>(igi::retail::mem::Mem_Alloc(0x4C, 4));

    // 0x004CEFB6/0x004CEFBD/0x004CEFC3: three fields cleared, in retail's order, before the record is linked.
    *reinterpret_cast<std::uint32_t*>(package + 0x04) = 0;
    *reinterpret_cast<std::uint32_t*>(package + 0x00) = 0;
    *reinterpret_cast<std::uint32_t*>(package + 0x48) = 0;

    // 0x004CEFB0/0x004CEFB1/0x004CEFCA: push record; push &g_a4ec60; call 0x004AF8F0 - append to the global list.
    // The return value is discarded: retail overwrites eax at 0x004CEFCF.
    (void)Sys_InsertListTail_4AF8F0(&g_a4ec60, package);

    // 0x004CEFCF/0x004CEFD2: lea eax,[esi+0x14]; lea ecx,[esi+0x10] - the first sentinel's own slots.
    std::uint8_t* const sentinel_10 = package + 0x10;
    std::uint8_t* const sentinel_10_middle = package + 0x14;
    *reinterpret_cast<std::uint8_t**>(package + 0x18) = sentinel_10;       // 0x004CEFD9: mov [esi+0x18],ecx
    *reinterpret_cast<std::uint8_t**>(sentinel_10) = sentinel_10_middle;   // 0x004CEFDC: mov [ecx],eax
    *reinterpret_cast<std::uint8_t**>(sentinel_10_middle) = nullptr;       // 0x004CEFDE: mov [eax],0

    // 0x004CEFE4/0x004CEFE7: lea eax,[esi+0x2c]; lea ecx,[esi+0x28] - the second sentinel's own slots.
    std::uint8_t* const sentinel_28 = package + 0x28;
    std::uint8_t* const sentinel_28_middle = package + 0x2C;
    *reinterpret_cast<std::uint8_t**>(package + 0x30) = sentinel_28;       // 0x004CEFEA: mov [esi+0x30],ecx
    *reinterpret_cast<std::uint32_t*>(package + 0x20) = 0;                 // 0x004CEFED: mov [esi+0x20],0
    *reinterpret_cast<std::uint8_t**>(sentinel_28) = sentinel_28_middle;   // 0x004CEFF4: mov [ecx],eax
    *reinterpret_cast<std::uint8_t**>(sentinel_28_middle) = nullptr;       // 0x004CEFF6: mov [eax],0

    // 0x004CEFFC/0x004CEFFF: lea eax,[esi+0x20]; lea ecx,[esi+0x1c] - the third sentinel. Its middle dword at +0x20
    // is the word cleared at 0x004CEFED above, which is why no second clear appears here.
    std::uint8_t* const sentinel_1C = package + 0x1C;
    std::uint8_t* const sentinel_1C_middle = package + 0x20;
    *reinterpret_cast<std::uint8_t**>(sentinel_1C) = sentinel_1C_middle;   // 0x004CF008: mov [ecx],eax
    *reinterpret_cast<std::uint8_t**>(package + 0x24) = sentinel_1C;       // 0x004CF014: mov [esi+0x24],ecx

    // 0x004CF002/0x004CF003/0x004CF00E/0x004CF013/0x004CF017: push name; push 0x00A4ED6C; push 0x00548264;
    // push &buffer; call 0x004A53B3. 0x00548264 is "%s%s.mtp" byte-for-byte, and `sub esp,0x40` at 0x004CEFA0 is
    // the destination frame this buffer models. The first %s is the global prefix, the second is the argument.
    char buffer[0x40];
    ::sprintf(buffer, "%s%s.mtp", g_a4ed6c, name);

    // 0x004CF01C/0x004CF020/0x004CF022/0x004CF023: push 0; push &buffer; call 0x004B5F00 - the out-size slot is the
    // literal null, so this load asks for no size back.
    // 0x004CF028/0x004CF02A/0x004CF02B/0x004CF02C/0x004CF02F: push 1; push name; push record; store the handle;
    // call 0x004CF080. Retail stores the handle at 0x004CF02C, between those pushes and the call.
    *reinterpret_cast<void**>(package + 0x08) = igi::retail::resource::Resource_Load(buffer, nullptr);
    igi::retail::dynobj::DynObj_LoadModelTexturePackage(package, name, 1);

    // 0x004CF034: add esp,0x34 - the single cleanup for all thirteen argument dwords pushed since the frame.
    // 0x004CF037: call 0x004CF050 - the package-record sweep, no arguments, result discarded.
    DynObj_SweepPackageRecords_4CF050();

    return package;                                                        // 0x004CF03C: mov %esi,%eax
}

} // extern "C"

#undef NATIVE_DECL
