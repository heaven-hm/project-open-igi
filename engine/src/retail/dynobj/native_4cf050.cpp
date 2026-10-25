// Native: DynObj_SweepPackageRecords_4CF050 (address-suffixed placeholder: retail's symbol name is unknown and no manifest record exists; the name matches the NATIVE GAP declared in dynobj/native_4cefa0.cpp so the two files agree); retail VA 0x004CF050-0x004CF072 (35 bytes, 5 blocks, 8 edges, 13 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a ret). The extent is bounded by the `ret` at 0x004CF072 and THIRTEEN `90` nops follow it (read byte-by-byte from the image; the next entry 0x004CF080 starts `push %esi`). Exact bytes 56 BE A0 43 A4 00 8D 46 14 85 C0 74 09 56 E8 ED FC FF FF 83 C4 04 83 C6 48 81 FE 60 EC A4 00 7C E5 5E C3 = `push %esi` / `mov $0x00A443A0,%esi` / `lea 0x14(%esi),%eax` / `test %eax,%eax` / `je 0x004CF066` / `push %esi` / `call 0x004CED50` / `add $0x4,%esp` / `add $0x48,%esi` / `cmp $0x00A4EC60,%esi` / `jl 0x004CF056` / `pop %esi` / `ret`. The call displacement resolves as v+5+rel32: 0x004CF05E + 0xFFFFFCED (-787) = 0x004CED50. Call census from the image's 0xE8 enumeration: TWO call sites image-wide, 0x004CF037 (the .mtp package opener in dynobj/native_4cefa0.cpp) and 0x004CFB1F - the tree's notes previously knew only the first.
// Behavior/CFG: no arguments, no frame, no return value. It walks the package-record table from 0x00A443A0 in 0x48-byte strides while the cursor is BELOW 0x00A4EC60, handing each record to 0x004CED50. Retail bounds the walk with the comparison `cmp $0x00A4EC60,%esi; jl` rather than with a counter, and this transcription keeps the comparison: the derived record count is (0x00A4EC60 - 0x00A443A0) / 0x48 = 0xA8C0 / 0x48 = 600, but the count is a consequence of the bound, not the bound itself. THE GUARD AT 0x004CF056 IS DEAD AND IS PRESERVED: `lea 0x14(%esi),%eax; test %eax,%eax; je 0x004CF066` tests a computed address inside a table that starts at 0x00A443A0, so it can never be null, the branch is never taken, and every record reaches 0x004CED50. A reader who tidies it into an unconditional call has changed the instruction stream, so the branch stays; the compiler may fold it, which is a codegen difference with no behavioral effect. The `jl` is a signed compare while C++ pointer comparison is unsigned, and the table start and end are both below 0x80000000, so the two cannot differ here.
// Status/limits: TRANSCRIBED; NOT LINKABLE - undefined in tree, measured: DynObj_LoadRecordVariants_4CED50 (0x004CED50, the companion file of this same increment) and nothing else. STRUCTURALLY_VERIFIED against the 13-instruction listing (5 blocks, 8 edges); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile, rc=0, zero warnings); runtime UNOBSERVED. Domain: dynobj/.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// NATIVE GAP: 0x004CED50 has no implementation under engine/ yet; it is the companion file of this increment
// (dynobj/native_4ced50.cpp), where the same symbol is defined. One cdecl pointer argument, no return value.
void NATIVE_DECL DynObj_LoadRecordVariants_4CED50(void* record);

void NATIVE_DECL DynObj_SweepPackageRecords_4CF050()
{
    // 0x004CF051: mov $0x00A443A0,%esi - the first record of the package table.
    std::uint8_t* record = reinterpret_cast<std::uint8_t*>(0x00A443A0);

    do
    {
        // 0x004CF056/0x004CF059/0x004CF05B: lea 0x14(%esi),%eax; test %eax,%eax; je 0x004CF066.
        // THE GUARD IS DEAD: `record + 0x14` is a computed address in a table that starts at 0x00A443A0, so it can
        // never be null and the branch is never taken. Retail carries the branch and it is preserved here rather
        // than simplified to an unconditional call.
        std::uint8_t* const guard = record + 0x14;
        if (guard != nullptr)
        {
            // 0x004CF05D/0x004CF05E/0x004CF063: push %esi; call 0x004CED50; add $0x4,%esp.
            DynObj_LoadRecordVariants_4CED50(record);
        }

        // 0x004CF066: add $0x48,%esi - the record stride.
        record += 0x48;
        // 0x004CF069/0x004CF06F: cmp $0x00A4EC60,%esi; jl 0x004CF056 - retail's bound is this comparison. The
        // comparison is kept as written; the 600-record count is derived from it, not the other way round.
    } while (record < reinterpret_cast<std::uint8_t*>(0x00A4EC60));
}

} // extern "C"

#undef NATIVE_DECL
