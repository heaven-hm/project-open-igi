// Native: Task_DefineElevator (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0043CC70-0x0043D00C.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 907 bytes, 5 basic blocks, 267 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 5 blocks: 0x0043CC70-0x0043CF95 (225 instrs, falls through
//   into the first loop), 0x0043CF96-0x0043CFD1 (21 instrs, jumps back to itself, exits to 0x0043CFD2), 0x0043CFD2-0x0043CFDD (2 instrs, falls
//   through into the second loop), 0x0043CFDE-0x0043D002 (14 instrs, jumps back to itself, exits to 0x0043D003) and 0x0043D003-0x0043D00C
//   (5 instrs, ends in ret) - plus the objdump listing build/retail-dis.txt for the same extent, instruction by instruction. `ps` at 0x0053C3EC
//   for the name literal (byte-identical to "Elevator") and at 0x0053C3DC for the first loop's format string (byte-identical to "Wanted floor %d");
//   `px 4` at 0x0057C108 (the id slot) and at every 0x0057BF90/0x0057C0Dx/0x0057C0Ex slot this body writes (all image value 0).
//   **The prologue was read before any `lea` was resolved: `sub esp,0x648` followed by `push ebx; push esi; push edi`, so three saved registers sit
//   BELOW the frame and every frame-relative address in this body is offset by twelve bytes.** That shift is applied throughout: `lea ebx,[esp+0x1D0]`
//   is frame+0x1C4, `lea esi,[esp+0x5E5]` is frame+0x5D9, and every append's `lea` was resolved with the twelve bytes included.
//   A full push-by-push simulation closes exactly: the registration's `add esp,0x38` is fourteen pushes (the five registration pushes plus the
//   first three links), the fourth to eighth links' `add esp,0xC` is three each, the container's `add esp,0x10` is its own count push plus the NINTH
//   link's three pushes (whose cleanup is folded into it), each of the twelve straight-line appends' `add esp,0x18` is six, and the epilogue's
//   `add esp,0x648` with `pop edi`/`pop esi`/`pop ebx` restores the frame and the three saved registers.
//   Every callee was swept against the tree's `// Native:` provenance index with no lane exemption and every one resolves to an existing file under
//   the same name, return type and linkage. 0x004A53B3 is the statically linked MSVC CRT `sprintf` (classified CRT in scripts/native_coverage.py),
//   so it is called as `::sprintf` here and declared nowhere, exactly as 0x0043AD80 does.
// Behavior/CFG: a registrar with two loops. It registers the "Elevator" task type of size 0x648 with parent Sys_GetGlobal16_4C48C0() -> id at
//   0x0057C108, stores the id, then links NINE handlers: e0->0x0043D010, e1->0x0043D280 and e2->0x0043DCF0 with the literal event ids 0, 1 and 2,
//   then 0x004F1A60->0x0043DDA0, 0x004F1A70->0x0043DDB0, 0x004F1210->0x0043DF20, 0x004F1220->0x0043E0A0, 0x004C5800->0x0043E1F0 and
//   0x00416890->0x0043E290.
//   It then pushes the container CAPACITY 0x19 (TWENTY-FIVE) and, with that push still outstanding, initialises six dword slots; then allocates the
//   container into 0x0057C0E0 and appends twelve records through 0x004F1A80 with recorded deltas 0xF0, 0x120, 0x170, 0x180, 0x18C, 0x18D, 0x188,
//   0x194, 0x198, 0x534, 0x4E4 and 0x584. **One of those deltas is ODD (0x18D)**, which is correct and not a transcription slip: the site's `lea`
//   displacement is 0x1A5 rather than the neighbouring site's 0x1A4, so the record points one byte further into the frame than its sibling.
//   FIRST LOOP (0x0043CF96-0x0043CFD0): a do-while over a fixed name table. `mov esi,0x0057BF90` sets a DATA pointer (not frame-relative),
//   `xor edi,edi` sets the sprintf argument, `lea ebx,[esp+0x1D0]` resolves to frame+0x1C4 for the record's third argument, and the back edge is
//   `add esi,0x20; inc edi; add ebx,0x50; cmp esi,0x57C0D0; jl 0x0043CF96`. Each iteration formats "Wanted floor %d" into a 0x20-byte slot of the
//   0x0057BF90 table and appends one record. The trip count is (0x57C0D0 - 0x57BF90) / 0x20 = 0x140 / 0x20 = TEN, and the record stride (0x50)
//   differs from the string stride (0x20) - the same two-rate induction as 0x0043AD80.
//   SECOND LOOP (0x0043CFDE-0x0043D001): `lea esi,[esp+0x5E5]` (frame+0x5D9, a frame-relative BYTE offset) and `mov edi,0x3`; the back edge is
//   `add esi,0x20; dec edi; jne`, so it runs exactly THREE times and the third argument is a frame offset rather than a delta computation.
//   Call census: 47 call sites over 21 distinct callees - 0x004F1A80 x14, 0x00401400 x5, 0x00401530 x4, 0x004F0FC0 x4, 0x004F0F50 x3,
//   0x004F1000 x2, and one each of 0x00401900, 0x00416890, 0x004A53B3, 0x004C48C0, 0x004C5800, 0x004F0F70, 0x004F0FA0, 0x004F0FD0, 0x004F0FE0,
//   0x004F1010, 0x004F1210, 0x004F1220, 0x004F1950, 0x004F1A60 and 0x004F1A70.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The three-number check is exact and independent here: capacity 0x19 = 25, and the records are
//   12 (straight-line) + 10 (first loop) + 3 (second loop) = 25.** The two trip counts come from a bound-and-stride ratio and a `dec/jne` counter,
//   neither of which is derived from the capacity, so the equality is a genuine cross-check rather than a restatement. Per the correction recorded
//   in native_43b850.cpp the allocation argument is a CAPACITY (`[container+4]`, with `[container]` a live count starting at zero), so the correct
//   general statement is `records <= capacity`; here the two meet exactly.
//   **The id slot 0x0057C108, the container slot 0x0057C0E0, the six initialised slots and the 0x140-byte name table at 0x0057BF90 are all in
//   writable `.data`** (the section table puts `.rdata` at VA 0x00533000 with virtual size 0x28F6, ending at 0x005358F6) and all have image
//   value 0, so every initialiser is 0 and every symbol class is `B`; this image has no `.bss` section, so a zero-image global is `.data` with a zero
//   initialiser and `B`/`D` is the symbol classification rather than the section. **Three of the six initialised slots are ADDRESS-TAKEN** - 0x57C0E8,
//   0x57C0D0 and 0x57C0D8 are each passed as an append's sixth argument - while 0x57C0EC, 0x57C0D4 and 0x57C0DC are written and never referenced by
//   address. The stored dwords include the exact bit patterns 0x4B189680, 0x461C4000 (10000.0f) and 0x3F800000 (1.0f) and the integer 0; they are
//   transcribed as raw dwords with no reinterpretation. **The 0x140-byte table at 0x0057BF90 is written only through the CRT sprintf call, never by
//   a direct store**, and its bound 0x0057C0D0 is the first byte past it - it is defined here as the object the first loop advances. The six stores
//   execute while the container's capacity push is outstanding, between the ninth link and the allocation call; the port keeps that order and passes
//   the capacity as the argument of the allocation call, which is where retail's value lands.
//   Arg6 is per-site: 0 at eleven appends and one of the three address-taken globals at the other three. Deferred pushes: at every getter-fed link
//   site the handler immediate is pushed before the event-id getter call and consumed as the link's third argument. Handler immediates 0x0043D010,
//   0x0043D280, 0x0043DCF0, 0x0043DDA0, 0x0043DDB0, 0x0043DF20, 0x0043E0A0, 0x0043E1F0 and 0x0043E290 are raw code immediates with no
//   implementation in the tree yet. Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail
//   that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) the
//   cleanups are reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38, 0xC
//   (five times), 0x10, 0x18 (fourteen times) and 0x648 are recorded so a future reviewer can check the arithmetic; (3) the loops' induction values
//   are kept as the same three locals retail uses because the string pointer and the record offset advance at different rates and collapsing them
//   would change one of the strides; (4) the port's `::sprintf` is the same CRT entry retail calls, so the formatted bytes come from the same routine;
//   (5) the second loop's third argument is kept as a frame BYTE offset (frame+0x5D9, stride 0x20) because retail pushes the computed pointer, and only
//   the difference from argument 2 reaches the record; the offset is never dereferenced.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>
#include <cstdio>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);            // 0x004C48C0: parent type getter (sys/native_4c48c0.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);            // 0x004F1210: `mov eax,[0x54DDA8]; ret` (image 255)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);            // 0x004F1220: `mov eax,[0x54DDAC]; ret` (image 255)
extern "C" std::uint8_t DynObj_GetByte_4C5800(void);              // 0x004C5800: argument-less byte getter (dynobj/native_4c5800.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416890(void);          // 0x00416890: argument-less byte getter (config/native_416890.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret` (image 0)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);            // 0x004F0F70: `mov eax,[0xA70C6C]; ret` (image 0)
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Core_GetRenderWordA_4F0FA0(void);        // 0x004F0FA0: `mov eax,[0xA70C7C]; ret` (image 0)
extern "C" std::uint32_t Camera_Get_4F1000(void);                 // 0x004F1000: `mov eax,[0xA70C90]; ret` (image 0)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);            // 0x004F0F50: `mov eax,[0xA71870]; ret` (image 0)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret` (image 0)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FE0(void);            // 0x004F0FE0: `mov eax,[0xA71888]; ret` (image 0)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0057C108: the "Elevator" task type id, in writable `.data`. Image value 0, so the initialiser is 0 and the symbol class is `B`.
// Only the low 16 bits are ever stored.
std::uint16_t g_57c108 = 0U;

