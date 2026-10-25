// Native: Task_DefineGlassShatter_449D30 (behavior-derived placeholder name from the byte-identical task-type name literal this body registers; retail symbol unknown); retail VA 0x00449D30-0x00449E42.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 275 bytes (0x113), 3 blocks, 3 edges,
//   69 instructions, counted with af + afbj over the exact image. The extent runs to the `ret` at 0x00449E42; the next function starts at
//   0x00449E50, which this body also links as a handler. NOTE: af's own size for this function is 30 bytes / 7 instructions, which stops at
//   the first block boundary; the listing walk to the ret is what closes the extent.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply and the objdump listing build/retail-dis.txt over the same extent. Blocks:
//   0x00449D30-0x00449D4D (7 instructions, falls through to the loop head), 0x00449D4E-0x00449DFA (41 instructions, back edge to 0x00449D4E at
//   0x00449DF5, forward exit to 0x00449DFB) and 0x00449DFB-0x00449E42 (21 instructions). Every push and cleanup was replayed: the prologue's one
//   argument dword closes with `add esp,4` at 0x00449D3E, the loop's three generator arguments with `add esp,0xC` at 0x00449D9A, the four compose
//   arguments with `add esp,0x10` at 0x00449DE4, and the tail's eleven with `add esp,0x2C` at 0x00449E3B - 4+12+16+44 = 76 = 19 pushes, and the
//   epilogue closes the 0x14 frame with `add esp,0x14` after `pop %esi`. Callees, each counted in the listing: 0x004B4720 once, 0x004B4770 three
//   times, 0x004B38E0 once, 0x004018F0 once, 0x00401900 once, 0x00401530 twice - nine call sites, no indirect call. Data read: the floats
//   0x005338C0 (image 0x42200000 = 40.0f), 0x005335B0 (image 0x41A00000 = 20.0f) and 0x005338BC (image 0x3A91A2B4 = 0.0011111111380159855f), the
//   name bytes at 0x0053C8FC (`px 16` gives 47 6c 61 73 73 53 68 61 74 74 65 72 00 = "GlassShatter"), and the loop bound 0x0057C730. Written:
//   0x0057C230 (three dwords), 0x0057C240 (a 16-bit word), and the 0x28-byte records from 0x0057C280 up to the bound.
// Behavior/CFG: no argument, no return value, no frame other than 0x14 bytes and the saved esi. It seeds the three-dword generator state at
//   0x0057C230 through 0x004B4720, then loops exactly thirty times - (0x0057C730-0x0057C280)/0x28 = 30 - advancing the destination pointer by
//   0x28 (ten dwords) per iteration while a stack counter goes 0, 1, ... 29. Each iteration draws three values from 0x004B4770 over that state,
//   scales each draw as `draw * 40.0f - 20.0f`, converts the counter to a float, forms three angles from those values and the counter times
//   0.0011111111380159855f, and calls 0x004B38E0 to compose a Z, a Y and an X rotation into the current record. After the loop it registers the
//   task type "GlassShatter" with object size 0x2B048 and the current type as parent, stores the returned id as a 16-bit word at 0x0057C240, and
//   links event 1 to 0x00449E50 and event 2 to 0x0044A1A0 for that id.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 69-instruction listing and afbj (275 B / 3 blocks / 3 edges); runtime UNOBSERVED.
//   Recorded limits: (a) retail computes the whole angle chain in 80-bit x87 registers and rounds only at each `fstps`, so the port computes in
//   long double and casts to float at every store; every stored value is re-loaded as a 32-bit float, exactly as `flds` does. (b) 0x004B4770
//   returns an exactly representable value (a 16-bit integer times 2^-16), so modelling its result as a double loses nothing. (c) The two zero
//   dwords retail pushes at 0x00449DFB/0x00449DFD are transcribed as the fourth argument of TaskType_Register and the argument of
//   QTask_GetCurrentType, which is what core/native_464260.cpp does for the same shape; 0x004018F0's own bytes are `mov ax,[0x54F928]; ret`,
//   so the argument it is pushed is inert. (d) The two linked handlers 0x00449E50 and 0x0044A1A0 are NOT transcribed yet, so this file declares
//   them and is not linkable until they exist; their bodies are a recorded gap. (e) What the 0x2B048-byte object or the thirty 0x28-byte records
//   mean is not established by this body beyond the rotation composition it performs on them.
#include <cstdint>

#include "igi/retail/core/task_types.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

