// Native: Sub_4F1340 (address-suffixed placeholder: this VA has no manifest record, and although the body is a release routine it establishes no retail name); retail VA 0x004F1340-0x004F13B6.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 decode of the exact extent: 44 instructions, 11 basic blocks, 20 edges, 4 call sites, `ret` at 0x004F13B6, 9 `90` nops after. Exact bytes: 56 8B 74 24 08 57 33 FF 8B 46 4C 85 C0 7E 1B 53 8D 5E 0C 8B 03 50 E8 C5 07 F1 FF 8B 46 4C 83 C4 04 47 83 C3 04 3B F8 7C EA 5B 8B 06 C7 46 4C 00 00 00 00 85 C0 74 0F 50 E8 93 F9 FB FF 83 C4 04 C7 06 00 00 00 00 8B 46 04 85 C0 74 10 50 E8 7D F9 FB FF 83 C4 04 C7 46 04 00 00 00 00 8B 46 08 85 C0 74 10 50 E8 36 88 FC FF 83 C4 04 C7 46 08 00 00 00 00 5F 5E C3. ENTRY BOUNDARY: an image-wide arithmetic scan of every `E8` displacement finds 94 call sites; the byte before the call at 0x0041AD53 is `50` = push eax, so retail's callers pass exactly one argument. The extent is bounded by the `ret` and confirmed by the caller file core/native_449330.cpp, which records the tail jump (`add $0x4,%esp; pop %ebx; jmp Sub_4F1340`) that enters it.
// Behavior/CFG: two loops/guards over one region pointer. First, if the signed count at region+0x4C is positive, walk the count of 4-byte slots starting at region+0x0C and call QHashSlotTable_Release on each; the count is re-read from region+0x4C on every iteration (0x004F135B), because the release may change it, and the loop compares the signed index against that fresh value. Then region+0x4C is zeroed, and each of region+0x00, region+0x04 and region+0x08 is freed if non-null - the first two through Mem_DeAllocFn, the third through 0x004B9BE0 - each followed by storing null. No return value is produced; the caller cleans the argument (the body ends `pop edi; pop esi; ret` with no `add esp`).
// Status/limits: TRANSCRIBED; NOT LINKABLE if its three callees are absent from the tree (measured undefined list is reported to the Lead rather than hidden). STRUCTURALLY_VERIFIED against the 44-instruction listing (11 blocks, 20 edges, extent bounded by the `ret`); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile); runtime UNOBSERVED. What the region is and what 0x004B9BE0 does are not established by this body, so 0x004B9BE0 keeps its address-suffixed placeholder. Domain: core/, because its caller is core/native_449330.cpp and the region is one of the core task structures.

#include <cstdint>

#ifndef NATIVE_DECL
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif
#endif

extern "C" {

// 0x00401B20: called once per slot in the loop below. Named from its manifest record.
void QHashSlotTable_Release(void* table);

// 0x004B0D10: the allocator's free, named from its manifest record.
void Mem_DeAllocFn(void* block);

// NATIVE GAP: 0x004B9BE0 has no manifest record and no file. Measured: one call site here (0x004F13A5), one argument,
// result unused; the body was not read, so it keeps an address-suffixed placeholder.
void Sub_4B9BE0(void* block);

void NATIVE_DECL Sub_4F1340(void* region)
{
    std::uint8_t* const base = static_cast<std::uint8_t*>(region);

    std::int32_t count = *reinterpret_cast<std::int32_t*>(base + 0x4C); // 0x004F1348: mov eax,[esi+0x4c]; 0x004F134B: test eax,eax; 0x004F134D: jle
    if (count > 0) {
        std::uint8_t* slot = base + 0x0C; // 0x004F1350: lea ebx,[esi+0xc]
        std::int32_t index = 0;           // 0x004F1346: xor edi,edi
        do {
            QHashSlotTable_Release(*reinterpret_cast<void**>(slot)); // 0x004F1353: mov eax,[ebx]; 0x004F1355: push eax; 0x004F1356: call 0x401b20
            count = *reinterpret_cast<std::int32_t*>(base + 0x4C);   // 0x004F135B: the count is re-read every iteration, not cached
            ++index;                                                 // 0x004F1361: inc edi
            slot += 4;                                               // 0x004F1362: add ebx,4
        } while (index < count);                                     // 0x004F1365: cmp edi,eax; 0x004F1367: jl (signed)
    }

    void* const first = *reinterpret_cast<void**>(base);            // 0x004F136A: mov eax,[esi]
    *reinterpret_cast<std::int32_t*>(base + 0x4C) = 0;              // 0x004F136C: mov dword [esi+0x4c],0
    if (first != nullptr) {                                         // 0x004F1373: test eax,eax; 0x004F1375: je
        Mem_DeAllocFn(first);                                       // 0x004F1377: push eax; 0x004F1378: call 0x4b0d10
        *reinterpret_cast<void**>(base) = nullptr;                  // 0x004F1380: mov dword [esi],0
    }

    void* const second = *reinterpret_cast<void**>(base + 4);       // 0x004F1386: mov eax,[esi+4]
    if (second != nullptr) {                                        // 0x004F1389: test eax,eax; 0x004F138B: je
        Mem_DeAllocFn(second);                                      // 0x004F138D: push eax; 0x004F138E: call 0x4b0d10
        *reinterpret_cast<void**>(base + 4) = nullptr;              // 0x004F1396: mov dword [esi+4],0
    }

    void* const third = *reinterpret_cast<void**>(base + 8);        // 0x004F139D: mov eax,[esi+8]
    if (third != nullptr) {                                         // 0x004F13A0: test eax,eax; 0x004F13A2: je
        Sub_4B9BE0(third);                                          // 0x004F13A4: push eax; 0x004F13A5: call 0x4b9be0
        *reinterpret_cast<void**>(base + 8) = nullptr;              // 0x004F13AD: mov dword [esi+8],0
    }
    // 0x004F13B4: pop edi; 0x004F13B5: pop esi; 0x004F13B6: ret (cdecl - the caller cleans the argument).
}

} // extern "C"

#undef NATIVE_DECL