// 0x0057C0E0: the 25-slot parameter container. Image value 0, so the initialiser is 0 and the symbol class is `B`.
std::uint32_t g_57c0e0 = 0U;

// 0x0057C0E8, 0x0057C0EC, 0x0057C0D0, 0x0057C0D4, 0x0057C0D8, 0x0057C0DC: the six slots this body initialises, in the order retail stores them.
// Image value 0 for all of them, so every initialiser is 0 and every symbol class is `B`. Three are ADDRESS-TAKEN as append arg6.
std::uint32_t g_57c0e8 = 0U; // address-taken (append delta 0x188)
std::uint32_t g_57c0ec = 0U; // written only
std::uint32_t g_57c0d0 = 0U; // address-taken (append delta 0x194); also the first byte past the name table
std::uint32_t g_57c0d4 = 0U; // written only
std::uint32_t g_57c0d8 = 0U; // address-taken (append delta 0x198)
std::uint32_t g_57c0dc = 0U; // written only

// 0x0057BF90-0x0057C0CF: the 0x140-byte name table the first loop formats into, ten 0x20-byte slots. Image value 0 throughout, so the initialiser is 0
// and the symbol class is `B`. It is written only through the CRT sprintf call, never by a direct store; the loop's bound 0x0057C0D0 is the first byte
// past it.
std::uint8_t g_57bf90[0x140] = {};