extern "C" void Sys_StoreThreeWords_4B4720(void* target);                  // 0x004B4720: stores three immediate dwords at target+0x00/+0x04/+0x08 (sys/native_4b4720.cpp)
extern "C" double Sys_RandomNext_4B4770(void* state);                      // 0x004B4770: advances the three-dword state and returns the next scaled value (sys/native_4b4770.cpp)
extern "C" void Sys_ComposeRotation_4B38E0(float* out, float angle_x,
                                           float angle_y, float angle_z);  // 0x004B38E0: composes Z, Y and X rotations into a ten-dword matrix (sys/native_4b38e0.cpp)

// 0x00449E50 and 0x0044A1A0: the event 1 and event 2 handlers this body links. Neither is transcribed yet, so no body exists for them; the
// declarations are the placeholder form the handler work in this lane uses, and they make this file's dependency on them visible.
extern "C" void Task_Handler_449E50(void* context);                        // 0x00449E50: NOT TRANSCRIBED
extern "C" void Task_Handler_44A1A0(void* context);                        // 0x0044A1A0: NOT TRANSCRIBED

// 0x0057C230: the three-dword generator state that 0x004B4770 advances, seeded here through 0x004B4720. The address is past `.data`'s raw
// extent (raw ends 0x00552000), so the image value is zero fill.
std::uint32_t g_57c230[3] = {0U, 0U, 0U};

// 0x0057C240: the GlassShatter class id returned by TaskType_Register, stored and read back as a 16-bit word. Zero fill in the image.
std::uint16_t g_57c240 = 0U;

// 0x0057C280: the thirty ten-dword records this loop fills, 0x28 bytes each, ending at the loop bound 0x0057C730. Zero fill in the image.
float g_57c280[30 * 10] = {};

// The three floats this body reads - 0x005338C0 (image 0x42200000 = 40.0f), 0x005335B0 (0x41A00000 = 20.0f) and 0x005338BC (0x3A91A2B4 =
// 0.0011111111380159855f) - are written inline below with the same 32-bit values and their addresses in the comments, which is how the tree
// transcribes shared `.rdata` scalars (core/native_419860.cpp does the same for 0x005335A4). No new data symbol is invented for them.

