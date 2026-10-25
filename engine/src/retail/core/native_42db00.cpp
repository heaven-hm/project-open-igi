// Native: QTask_HandlerA_42DB00; retail VA 0x0042DB00 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0042DB00-0x0042DCD0 (465 bytes, 5 blocks, 8 edges, 156 instructions).
// Tools/evidence: GNU objdump i386 disassembly build/retail-dis.txt (0x0042DB00-0x0042DCD0), with the byte span and instruction count re-derived from that
//   listing; the dispatch ABI, the two loader call sites and the sibling class callbacks are in docs/fidelity/evidence/qtask-handler-abi.md.
// Behavior/CFG: the parse callback of the class whose id word is 0x0053BDA8 ("Car"), linked at the private phase byte 0x0054DC30 by 0x0042DAC0. The loader
//   0x004EDD20 allocates the 0x2C0-byte class block at 0x004EDF12, then calls this handler with (0, &context) at 0x004EDF48, where context =
//   {block, QVM argument frame, next argument index} lives in the loader's frame. The handler first re-dispatches the handler table at (byte 0x0054DC30,
//   class word 0x0054E200 = the base class registered at 0x004F4190) with its own two argument words at 0x0042DB2F-0x0042DB36, then reads the Car
//   arguments out of the frame: a signed count at block+0x118; per count a 0x14-byte record at block+0x11C (byte, byte, float, dword, float*4096.0,
//   float); then float*4096.0 at block+0x25C, two bytes at block+0x260 and block+0x261, a 0x20-byte argument block at block+0x262, a signed count at
//   block+0x2AC, that many dwords at block+0x284, and a 0x10-byte argument block at block+0x2B0. Every read takes its index from context+0x8 and leaves
//   the cursor incremented before the reader call. 17 calls: 0x004EE0E0, 0x004F45F0, the table dispatch, 0x004B8A20 x8, 0x004B8A50 x4, 0x004B8A80 x2;
//   two fmuls by the float at 0x005333B8.
// Status/limits: TRANSCRIBED (static only; no build and no runtime). The table call is unconditional in retail (0x0042DB2F-0x0042DB36 has no null
//   test), so it is unconditional here. The first argument is only forwarded to that table call and is never inspected. The readers 0x004B8A20 and
//   0x004B8A80 are not transcribed in engine/ yet, so they are declared here under address-suffixed placeholders and must be unified when their own
//   natives land. The retail leaves the last callee's residue in EAX on return and no caller reads it, so this declaration returns void. The candidate
//   compiles for the x86 target as a single translation unit (i686-w64-mingw32-g++ 16.2.0, -std=c++20 -O2) and the compiled body keeps the retail
//   call order, the getter-driven table index, the unconditional table call, the 0x14-byte record walk, every store offset and the fmuls constant;
//   the register allocation differs and the repo build target does not list this file yet.
#include "igi/retail/core/task_types.hpp"
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

// Retail float constant at 0x005333B8 (0x45800000), applied by fmuls at 0x0042DBC2 and 0x0042DC0B.
constexpr float argument_scale = 4096.0F;

} // namespace

