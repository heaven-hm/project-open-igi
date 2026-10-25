// Native: QTask_HandlerC_42FE80; retail VA 0x0042FE80 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0042FE80-0x0042FEE3 (100 bytes, 1 block, 0 edges, 36 instructions).
// Tools/evidence: GNU objdump i386 disassembly build/retail-dis.txt (0x0042FE80-0x0042FEE3), with the byte span and instruction count re-derived from
//   that listing; the dispatch ABI, the two loader call sites and the sibling class callbacks are in docs/fidelity/evidence/qtask-handler-abi.md.
// Behavior/CFG: the parse callback of the class whose id word is 0x0057BD30 ("Wheel"), linked at the private phase byte 0x0054DC30 by 0x0042FE40. The
//   loader 0x004EDD20 allocates the 0x2F0-byte class block at 0x004EDF12, then calls this handler with (0, &context) at 0x004EDF48, where context =
//   {block, QVM argument frame, next argument index} lives in the loader's frame. The handler reads a 0x10-byte argument block into block+0x2D8, then
//   two QVM arguments scaled by the float at 0x005333B8 into block+0x2E8 and block+0x2EC. It never reads its first argument and it does not call the
//   base class callback (retail has no table dispatch here). Every read takes its index from context+0x8 and leaves the cursor incremented before the
//   reader call. Three calls (0x004B8A80, 0x004B8A50 x2), two fmuls by 0x005333B8, no branch, single block.
// Status/limits: TRANSCRIBED (static only; no build and no runtime). This class block has no base-class fill: the 0x2D8 bytes before the fields written
//   here are left as allocated by the retail, and the candidate reproduces that. The reader 0x004B8A80 is not transcribed in engine/ yet, so it is
//   declared here under an address-suffixed placeholder and must be unified when its own native lands; its return value is ignored here. The retail
//   leaves the last callee's residue in EAX on return and no caller reads it, so this declaration returns void. The candidate compiles for the x86
//   target as a single translation unit (i686-w64-mingw32-g++ 16.2.0, -std=c++20 -O2); the compiled body keeps the caller's store order, the three
//   call sites with their cursor updates, the 0x2D8/0x2E8/0x2EC offsets and the 0x10-byte block size, with a different register allocation; the repo
//   build target does not list this file yet.
#include "igi/retail/script/qvm_execute.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace {

// The three words 0x004EDD20 builds in its own frame (0x004EDF1D, 0x004EDF40, 0x004EDF44) and passes as the second argument:
// +0x00 the class block it allocated at 0x004EDF12, +0x04 the loader's third argument slot (the QVM argument frame, saved into EDI at
// 0x004EDD2F before the size callback reuses that slot), +0x08 the index of the next argument to read.
struct QTaskLoadContext
{
    void* block;
    void* argument_frame;
    std::uint32_t next_argument;
};

// Retail float constant at 0x005333B8 (0x45800000), applied by fmuls at 0x0042FEB5 and 0x0042FED1.
constexpr float argument_scale = 4096.0F;

} // namespace

extern "C" {

// 0x004B8A80: the QVM argument-block reader (project alias Sys_ResolveSymbol_23). Not transcribed in engine/ yet; the call site here ignores its
// return value.
char* NATIVE_DECL Sys_ResolveSymbol_23_4B8A80(void* argument_frame, std::int32_t index, void* destination,
                                              std::uint32_t count);

void NATIVE_DECL QTask_HandlerC_42FE80(std::uint32_t arg1, void* context)
{
    (void)arg1;                                                                                 // 0x0042FE80-0x0042FEE3: the first argument is never read
    auto* const load = static_cast<QTaskLoadContext*>(context);                                 // 0x0042FE82: mov 0x10(%esp),%esi
    auto* const block = static_cast<std::uint8_t*>(load->block);                                // 0x0042FE89: mov (%esi),%edi
    void* const frame = load->argument_frame;                                                   // 0x0042FE8E: mov 0x4(%esi),%ebx

    // 0x0042FE87-0x0042FEA0: a 0x10-byte argument block at block+0x2D8.
    std::uint32_t index = load->next_argument;                                                  // 0x0042FE8B: mov 0x8(%esi),%eax
    load->next_argument = index + 1U;                                                           // 0x0042FE9D: mov %ecx,0x8(%esi)
    Sys_ResolveSymbol_23_4B8A80(frame, static_cast<std::int32_t>(index), block + 0x2D8, 0x10U); // 0x0042FEA0: call 0x004B8A80

    // 0x0042FEA5-0x0042FEBB: a scaled float at block+0x2E8.
    index = load->next_argument;
    load->next_argument = index + 1U;
    *reinterpret_cast<float*>(block + 0x2E8) = static_cast<float>(
        Qvm_GetFloatArg(frame, static_cast<std::int32_t>(index)) * argument_scale);              // 0x0042FEB5 fmuls; 0x0042FEBB fstps 0x2e8(%edi)

    // 0x0042FEC1-0x0042FEDA: a scaled float at block+0x2EC.
    index = load->next_argument;
    load->next_argument = index + 1U;
    *reinterpret_cast<float*>(block + 0x2EC) = static_cast<float>(
        Qvm_GetFloatArg(frame, static_cast<std::int32_t>(index)) * argument_scale);              // 0x0042FED1 fmuls; 0x0042FEDA fstps 0x2ec(%edi)
}                                                                                               // 0x0042FEE0-0x0042FEE3: pops and ret

} // extern "C"

#undef NATIVE_DECL