void NATIVE_DECL Task_DefineGlassShatter_449D30()
{
    // 0x00449D30: sub $0x14,%esp (the frame, which holds the counter at frame+0x04); 0x00449D33: push %esi; 0x00449D34: push $0x57C230;
    // 0x00449D39: call 0x4B4720; 0x00449D3E: add $0x4,%esp.
    Sys_StoreThreeWords_4B4720(::g_57c230);

    // 0x00449D41: movl $0x0,0x8(%esp) - the loop counter, which is frame+0x04, starts at zero.
    std::int32_t counter = 0;

    // 0x00449D49: mov $0x57C280,%esi - the destination walks the thirty records.
    float* matrix = ::g_57c280;

    do
    {
        // 0x00449D4E: push $0x57C230; 0x00449D53: call 0x4B4770; 0x00449D58: fmuls 0x5338C0; 0x00449D5E: push $0x57C230;
        // 0x00449D63: fsubs 0x5335B0; 0x00449D69: fstps 0x1c(%esp).
        float const first = static_cast<float>(static_cast<long double>(Sys_RandomNext_4B4770(::g_57c230)) * static_cast<long double>(40.0F) - static_cast<long double>(20.0F));

        // 0x00449D6D: call 0x4B4770 (the argument pushed at 0x00449D5E); 0x00449D72: fmuls 0x5338C0; 0x00449D78: push $0x57C230;
        // 0x00449D7D: fsubs 0x5335B0; 0x00449D83: fstps 0x1c(%esp).
        float const second = static_cast<float>(static_cast<long double>(Sys_RandomNext_4B4770(::g_57c230)) * static_cast<long double>(40.0F) - static_cast<long double>(20.0F));

        // 0x00449D87: fildl 0x14(%esp) - the counter as a signed 32-bit integer; 0x00449D8B: fstps 0x10(%esp).
        float const index_value = static_cast<float>(counter);

        // 0x1.234568p-10L is the exact value of the 32-bit float at 0x005338BC (0x3A91A2B4); a decimal float literal is folded here with
        //   more precision than that float carries, which would round differently at the stores below.
        // 0x00449D8F: call 0x4B4770 (the argument pushed at 0x00449D78); 0x00449D94: fmuls 0x5338C0; 0x00449D9A: add $0xC,%esp;
        // 0x00449D9D: fsubs 0x5335B0; 0x00449DA3: fmuls 0x4(%esp) (the counter as a float); 0x00449DA7: fmuls 0x5338BC;
        // 0x00449DAD: fstps 0xC(%esp). This stored value is the fourth argument of the call below.
        float const third = static_cast<float>(((static_cast<long double>(Sys_RandomNext_4B4770(::g_57c230)) * static_cast<long double>(40.0F) - static_cast<long double>(20.0F))
                                                * static_cast<long double>(index_value)) * 0x1.234568p-10L);

        // 0x00449DB1: flds 0x4(%esp); 0x00449DB5: fmuls 0x10(%esp); 0x00449DBF: fmuls 0x5338BC; 0x00449DC5: fstps (%esp) - the third argument.
        float const angle_y = static_cast<float>((static_cast<long double>(index_value) * static_cast<long double>(second))
                                                 * 0x1.234568p-10L);

        // 0x00449DC8: flds 0xC(%esp); 0x00449DCC: fmuls 0x1c(%esp); 0x00449DD1: fmuls 0x5338BC; 0x00449DD7: fstps (%esp) - the second argument.
        float const angle_x = static_cast<float>((static_cast<long double>(third) * static_cast<long double>(first))
                                                 * 0x1.234568p-10L);

        // 0x00449DB9: mov 0xC(%esp),%eax (the bits of the third value); 0x00449DBD: push %eax - the fourth argument, pushed first because
        // cdecl passes right to left. 0x00449DBE/0x00449DD0: push %ecx twice, each slot overwritten by the two fstps above before the call.
        // 0x00449DDA: push %esi (the destination record); 0x00449DDB: call 0x4B38E0.
        Sys_ComposeRotation_4B38E0(matrix, angle_x, angle_y, third);

        // 0x00449DE0: mov 0x18(%esp),%edx (the counter); 0x00449DE4: add $0x10,%esp; 0x00449DE7: add $0x28,%esi; 0x00449DEA: inc %edx;
        // 0x00449DEB: cmp $0x57C730,%esi; 0x00449DF1: mov %edx,0x8(%esp); 0x00449DF5: jl 0x449D4E - the loop condition is the incremented
        // pointer against the bound, tested after the counter store.
        matrix += 10;
        ++counter;
    } while (matrix < ::g_57c280 + (30 * 10));

    // 0x00449DFB: push $0x0 (the inert argument of the parent getter, whose bytes are `mov ax,[0x54F928]; ret`); 0x00449DFD: push $0x0 (the
    // fourth argument of TaskType_Register); 0x00449DFF: call 0x4018F0; 0x00449E04: push %eax (the parent type); 0x00449E05: push $0x2B048 (the
    // object size); 0x00449E0A: push $0x53C8FC (the name bytes); 0x00449E0F: call 0x401900.
    const std::uint16_t glass_shatter_id = TaskType_Register("GlassShatter",        // 0x00449E0A: push 0x0053C8FC
                                                             0x2B048U,              // 0x00449E05: push 0x2B048
                                                             QTask_GetCurrentType(), // 0x00449DFF: call 0x4018F0
                                                             0U);                   // 0x00449DFD: push 0

    // 0x00449E14: push $0x449E50; 0x00449E19: push $0x1; 0x00449E1B: push %eax; 0x00449E1C: mov %ax,0x57C240 (the id store lands between the
    // pushes); 0x00449E22: call 0x401530.
    ::g_57c240 = glass_shatter_id;
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c240), // 0x00449E1C: mov %ax,[0x57C240]
                                           1U,                                     // 0x00449E19: push 1
                                           &Task_Handler_449E50);                  // 0x00449E14: push 0x00449E50

    // 0x00449E27: mov 0x57C240,%cx (the id is read back from the global, not reused from the register); 0x00449E2E: push $0x44A1A0;
    // 0x00449E33: push $0x2; 0x00449E35: push %ecx; 0x00449E36: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c240), // 0x00449E27: mov %cx,[0x57C240]
                                           2U,                                     // 0x00449E33: push 2
                                           &Task_Handler_44A1A0);                  // 0x00449E2E: push 0x0044A1A0

    // 0x00449E3B: add $0x2C,%esp; 0x00449E3E: pop %esi; 0x00449E3F: add $0x14,%esp; 0x00449E42: ret.
}

} // extern "C"

#undef NATIVE_DECL