extern "C" {

// 0x004EE0E0 and 0x004F45F0: transcribed as engine/src/retail/sys/native_4ee0e0.cpp and native_4f45f0.cpp.
std::uint8_t NATIVE_DECL Sys_GetGlobal8_4EE0E0(void);
std::uint16_t NATIVE_DECL Sys_GetGlobal16_4F45F0(void);

// 0x004B8A20: the QVM integer-argument reader (project alias Script_GetArg / Script_EvalArgInt). Not transcribed in engine/ yet.
std::int32_t NATIVE_DECL Script_EvalArgInt_4B8A20(void* argument_frame, std::int32_t index);

// 0x004B8A80: the QVM argument-block reader (project alias Sys_ResolveSymbol_23). Not transcribed in engine/ yet; both call sites here
// ignore the return value.
char* NATIVE_DECL Sys_ResolveSymbol_23_4B8A80(void* argument_frame, std::int32_t index, void* destination,
                                              std::uint32_t count);

void NATIVE_DECL QTask_HandlerA_42DB00(std::uint32_t arg1, void* context)
{
    auto* const load = static_cast<QTaskLoadContext*>(context);                                // 0x0042DB03: mov 0x14(%esp),%esi
    auto* const block = static_cast<std::uint8_t*>(load->block);                               // 0x0042DB08: mov (%esi),%ebp
    void* const frame = load->argument_frame;                                                  // 0x0042DB0A: mov 0x4(%esi),%edi

    // 0x0042DB0D-0x0042DB36: dispatch table entry [(byte 0x0054DC30 & 0xFF) * 0x180 + (word 0x0054E200 & 0xFFFF)] with this handler's own
    // two argument words. The retail does not test the entry for null.
    std::size_t base_index = static_cast<std::size_t>(Sys_GetGlobal8_4EE0E0() & 0xFFU) *
                             igi::retail::core::task_phase_stride;                             // 0x0042DB12-0x0042DB1A
    base_index += Sys_GetGlobal16_4F45F0() & 0xFFFFU;                                          // 0x0042DB26-0x0042DB2B
    using TwoWordHandler = void (NATIVE_DECL*)(std::uint32_t, void*);
    reinterpret_cast<TwoWordHandler>(::QTask_HandlerTable[base_index])(arg1, context);          // 0x0042DB2F-0x0042DB36

    // 0x0042DB38-0x0042DB51: the wheel count, stored at block+0x118.
    std::uint32_t index = load->next_argument;                                                 // 0x0042DB38: mov 0x8(%esi),%eax
    load->next_argument = index + 1U;                                                          // 0x0042DB40: mov %edx,0x8(%esi)
    const std::int32_t wheel_count =
        Script_EvalArgInt_4B8A20(frame, static_cast<std::int32_t>(index));                     // 0x0042DB43: call 0x004B8A20
    *reinterpret_cast<std::int32_t*>(block + 0x118) = wheel_count;                             // 0x0042DB4B: mov %eax,0x118(%ebp)

    // 0x0042DB61-0x0042DBF5: one 0x14-byte record per wheel at block+0x11C; the retail walks EBX from block+0x11D in 0x14-byte steps.
    for (std::int32_t wheel_index = 0; wheel_index < wheel_count; ++wheel_index)               // 0x0042DB51-0x0042DB5B, 0x0042DBE5-0x0042DBF5
    {
        std::uint8_t* const wheel = block + 0x11C + static_cast<std::size_t>(wheel_index) * 0x14;
        index = load->next_argument;
        load->next_argument = index + 1U;
        wheel[0x00] = static_cast<std::uint8_t>(
            Script_EvalArgInt_4B8A20(frame, static_cast<std::int32_t>(index)));                // 0x0042DB77: mov %al,-0x1(%ebx)
        index = load->next_argument;
        load->next_argument = index + 1U;
        wheel[0x01] = static_cast<std::uint8_t>(
            Script_EvalArgInt_4B8A20(frame, static_cast<std::int32_t>(index)));                // 0x0042DB8A: mov %al,(%ebx)
        index = load->next_argument;
        load->next_argument = index + 1U;
        *reinterpret_cast<float*>(wheel + 0x04) = static_cast<float>(
            Qvm_GetFloatArg(frame, static_cast<std::int32_t>(index)));                         // 0x0042DB9C: fstps 0x3(%ebx)
        index = load->next_argument;
        load->next_argument = index + 1U;
        *reinterpret_cast<std::uint32_t*>(wheel + 0x08) = static_cast<std::uint32_t>(
            Script_EvalArgInt_4B8A20(frame, static_cast<std::int32_t>(index)));                // 0x0042DBAF: mov %eax,0x7(%ebx)
        index = load->next_argument;
        load->next_argument = index + 1U;
        *reinterpret_cast<float*>(wheel + 0x0C) = static_cast<float>(
            Qvm_GetFloatArg(frame, static_cast<std::int32_t>(index)) * argument_scale);        // 0x0042DBC2 fmuls; 0x0042DBC8 fstps 0xb(%ebx)
        index = load->next_argument;
        load->next_argument = index + 1U;
        *reinterpret_cast<float*>(wheel + 0x10) = static_cast<float>(
            Qvm_GetFloatArg(frame, static_cast<std::int32_t>(index)));                         // 0x0042DBE2: fstps 0xf(%ebx)
    }

    // 0x0042DBFB-0x0042DC11: scaled float at block+0x25C.
    index = load->next_argument;
    load->next_argument = index + 1U;
    *reinterpret_cast<float*>(block + 0x25C) = static_cast<float>(
        Qvm_GetFloatArg(frame, static_cast<std::int32_t>(index)) * argument_scale);             // 0x0042DC0B fmuls; 0x0042DC11 fstps

    // 0x0042DC17-0x0042DC27: one byte at block+0x260.
    index = load->next_argument;
    load->next_argument = index + 1U;
    block[0x260] = static_cast<std::uint8_t>(
        Script_EvalArgInt_4B8A20(frame, static_cast<std::int32_t>(index)));                    // 0x0042DC27: mov %al,0x260(%ebp)

    // 0x0042DC2D-0x0042DC3D: one byte at block+0x261.
    index = load->next_argument;
    load->next_argument = index + 1U;
    block[0x261] = static_cast<std::uint8_t>(
        Script_EvalArgInt_4B8A20(frame, static_cast<std::int32_t>(index)));                    // 0x0042DC3D: mov %al,0x261(%ebp)

    // 0x0042DC43-0x0042DC57: a 0x20-byte argument block at block+0x262.
    index = load->next_argument;
    load->next_argument = index + 1U;
    Sys_ResolveSymbol_23_4B8A80(frame, static_cast<std::int32_t>(index), block + 0x262, 0x20U); // 0x0042DC57: call 0x004B8A80

    // 0x0042DC5C-0x0042DC73: a signed count at block+0x2AC.
    index = load->next_argument;
    load->next_argument = index + 1U;
    const std::int32_t dword_count =
        Script_EvalArgInt_4B8A20(frame, static_cast<std::int32_t>(index));                     // 0x0042DC67: call 0x004B8A20
    *reinterpret_cast<std::int32_t*>(block + 0x2AC) = dword_count;                             // 0x0042DC73: mov %eax,0x2ac(%ebp)

    // 0x0042DC7B-0x0042DCAE: that many dwords at block+0x284.
    std::uint32_t* target = reinterpret_cast<std::uint32_t*>(block + 0x284);                    // 0x0042DC7B: lea 0x284(%ebp),%eax
    for (std::int32_t dword_index = 0; dword_index < dword_count; ++dword_index)               // 0x0042DC79, 0x0042DC9C-0x0042DCAE
    {
        index = load->next_argument;
        load->next_argument = index + 1U;
        *target = static_cast<std::uint32_t>(
            Script_EvalArgInt_4B8A20(frame, static_cast<std::int32_t>(index)));                // 0x0042DC9D: mov %eax,(%ecx)
        ++target;                                                                              // 0x0042DCA5: add $0x4,%ecx
    }

    // 0x0042DCB0-0x0042DCC4: a 0x10-byte argument block at block+0x2B0.
    index = load->next_argument;
    load->next_argument = index + 1U;
    Sys_ResolveSymbol_23_4B8A80(frame, static_cast<std::int32_t>(index), block + 0x2B0, 0x10U); // 0x0042DCC4: call 0x004B8A80
}                                                                                              // 0x0042DCCC-0x0042DCD0: pops and ret

} // extern "C"

#undef NATIVE_DECL