void Task_DefineElevator()
{
    // 0x0043CC70: sub esp,0x648 - a plain frame; this body has NO stack probe. 0x0043CC76/0x0043CC77/0x0043CC78: push ebx; push esi; push edi -
    // three callee-saved registers saved BELOW the frame, which is why every frame-relative address in this body is offset by twelve bytes.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0043CC79: push 0 (TaskType_Register's fourth argument, pushed first); 0x0043CC7B: push 0 (the parent getter's inert argument);
    // 0x0043CC7D: call 0x4C48C0; 0x0043CC82: push eax; 0x0043CC83: push 0x648; 0x0043CC88: push "Elevator"; 0x0043CC8D: call 0x401900.
    const std::uint16_t elevator_id = TaskType_Register("Elevator",                // 0x0043CC88: push 0x0053C3EC
                                                        0x648U,                    // 0x0043CC83: push 0x648 (object size, equal to the frame)
                                                        Sys_GetGlobal16_4C48C0(),  // 0x0043CC7D: call 0x4C48C0 (parent type)
                                                        0U);                      // 0x0043CC7B: push 0 (fourth argument)
    ::g_57c108 = elevator_id; // 0x0043CC9A: mov word [0x57C108],ax - between the pushes of the first link

    // 0x43CC92: push 0x43D010 (handler, deferred argument 3); 0x43CC97: push $0 (argument 2, literal event id);
    // 0x43CCA0: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c108), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43D010U)));

    // 0x43CCAB: push 0x43D280 (handler, deferred argument 3); 0x43CCB0: push $1 (argument 2, literal event id);
    // 0x43CCB3: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c108), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43D280U)));

    // 0x43CCBF: push 0x43DCF0 (handler, deferred argument 3); 0x43CCC4: push $2 (argument 2, literal event id);
    // 0x43CCC7: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c108), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43DCF0U)));

    // 0x43CCCF: push 0x43DDA0 (handler, deferred argument 3); the Script_PhaseIdGetE4_4F1A60 getter supplies argument 2;
    // 0x43CCE2: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c108),
        static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43DDA0U)));

    // 0x43CCEA: push 0x43DDB0 (handler, deferred argument 3); the Script_PhaseIdGetE5_4F1A70 getter supplies argument 2;
    // 0x43CCFC: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c108),
        static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43DDB0U)));

    // 0x43CD04: push 0x43DF20 (handler, deferred argument 3); the Sys_GetGlobal32_4F1210 getter supplies argument 2;
    // 0x43CD17: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c108),
        static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43DF20U)));

    // 0x43CD1F: push 0x43E0A0 (handler, deferred argument 3); the Sys_GetGlobal32_4F1220 getter supplies argument 2;
    // 0x43CD32: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c108),
        static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43E0A0U)));

    // 0x43CD3A: push 0x43E1F0 (handler, deferred argument 3); the DynObj_GetByte_4C5800 getter supplies argument 2;
    // 0x43CD4C: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c108),
        static_cast<std::uint32_t>(DynObj_GetByte_4C5800()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43E1F0U)));

    // 0x43CD54: push 0x43E290 (handler, deferred argument 3); the Sys_GetSettingByte_416890 getter supplies argument 2;
    // 0x43CD67: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c108),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416890()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43E290U)));


    // 0x0043CD6C: push 0x19 (the container CAPACITY - twenty-five slots: twelve below, ten in the first loop and three in the second). The stores
    // that follow execute WITH THIS PUSH OUTSTANDING; the allocation call then consumes it together with the ninth link's three pushes.
    // 0x0043CD6E-0x0043CDA0: the six dword stores, in retail order.
    // 0x0043CDAA: call 0x4F1950; 0x0043CDAF: add esp,0x10; 0x0043CDB2: mov [0x57C0E0],eax.
    ::g_57c0e8 = 0x0U;          // 0x0043CD6E
    ::g_57c0ec = 0x4B189680U;   // 0x0043CD78
    ::g_57c0d0 = 0x0U;          // 0x0043CD82
    ::g_57c0d4 = 0x461C4000U;   // 0x0043CD8C (10000.0f as bits)
    ::g_57c0d8 = 0x0U;          // 0x0043CD96
    ::g_57c0dc = 0x3F800000U;   // 0x0043CDA0 (1.0f as bits)
    ::g_57c0e0 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0x19)); // 0x0043CD6C: push 0x19

    // 0x43CDB7: push $0 (arg6 - a literal 0);
    // 0x43CDD5: call 0x004F1A80 (frame+0x00, frame+0xF0); argument 4 is the result of Sys_GetGlobal32_4F1010().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                append_frame, append_frame + 0xF0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x43CDDD: push $0 (arg6 - a literal 0);
    // 0x43CDFB: call 0x004F1A80 (frame+0x00, frame+0x120); argument 4 is the result of Sys_GetGlobal32_4F0F70().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                append_frame, append_frame + 0x120,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // 0x43CE03: push $0 (arg6 - a literal 0);
    // 0x43CE21: call 0x004F1A80 (frame+0x00, frame+0x170); argument 4 is the result of Core_GetRenderWordB_4F0FD0().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                append_frame, append_frame + 0x170,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x43CE29: push $0 (arg6 - a literal 0);
    // 0x43CE47: call 0x004F1A80 (frame+0x00, frame+0x180); argument 4 is the result of Core_GetRenderWordA_4F0FA0().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                append_frame, append_frame + 0x180,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // 0x43CE4F: push $0 (arg6 - a literal 0);
    // 0x43CE6D: call 0x004F1A80 (frame+0x00, frame+0x18C); argument 4 is the result of Camera_Get_4F1000().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                append_frame, append_frame + 0x18C,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x43CE75: push $0 (arg6 - a literal 0);
    // 0x43CE93: call 0x004F1A80 (frame+0x00, frame+0x18D); argument 4 is the result of Camera_Get_4F1000().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                append_frame, append_frame + 0x18D,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x43CE9B: push $57C0E8 (arg6 - the ADDRESS of a runtime slot, i.e. address-taken);
    // 0x43CEBC: call 0x004F1A80 (frame+0x00, frame+0x188); argument 4 is the result of Sys_GetGlobal32_4F0F50().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                append_frame, append_frame + 0x188,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57c0e8)));

    // 0x43CEC4: push $57C0D0 (arg6 - the ADDRESS of a runtime slot, i.e. address-taken);
    // 0x43CEE5: call 0x004F1A80 (frame+0x00, frame+0x194); argument 4 is the result of Sys_GetGlobal32_4F0F50().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                append_frame, append_frame + 0x194,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57c0d0)));

    // 0x43CEED: push $57C0D8 (arg6 - the ADDRESS of a runtime slot, i.e. address-taken);
    // 0x43CF0E: call 0x004F1A80 (frame+0x00, frame+0x198); argument 4 is the result of Sys_GetGlobal32_4F0F50().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                append_frame, append_frame + 0x198,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57c0d8)));

    // 0x43CF1C: push $0 (arg6 - a literal 0);
    // 0x43CF34: call 0x004F1A80 (frame+0x00, frame+0x534); argument 4 is the result of Sys_GetGlobal32_4F0FC0().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                append_frame, append_frame + 0x534,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x43CF42: push $0 (arg6 - a literal 0);
    // 0x43CF5A: call 0x004F1A80 (frame+0x00, frame+0x4E4); argument 4 is the result of Sys_GetGlobal32_4F0FC0().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                append_frame, append_frame + 0x4E4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x43CF68: push $0 (arg6 - a literal 0);
    // 0x43CF80: call 0x004F1A80 (frame+0x00, frame+0x584); argument 4 is the result of Sys_GetGlobal32_4F0FC0().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                append_frame, append_frame + 0x584,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);


    // 0x0043CF88: xor edi,edi (the sprintf argument); 0x0043CF8A: lea ebx,[esp+0x1D0] -> frame+0x1C4 (the running third argument; the twelve-byte
    // saved-register shift is included); 0x0043CF91: mov esi,0x0057BF90 (the fixed name table, NOT frame-relative).
    char* floor_name = reinterpret_cast<char*>(&::g_57bf90[0]); // retail esi (0x0043CF91: mov esi,0x57BF90)
    std::int32_t floor_index = 0;                               // retail edi (0x0043CF88: xor edi,edi)
    std::int32_t floor_record_delta = 0x1C4;                    // retail ebx (0x0043CF8A: lea ebx,[esp+0x1D0])

    // 0x0043CF96-0x0043CFD0: do-while. The back edge is `add esi,0x20; inc edi; add ebx,0x50; cmp esi,0x57C0D0; jl 0x0043CF96`, so the loop runs
    // while the name pointer stays below the first byte past the 0x140-byte table: (0x57C0D0 - 0x57BF90) / 0x20 = 0x140 / 0x20 = 10 iterations.
    do
    {
        // 0x0043CF96: push edi (argument 3, the format argument); 0x0043CF97: push 0x0053C3DC ("Wanted floor %d", argument 2);
        // 0x0043CF9C: push esi (argument 1, the destination); 0x0043CF9D: call 0x4A53B3 (the CRT sprintf); 0x0043CFA2: add esp,0xC.
        ::sprintf(floor_name, "Wanted floor %d", floor_index); // 0x0043CF9D: call 0x4A53B3

        // 0x0043CFA5: push 0 (arg6); 0x0043CFA7: push 0x7 (arg5); 0x0043CFA9: call 0x4F0FC0 (arg4); 0x0043CFAF: mov eax,[0x57C0E0] (arg1);
        // 0x0043CFB4: lea edx,[esp+0x18] -> frame+0x00 (arg2); 0x0043CFB8: push ebx (arg3); 0x0043CFBB: call 0x4F1A80; 0x0043CFC0: add esp,0x18.
        Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                    append_frame, append_frame + floor_record_delta,
                                    static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

        floor_name += 0x20;          // 0x0043CFC3: add esi,0x20 (the name-table stride)
        ++floor_index;               // 0x0043CFC6: inc edi
        floor_record_delta += 0x50;  // 0x0043CFC7: add ebx,0x50 (the record stride, deliberately different from the name stride)
    } while (floor_name < reinterpret_cast<char*>(&::g_57bf90[0]) + 0x140); // 0x0043CFCA: cmp esi,0x57C0D0; 0x0043CFD0: jl

    // 0x0043CFD2: lea esi,[esp+0x5E5] -> frame+0x5D9 (the twelve-byte shift included): a frame-relative BYTE offset, never dereferenced here.
    // 0x0043CFD9: mov edi,0x3 (the iteration counter).
    std::int32_t label_frame_offset = 0x5D9; // retail esi (0x0043CFD2)
    std::int32_t label_remaining = 3;        // retail edi (0x0043CFD9: mov edi,0x3)

    // 0x0043CFDE-0x0043D001: do-while with `dec edi; jne`, so exactly three iterations.
    do
    {
        // 0x0043CFDE: push 0 (arg6); 0x0043CFE0: push 0x7 (arg5); 0x0043CFE2: call 0x4F0FE0 (arg4); 0x0043CFE7: mov edx,[0x57C0E0] (arg1);
        // 0x0043CFEE: lea ecx,[esp+0x18] -> frame+0x00 (arg2); 0x0043CFF2: push esi (arg3); 0x0043CFF5: call 0x4F1A80; 0x0043CFFA: add esp,0x18.
        Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c0e0)),
                                    append_frame, append_frame + label_frame_offset,
                                    static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

        label_frame_offset += 0x20; // 0x0043CFFD: add esi,0x20
        --label_remaining;          // 0x0043D000: dec edi
    } while (label_remaining != 0); // 0x0043D001: jne

    // 0x0043D003: pop edi; 0x0043D004: pop esi; 0x0043D005: pop ebx; 0x0043D006: add esp,0x648; 0x0043D00C: ret - the frame and the three
    // saved registers are restored here; the port's locals take their place.
} // end of Task_DefineElevator

} // extern "C"
