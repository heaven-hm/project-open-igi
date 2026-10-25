// Native: Task_DefineHumanAI (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x0044C8D0-0x0044CC9B.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 918 bytes (0x396), 15 blocks,
//   235 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 15 blocks; the SEVEN backward branches at 0x0044CA77,
//   0x0044CAB4, 0x0044CAD3, 0x0044CB5B, 0x0044CBB3, 0x0044CC34 and 0x0044CC60 are the only edges that close a loop and each targets the
//   instruction after the block that sets its counter up, so all seven loops are do-while with no pre-test. The objdump listing
//   build/retail-dis.txt was read for the same extent. `ps` at 0x0053D404 (byte-identical to "HumanAI") and at every string the body loads.
//   `px 4` gives zero at 0x0053C920 and 0x0058A9D4 and zero for the nine dwords of the table at 0x0053C928, whose image values are pointers to
//   nine byte-identical script paths in `.data` (listed by the port's table below). A push-by-push simulation closes exactly; see the cleanup
//   reconciliation. Every callee was read from its own bytes before being trusted: 0x004A54A0 is the MSVC 6 stack probe (the C++ frame replaces
//   it, the same convention as core/native_4310f0.cpp); 0x004AF8F0 is Sys_InsertListTail_4AF8F0 (sys/native_4af8f0.cpp, transcribed); 0x004B80B0
//   is Qvm_Load (script/qvm_native_loader.cpp); 0x004F0F10 is `mov eax,[0xA70C8C]; ret`; 0x00450C80 is the lazy one-shot event-id getter on the
//   byte at 0x0053C922 and the sibling of the transcribed 0x00450CA0 (core/native_450ca0.cpp).
// Behavior/CFG: a 0x3ED0-byte frame allocated by the stack probe, with ebx held at zero for the whole body (every "store zero" is `mov %ebx,...`)
//   and ebp held at -1 from 0x0044CB0F. The body is three phases.
//   PHASE 1, registration and handlers: registers the "HumanAI" task type of size 0x3ED0 (equal to the frame) with parent
//   QTask_GetCurrentType() and the id at 0x0053C920; links NINE handlers - 0x44F060/e0 and 0x44F410/e1 and 0x44FFC0/e2 through the literal
//   events 0, 1 and 2, then 0x450110 (event from 0x00450C80), 0x4505C0 (from 0x004F1A60), 0x4505D0 (from 0x004F1A70), 0x450770 (from 0x004F1210)
//   and 0x4507D0 (from 0x004F1220), and finally a link with a NULL handler and the literal event 4; allocates a container of capacity 3 into
//   0x0058A9D4 and appends THREE records whose recorded frame deltas are 0x3D15, 0x3B68 and 0x3B70.
//   PHASE 2, two untranscribed setup calls that take no argument and whose results are ignored (0x0044CCA0 and 0x0044EEB0).
//   PHASE 3, the seven do-while loops and the nine empty-list heads, in retail order: the head at 0x0058A9D8, the byte fill of the 8 records at
//   0x0057D840 (stride 0x130), the head at 0x0058A9C8, the 1280 records at 0x0057E1C8 (stride 0x28) linked onto it, the 9-entry script load
//   through 0x004B80B0 from the table at 0x0053C928 into 0x0058A9F4, the heads at 0x0057D768 and 0x0058A9E8, the 128 records at 0x0058AA28
//   (stride 0x660) linked onto 0x0058A9E8 with a `rep stos` dword clear of 0x190 dwords at record+0x20 and five field stores, the heads at
//   0x005BDA28 and 0x0057D758, the 16 records at 0x0057D778 (stride 0xC) linked onto 0x0057D758, the heads at 0x0058AA18, 0x0057C748 and
//   0x0057E1B8, the 64 records at 0x0057C758 (stride 0x18) linked onto 0x0057C748 with four further field stores, the 128 records at 0x0057CD58
//   (stride 0x14) linked onto 0x0057E1B8 with three field stores, and finally eight dword clears at 0x005BDA38-0x005BDA54.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Census: 0x00401400 x5, 0x00401530 x4 (9 links), 0x004018F0 x1, 0x00401900 x1, 0x0044CCA0 x1,
//   0x0044EEB0 x1, 0x00450C80 x1, 0x004A54A0 x1 (stack probe, see the shape difference below), 0x004AF8F0 x5, 0x004B80B0 x1, 0x004F0F10 x1,
//   0x004F0FA0 x1, 0x004F0FE0 x1, 0x004F1210 x1, 0x004F1220 x1, 0x004F1950 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x3 = 32 sites /
//   19 distinct in retail. The port's compiled object carries 31 sites / 19 distinct: EIGHTEEN of the nineteen retail targets appear with EXACTLY
//   the retail count and zero mismatches (0x401400 5/5, 0x401530 4/4, 0x4018F0 1/1, 0x401900 1/1, 0x44CCA0 1/1, 0x44EEB0 1/1, 0x450C80 1/1,
//   0x4AF8F0 5/5, 0x4B80B0 1/1, 0x4F0F10 1/1, 0x4F0FA0 1/1, 0x4F0FE0 1/1, 0x4F1210 1/1, 0x4F1220 1/1, 0x4F1950 1/1, 0x4F1A60 1/1,
//   0x4F1A70 1/1, 0x4F1A80 3/3); the nineteenth, 0x4A54A0, is absent because the C++ frame replaces the probe, and the port carries ONE extra call
//   the retail does not have, `_memset`, which the compiler emits for the `rep stos` loop (see shape difference 9).
//   Cleanup reconciliation, each `add esp` matched to the pushes it removes: `add esp,0x38` at 0x0044C92F = the five registration pushes plus
//   links 1-3 (fourteen dwords); `add esp,0xC` at 0x0044C94A/0x0044C964/0x0044C97F/0x0044C99A = links 4, 5, 6 and 7; link 8 has NO cleanup of
//   its own - its three pushes fold forward together with link 9's three and the capacity push into `add esp,0x1C` at 0x0044C9CB (seven
//   dwords); three `add esp,0x18` at 0x0044C9F5, 0x0044CA1A and 0x0044CA3F = appends 1, 2 and 3; `add esp,0x8` inside every one of the seven
//   loops (each removes that iteration's two argument pushes); and `add esp,0x3ED0` at 0x0044CC95 = the frame itself. Totals: 5 + 27 (nine
//   links, three dwords each) + 1 (capacity) + 18 (three appends, six dwords each) + 14 (seven loops, two dwords each) = 65 pushes = 260 bytes
//   against 0x38 + 4*0xC + 0x1C + 3*0x18 + 7*0x8 = 56 + 48 + 28 + 72 + 56 = 260 bytes; the saved ebx/ebp/esi/edi pushes are balanced by their
//   pops and the probe's own esp drop is released by the final `add esp,0x3ED0`, so `esp` is 0 at the ret.
//   **TaskType_Register ARITY, again from the callee's own bytes: retail pushes FIVE dwords here and the callee reads FOUR.** The fifth is
//   pushed and cleaned and never read - established in native_449d30.cpp from 0x00401900's own body and applied identically here.
//   **NO HASH REGISTRATION: this body never calls 0x004B8810. That is the FOURTH instance in this lane** (after 0x00449360 Siren,
//   0x00449D30 GlassShatter and 0x0044C130 AmbientArea). It is not that this body has no names to register: the untranscribed 0x0044EEB0 it
//   calls immediately afterwards registers nine "AIEVENT_*" strings through 0x004B8810 (values 0..8 at 0x0053DD5C down to 0x0053DCE0) and
//   0x0044CCA0 registers five "AIAction_*" handler tables through 0x004B8890. So the reason the four bodies skip the hash is visible here: their
//   names are registered by a separate body they call, not by the registrar itself. That is the answer to the question recorded in
//   native_449d30.cpp and it is evidence, not inference - the call target's stores are read from its own bytes.
//   Registered size 0x3ED0 equals the frame 0x3ED0 (the append-shape case: this body has no stack template to append, its three appends carry
//   parameter blocks only, but the frame is still the class's object size).
//   Documented shape differences, all value-preserving: (1) the stack probe `mov eax,0x3ED0; call 0x004A54A0` is not reproduced; the C++ frame
//   replaces it, following core/native_4310f0.cpp and core/native_46e370.cpp; (2) the
//   0x3ED0-byte frame is reproduced as a single local slot whose address stands in for the frame base - the only consumers of the base are the
//   three appends, and 0x004F1A80 reads arg2 and arg3 ONLY as `arg3 - arg2` (script/native_4f1a80.cpp: `sub ecx,edx`), so no byte of the frame is
//   ever dereferenced through those arguments; (3) the retail loop counters are byte offsets into absolute addresses and the port walks pointers
//   through the objects that now hold those bytes, with each loop bound taken from the object's own extent; the retail bound literals are recorded
//   at each site; (4) retail compares loop pointers with a signed `jl` and the port compares pointers, which is the same order for these addresses;
//   (5) `rep stos` is reproduced as an explicit dword-store loop of 0x190 stores, which is what one `rep stos` iteration is; (6) argument-less
//   getters ignore the arguments retail pushes at their call sites, so those inert pushes are not reproduced; (7) byte-valued getters are
//   zero-extended in the port while retail leaves the upper bits of eax undefined - inert because every link here masks the event word; (8) the
//   nine script-path dwords of the retail table at 0x0053C928 are absolute addresses of strings in the retail `.data`, which the port cannot use,
//   so the port's table holds its own pointers to byte-identical literals - the strings Qvm_Load receives are the same bytes in the same order;
//   (9) the compiler recognises the `rep stos` dword-store loop as `memset(record + 0x20, 0, 0x640)` at -O2 and emits that call, so the port's
//   object carries one `_memset` call site the retail does not have as a call (retail's `rep stos` is the same 0x640 bytes of zero). This is the
//   same class of compiler-emitted replacement as the stack probe in (1) and it is subtracted from the port's census the same way: the retail
//   target 0x004A54A0 disappears and `_memset` appears. The return value is unused in retail (eax is dead after the `rep stos`) and dead here.
//   Cross-references: 0x00450C80 is the sibling of the transcribed 0x00450CA0 and is UNTRANSCRIBED; 0x004F0F10 is a six-byte `mov eax,[...]`
//   getter and is UNTRANSCRIBED (it is on the second list); 0x0044CCA0, 0x0044EEB0 and every handler immediate (0x0044F060, 0x0044F410,
//   0x0044FFC0, 0x00450110, 0x004505C0, 0x004505D0, 0x00450770, 0x004507D0) are UNTRANSCRIBED.
//   Unestablished: what the two setup calls 0x0044CCA0/0x0044EEB0 do beyond the registration sequences read from their first instructions;
//   what 0x0044CCA0's 0x004B8890 arguments mean; the record layouts behind the nine regions beyond the fields this body writes; and what consumes
//   the nine loaded script objects at 0x0058A9F4.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// 0x004B80B0 is declared with C++ linkage in script/qvm_native_loader.hpp (`void* Qvm_Load(const char* qvm_path)`), so it is declared here
// outside the extern "C" block to keep the same linkage.
void* Qvm_Load(char const* qvm_path);

extern "C" {

// Callees with a transcribed file, each with that file's return type and linkage. 0x00401400, 0x00401530, 0x004018F0 and 0x00401900 come from
// igi/retail/core/task_types.hpp (core/task_types.cpp) and are not redeclared here.
void* Sys_InsertListTail_4AF8F0(void* head, void* node);              // 0x004AF8F0 (sys/native_4af8f0.cpp): links node onto head, returns head
void* Script_TaskContainer_Alloc(std::int32_t count);                 // 0x004F1950 (script/native_4f1950.cpp)
extern std::uint32_t Core_GetRenderWordA_4F0FA0();                    // 0x004F0FA0 (core/native_4f0fa0.cpp)
extern std::uint32_t Sys_GetGlobal32_4F0FE0();                        // 0x004F0FE0 (sys/native_4f0fe0.cpp)
extern std::uint32_t Sys_GetGlobal32_4F1210();                        // 0x004F1210 (sys/native_4f1210.cpp)
extern std::uint32_t Sys_GetGlobal32_4F1220();                        // 0x004F1220 (sys/native_4f1220.cpp)
extern std::uint8_t Script_PhaseIdGetE4_4F1A60();                     // 0x004F1A60 (script/script_state.hpp)
extern std::uint8_t Script_PhaseIdGetE5_4F1A70();                     // 0x004F1A70 (script/script_state.hpp)
// 0x004F1A80 (script/native_4f1a80.cpp): arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08); arg2 and arg3 are read
// only as `arg3 - arg2`, the record's parameter-block size.
void* Script_TaskContainer_Append(std::int32_t* container, std::int32_t arg2, std::int32_t arg3,
                                  std::int32_t arg4, std::int32_t arg5, std::int32_t arg6);

// UNTRANSCRIBED dependencies, declared from their own bytes and from this call site; the names are provisional call-site placeholders that must
// be reconciled when those bodies land.
//   0x00450C80: lazy one-shot event-id getter on the byte at 0x0053C922 (`mov 0x53C922,%al; cmp $0xFF,%al; jne ret; push 0; call 0x4017C0;
//   mov %al,0x53C922; ret`) - the sibling of the transcribed 0x00450CA0, same 25-byte shape.
extern std::uint8_t Task_GetCachedEventId_450C80(void);
//   0x004F0F10: `mov eax,[0xA70C8C]; ret`, a six-byte dword getter.
extern std::uint32_t Sys_GetGlobal32_4F0F10(void);
//   0x0044CCA0: registers handler tables through 0x004B8890 (its first four calls pass "AIAction_Patrol"/"AIAction_Combat"/"AIAction_Dead"/
//   "AIAction_FallFlat" with handler immediates and small counts) and takes no argument.
extern void Task_RegisterAIActions_44CCA0(void);
//   0x0044EEB0: registers hash names through 0x004B8810 ("AIEVENT_CREATE" with value 0 and so on) and takes no argument.
extern void Task_RegisterAIEvents_44EEB0(void);

// 0x0053C920: the "HumanAI" task type id (word). Image value 0 -> initialiser 0, symbol class `B`, writable `.data`: `.rdata` ends at
// 0x005358F6 and `.data` starts at 0x00536000, and this image has no `.bss`, so a zero-image global is `.data` with a zero initialiser.
std::uint16_t g_53c920 = 0U;

// 0x0053C928: the nine script paths this body loads through Qvm_Load. The retail image holds nine absolute pointers to strings in `.data`; the
// port holds its own pointers to byte-identical literals, in the retail order and with the retail text. The array is left non-const so it lands
// in writable `.data` as the retail's does.
char const* g_53c928[9] = {
    "LOCAL:common/ai/rpg.qsc",      // 0x0053D3EC, loaded first
    "LOCAL:common/ai/gunner.qsc",   // 0x0053D3D0
    "LOCAL:common/ai/sniper.qsc",   // 0x0053D3B4
    "LOCAL:common/ai/anya.qsc",     // 0x0053D398
    "LOCAL:common/ai/ekk.qsc",      // 0x0053D380
    "LOCAL:common/ai/priboi.qsc",   // 0x0053D364
    "LOCAL:common/ai/civilian.qsc", // 0x0053D344
    "LOCAL:common/ai/patrol.qsc",   // 0x0053D328
    "LOCAL:common/ai/guard.qsc",    // 0x0053D30C
};

// 0x0058A9D4: the three-record parameter container.
std::uint32_t g_58a9d4 = 0U;

// 0x0058A9F4: the nine pointers Qvm_Load returns, stored in load order. Image value 0 -> `B`.
std::uint32_t g_58a9f4[9] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};

// The nine list heads the body initialises and later links records onto, plus the eight-dword block at 0x005BDA38 it clears. Every head is the
// three-dword layout 0x004AF8F0 uses: +0x00 forward link, +0x04 the head's own link field, +0x08 tail. The initialiser writes +0x00 = head+0x04,
// +0x08 = head, +0x04 = 0, which is the empty-list state. Image value 0 -> `B` throughout.
std::uint32_t g_58a9d8[3] = {0U, 0U, 0U}; // 0x0058A9D8
std::uint32_t g_58a9c8[3] = {0U, 0U, 0U}; // 0x0058A9C8
std::uint32_t g_57d768[3] = {0U, 0U, 0U}; // 0x0057D768
std::uint32_t g_58a9e8[3] = {0U, 0U, 0U}; // 0x0058A9E8
std::uint32_t g_5bda28[3] = {0U, 0U, 0U}; // 0x005BDA28
std::uint32_t g_57d758[3] = {0U, 0U, 0U}; // 0x0057D758
std::uint32_t g_58aa18[3] = {0U, 0U, 0U}; // 0x0058AA18
std::uint32_t g_57c748[3] = {0U, 0U, 0U}; // 0x0057C748
std::uint32_t g_57e1b8[3] = {0U, 0U, 0U}; // 0x0057E1B8
std::uint32_t g_5bda38[8] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U}; // 0x005BDA38..0x005BDA54

// The record regions this body initialises, each sized by the difference between the retail loop bounds that walk it. The nodes are written as
// dwords, so each region is 4-aligned. Image value 0 -> `B` throughout.
alignas(4) std::uint8_t g_57d840[0x980];  // 0x0057D840..0x0057E1C0: 8 records of 0x130, only the first byte of each is cleared here
alignas(4) std::uint8_t g_57e1c8[0xC800]; // 0x0057E1C8..0x0058A9C8: 1280 records of 0x28
alignas(4) std::uint8_t g_58aa28[0x33000]; // 0x0058AA28..0x005BDA28: 128 records of 0x660
alignas(4) std::uint8_t g_57d778[0xC0];   // 0x0057D778..0x0057D838: 16 records of 0xC
alignas(4) std::uint8_t g_57c758[0x600];  // 0x0057C758..0x0057CD58: 64 records of 0x18
alignas(4) std::uint8_t g_57cd58[0xA00];  // 0x0057CD58..0x0057D758: 128 records of 0x14

void Task_DefineHumanAI()
{
    // 0x0044C8D0: mov eax,0x3ED0; 0x0044C8D5: call 0x004A54A0 - the MSVC 6 stack probe, which lowers esp by 0x3ED0 itself, so the frame base is
    // set inside the probe and there is no `sub esp` here. The C++ frame replaces it (the port's census omits that call). The 0x3ED0 bytes are
    // also the registered object size. The single local stands in for the frame base: the only consumers of the base are the three appends, and
    // 0x004F1A80 reads its second and third arguments only as their difference.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 - argument 2 of every append in this body
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0044C8DF: push %ebx; 0x0044C8E0: push %ebx - the task-list argument and a fifth dword the callee never reads (see the provenance
    // block); 0x0044C8E1: call 0x004018F0 (QTask_GetCurrentType, no argument); 0x0044C8E6: push %eax; 0x0044C8E7: push 0x3ED0;
    // 0x0044C8EC: push 0x0053D404; 0x0044C8F1: call 0x00401900.
    const std::uint16_t human_ai_id = TaskType_Register("HumanAI",             // 0x0044C8EC: push 0x0053D404
                                                        0x3ED0U,               // 0x0044C8E7: push 0x3ED0 (object size, equal to the frame)
                                                        QTask_GetCurrentType(), // 0x0044C8E1: call 0x004018F0 (parent = current task type)
                                                        0U);                   // 0x0044C8E0: push 0
    ::g_53c920 = human_ai_id; // 0x0044C8FD: mov word [0x53C920],ax

    // 0x0044C8F6: push 0x0044F060 (handler, argument 3); 0x0044C8FB: push %ebx (argument 2 - ebx is the body's zero); 0x0044C8FC: push %eax
    // (argument 1, still the id TaskType_Register returned); 0x0044C903: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(human_ai_id), 0U,
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0044F060U)));
    // 0x0044C90E: push 0x0044F410 (handler); 0x0044C913: push 0x1 (literal event id); 0x0044C915: push %eax; 0x0044C916: call 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(human_ai_id), 0x1U,
                                           reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0044F410U)));
    // 0x0044C91B: mov 0x53C920,%cx (the id RELOADED from the global for every link from here on); 0x0044C922: push 0x0044FFC0 (handler);
    // 0x0044C927: push 0x2 (literal event id); 0x0044C929: push %ecx; 0x0044C92A: call 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c920), 0x2U,
                                           reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0044FFC0U)));
    // 0x0044C92F: add esp,0x38 - the five registration pushes plus links 1-3.

    // 0x0044C932: push 0x00450110 (handler, deferred across the getter as argument 3); 0x0044C937: call 0x00450C80 (argument 2, the event id);
    // 0x0044C93C: mov 0x53C920,%dx; 0x0044C943: push %eax; 0x0044C944: push %edx; 0x0044C945: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c920),
                                static_cast<std::uint32_t>(Task_GetCachedEventId_450C80()),
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00450110U)));
    // 0x0044C94A: add esp,0xC.
    // 0x0044C94D: push 0x004505C0 (handler); 0x0044C952: call 0x004F1A60 (event id); 0x0044C957: push %eax; 0x0044C958: mov 0x53C920,%ax;
    // 0x0044C95E: push %eax; 0x0044C95F: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c920),
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004505C0U)));
    // 0x0044C964: add esp,0xC.
    // 0x0044C967: push 0x004505D0 (handler); 0x0044C96C: call 0x004F1A70 (event id); 0x0044C971: mov 0x53C920,%cx; 0x0044C978: push %eax;
    // 0x0044C979: push %ecx; 0x0044C97A: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c920),
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004505D0U)));
    // 0x0044C97F: add esp,0xC.
    // 0x0044C982: push 0x00450770 (handler); 0x0044C987: call 0x004F1210 (event id); 0x0044C98C: mov 0x53C920,%dx; 0x0044C993: push %eax;
    // 0x0044C994: push %edx; 0x0044C995: call 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c920),
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()),
                                           reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00450770U)));
    // 0x0044C99A: add esp,0xC.
    // 0x0044C99D: push 0x004507D0 (handler); 0x0044C9A2: call 0x004F1220 (event id); 0x0044C9A7: push %eax; 0x0044C9A8: mov 0x53C920,%ax;
    // 0x0044C9AE: push %eax; 0x0044C9AF: call 0x00401530. This link has NO cleanup of its own: its three pushes fold forward with link 9's
    // three and the capacity push into `add esp,0x1C` at 0x0044C9CB.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c920),
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()),
                                           reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004507D0U)));
    // 0x0044C9B4: mov 0x53C920,%cx; 0x0044C9BB: push %ebx (argument 3 - the handler slot is NULL, ebx is zero); 0x0044C9BC: push 0x4
    // (argument 2, the literal event id); 0x0044C9BE: push %ecx; 0x0044C9BF: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c920), 0x4U,
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0U)));
    // 0x0044C9C4: push 0x3 (container CAPACITY, three records - the same literal as the number of appends); 0x0044C9C6: call 0x004F1950;
    // 0x0044C9CB: add esp,0x1C (link 8's three, link 9's three and this capacity push); 0x0044C9CE: mov [0x58A9D4],eax.
    ::g_58a9d4 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(3)));

    // 0x0044C9D3: push %ebx (argument 6, zero); 0x0044C9D4: push 0x7 (argument 5); 0x0044C9D6: call 0x004F0FE0 (argument 4);
    // 0x0044C9E1: push %eax; 0x0044C9E2: lea 0x3D2D(%esp) -> frame+0x3D15 (argument 3); 0x0044C9E9: lea 0x18(%esp) -> frame+0x00
    // (argument 2); 0x0044C9ED/0x0044C9EE/0x0044C9EF: push all three; 0x0044C9F0: call 0x004F1A80; 0x0044C9F5: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_58a9d4)),
                                append_frame, append_frame + 0x3D15,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);
    // 0x0044C9F8: push %ebx; 0x0044C9F9: push 0x7; 0x0044C9FB: call 0x004F0FA0; 0x0044CA06: push %eax;
    // 0x0044CA07: lea 0x3B80(%esp) -> frame+0x3B68; 0x0044CA0E: lea 0x18(%esp) -> frame+0x00; 0x0044CA15: call 0x004F1A80;
    // 0x0044CA1A: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_58a9d4)),
                                append_frame, append_frame + 0x3B68,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
    // 0x0044CA1D: push %ebx; 0x0044CA1E: push 0x2 (argument 5 is TWO here, not 7); 0x0044CA20: call 0x004F0F10;
    // 0x0044CA2B: push %eax; 0x0044CA2C: lea 0x3B88(%esp) -> frame+0x3B70; 0x0044CA33: lea 0x18(%esp) -> frame+0x00;
    // 0x0044CA3A: call 0x004F1A80; 0x0044CA3F: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_58a9d4)),
                                append_frame, append_frame + 0x3B70,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F10()), 2, 0);

    // 0x0044CA42: call 0x0044CCA0; 0x0044CA47: call 0x0044EEB0. Neither takes an argument and neither result is used.
    Task_RegisterAIActions_44CCA0();
    Task_RegisterAIEvents_44EEB0();

    // 0x0044CA4C: movl $0x58A9DC,0x58A9D8; 0x0044CA56: movl $0x58A9D8,0x58A9E0; 0x0044CA60: mov %ebx,0x58A9DC - empty-list state of the head at
    // 0x0058A9D8: forward link = head+0x04, tail = head, head's link field = 0.
    ::g_58a9d8[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_58a9d8[1]));
    ::g_58a9d8[2] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_58a9d8[0]));
    ::g_58a9d8[1] = 0U;

    // 0x0044CA66: mov $0x57D840,%eax; 0x0044CA6B: mov %bl,(%eax); 0x0044CA6D: add $0x130,%eax; 0x0044CA72: cmp $0x57E1C0,%eax;
    // 0x0044CA77: jl 0x0044CA6B. Do-while: only the first byte of each of the 8 records is cleared.
    {
        std::uint8_t* fill = &::g_57d840[0];
        do {
            *fill = 0U;                         // 0x0044CA6B
            fill = fill + 0x130;               // 0x0044CA6D
        } while (fill < &::g_57d840[0x980]);    // 0x0044CA72/0x0044CA77: retail bound 0x0057E1C0
    }

    // 0x0044CA79: movl $0x58A9CC,0x58A9C8; 0x0044CA83: movl $0x58A9C8,0x58A9D0; 0x0044CA8D: mov %ebx,0x58A9CC - empty list at 0x0058A9C8.
    ::g_58a9c8[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_58a9c8[1]));
    ::g_58a9c8[2] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_58a9c8[0]));
    ::g_58a9c8[1] = 0U;

    // 0x0044CA93: mov $0x57E1C8,%esi; 0x0044CA98: push %esi; 0x0044CA99: mov %ebx,0x4(%esi); 0x0044CA9C: push $0x58A9C8;
    // 0x0044CAA1: mov %ebx,(%esi); 0x0044CAA3: call 0x004AF8F0; 0x0044CAA8: add $0x28,%esi; 0x0044CAAE: cmp $0x58A9C8,%esi;
    // 0x0044CAB4: jl 0x0044CA98. Do-while over 1280 records of 0x28, linked onto the head at 0x0058A9C8. The record's +0x04 is cleared
    // before its +0x00, in retail order.
    {
        std::uint8_t* node = &::g_57e1c8[0];
        do {
            *reinterpret_cast<std::uint32_t*>(node + 0x04U) = 0U;              // 0x0044CA99
            *reinterpret_cast<std::uint32_t*>(node) = 0U;                      // 0x0044CAA1
            Sys_InsertListTail_4AF8F0(&::g_58a9c8[0], node);                   // 0x0044CAA3
            node = node + 0x28;                                                // 0x0044CAA8
        } while (node < &::g_57e1c8[0xC800]);                                  // 0x0044CAAE/0x0044CAB4: retail bound 0x0058A9C8
    }

    // 0x0044CAB6: xor %esi,%esi; 0x0044CAB8: mov 0x53C928(%esi),%edx; 0x0044CABE: push %edx; 0x0044CABF: call 0x004B80B0;
    // 0x0044CAC4: mov %eax,0x58A9F4(%esi); 0x0044CACA: add $0x4,%esi; 0x0044CAD0: cmp $0x24,%esi; 0x0044CAD3: jl 0x0044CAB8.
    // Do-while over the NINE table entries - one byte offset (%esi) indexes BOTH tables in retail, so the port walks two pointers with one
    // stride.
    {
        char const* const* path = &::g_53c928[0];
        std::uint32_t* loaded = &::g_58a9f4[0];
        do {
            *loaded = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Qvm_Load(*path))); // 0x0044CAB8/0x0044CABF/0x0044CAC4
            path = path + 1;                     // 0x0044CACA: add $0x4,%esi (the table stride)
            loaded = loaded + 1;                 // (the same offset into the second table)
        } while (loaded < &::g_58a9f4[9]);       // 0x0044CAD0/0x0044CAD3: retail bound 0x24 bytes
    }

    // 0x0044CAD5: push %edi - retail saves edi here because the `rep stos` below needs it; the C++ compiler keeps its own registers.
    // 0x0044CAD6: movl $0x57D76C,0x57D768; 0x0044CAE0: movl $0x57D768,0x57D770; 0x0044CAEA: mov %ebx,0x57D76C - empty list at 0x0057D768.
    ::g_57d768[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_57d768[1]));
    ::g_57d768[2] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_57d768[0]));
    ::g_57d768[1] = 0U;
    // 0x0044CAF0: movl $0x58A9EC,0x58A9E8; 0x0044CAFA: movl $0x58A9E8,0x58A9F0; 0x0044CB04: mov %ebx,0x58A9EC - empty list at 0x0058A9E8.
    ::g_58a9e8[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_58a9e8[1]));
    ::g_58a9e8[2] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_58a9e8[0]));
    ::g_58a9e8[1] = 0U;

    // 0x0044CB0A: mov $0x58AA28,%esi; 0x0044CB0F: or $0xFFFFFFFF,%ebp - ebp is a saved register holding -1 for the whole rest of the body.
    const std::int32_t minus_one = -1;

    // 0x0044CB12: push %esi; 0x0044CB13: mov %ebx,0x4(%esi); 0x0044CB16: push $0x58A9E8; 0x0044CB1B: mov %ebx,(%esi);
    // 0x0044CB1D: call 0x004AF8F0; then the record clear. Do-while over 128 records of 0x660 linked onto the head at 0x0058A9E8.
    {
        std::uint8_t* node = &::g_58aa28[0];
        do {
            *reinterpret_cast<std::uint32_t*>(node + 0x04U) = 0U;              // 0x0044CB13
            *reinterpret_cast<std::uint32_t*>(node) = 0U;                      // 0x0044CB1B
            Sys_InsertListTail_4AF8F0(&::g_58a9e8[0], node);                   // 0x0044CB1D
            // 0x0044CB22: mov $0x190,%ecx; 0x0044CB27: xor %eax,%eax; 0x0044CB29: lea 0x20(%esi),%edi; 0x0044CB2C: add $0x660,%esi;
            // 0x0044CB32: rep stos %eax,%es:(%edi) - 0x190 DWORD stores of zero starting at the record's +0x20, i.e. 0x640 bytes. At -O2 the
            // compiler emits `memset(record + 0x20, 0, 0x640)` for this loop (shape difference 9 in the provenance block): one port call site
            // that retail does not have as a call, with the same 0x640 zeroed bytes. eax is dead after the retail `rep stos`, so the call's
            // return value is dead here too.
            std::uint32_t* clear = reinterpret_cast<std::uint32_t*>(node + 0x20U);
            node = node + 0x660;                                               // 0x0044CB2C
            for (std::int32_t count = 0; count < 0x190; count = count + 1) {
                clear[count] = 0U;                                             // 0x0044CB32: rep stos
            }
            // The five field stores use the ADVANCED pointer, so their displacements are negative.
            *reinterpret_cast<std::int32_t*>(node - 0x658) = minus_one;        // 0x0044CB34: mov %ebp,-0x658(%esi) -> record+0x08
            *reinterpret_cast<std::int32_t*>(node - 0x654) = minus_one;        // 0x0044CB3A: mov %ebp,-0x654(%esi) -> record+0x0C
            *reinterpret_cast<std::uint32_t*>(node - 0x64C) = 0U;              // 0x0044CB40: mov %ebx,-0x64C(%esi) -> record+0x14
            *reinterpret_cast<std::uint32_t*>(node - 0x648) = 0U;              // 0x0044CB46: mov %ebx,-0x648(%esi) -> record+0x18
            *reinterpret_cast<std::uint32_t*>(node - 0x650) = 0U;              // 0x0044CB4C: mov %ebx,-0x650(%esi) -> record+0x10
        } while (node < &::g_58aa28[0x33000]);                                 // 0x0044CB55/0x0044CB5B: retail bound 0x005BDA28
    }

    // 0x0044CB5D: movl $0x5BDA2C,0x5BDA28; 0x0044CB67: movl $0x5BDA28,0x5BDA30; 0x0044CB71: mov %ebx,0x5BDA2C - empty list at 0x005BDA28.
    ::g_5bda28[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_5bda28[1]));
    ::g_5bda28[2] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_5bda28[0]));
    ::g_5bda28[1] = 0U;
    // 0x0044CB77: movl $0x57D75C,0x57D758; 0x0044CB81: movl $0x57D758,0x57D760; 0x0044CB8B: mov %ebx,0x57D75C - empty list at 0x0057D758.
    ::g_57d758[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_57d758[1]));
    ::g_57d758[2] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_57d758[0]));
    ::g_57d758[1] = 0U;
    // 0x0044CB91: mov $0x57D778,%esi; 0x0044CB96: pop %edi.
    // 0x0044CB97: push %esi; 0x0044CB98: mov %ebx,0x4(%esi); 0x0044CB9B: push $0x57D758; 0x0044CBA0: mov %ebx,(%esi); 0x0044CBA2: call
    // 0x004AF8F0; 0x0044CBA7: add $0xC,%esi; 0x0044CBAD: cmp $0x57D838,%esi; 0x0044CBB3: jl 0x0044CB97. Do-while over 16 records of 0xC.
    {
        std::uint8_t* node = &::g_57d778[0];
        do {
            *reinterpret_cast<std::uint32_t*>(node + 0x04U) = 0U;              // 0x0044CB98
            *reinterpret_cast<std::uint32_t*>(node) = 0U;                      // 0x0044CBA0
            Sys_InsertListTail_4AF8F0(&::g_57d758[0], node);                   // 0x0044CBA2
            node = node + 0x0C;                                                // 0x0044CBA7
        } while (node < &::g_57d778[0xC0]);                                    // 0x0044CBAD/0x0044CBB3: retail bound 0x0057D838
    }

    // 0x0044CBB5: movl $0x58AA1C,0x58AA18; 0x0044CBBF: movl $0x58AA18,0x58AA20; 0x0044CBC9: mov %ebx,0x58AA1C - empty list at 0x0058AA18.
    ::g_58aa18[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_58aa18[1]));
    ::g_58aa18[2] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_58aa18[0]));
    ::g_58aa18[1] = 0U;
    // 0x0044CBCF: movl $0x57C74C,0x57C748; 0x0044CBD9: movl $0x57C748,0x57C750; 0x0044CBE3: mov %ebx,0x57C74C - empty list at 0x0057C748.
    ::g_57c748[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_57c748[1]));
    ::g_57c748[2] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_57c748[0]));
    ::g_57c748[1] = 0U;
    // 0x0044CBE9: movl $0x57E1BC,0x57E1B8; 0x0044CBF3: movl $0x57E1B8,0x57E1C0; 0x0044CBFD: mov %ebx,0x57E1BC - empty list at 0x0057E1B8.
    ::g_57e1b8[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_57e1b8[1]));
    ::g_57e1b8[2] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_57e1b8[0]));
    ::g_57e1b8[1] = 0U;

    // 0x0044CC03: mov $0x57C758,%esi; 0x0044CC08: push %esi; 0x0044CC09: mov %ebx,0x4(%esi); 0x0044CC0C: push $0x57C748;
    // 0x0044CC11: mov %ebx,(%esi); 0x0044CC13: call 0x004AF8F0; 0x0044CC18: lea 0xC(%esi),%eax; 0x0044CC1B: lea 0x10(%esi),%ecx;
    // 0x0044CC1E: add $0x18,%esi; 0x0044CC24: mov %ecx,(%eax); 0x0044CC26: mov %eax,-0x4(%esi); 0x0044CC29: mov %ebx,(%ecx);
    // 0x0044CC2B: mov %ebx,-0x10(%esi); 0x0044CC2E: cmp $0x57CD58,%esi; 0x0044CC34: jl 0x0044CC08. Do-while over 64 records of 0x18.
    {
        std::uint8_t* node = &::g_57c758[0];
        do {
            *reinterpret_cast<std::uint32_t*>(node + 0x04U) = 0U;              // 0x0044CC09
            *reinterpret_cast<std::uint32_t*>(node) = 0U;                      // 0x0044CC11
            Sys_InsertListTail_4AF8F0(&::g_57c748[0], node);                   // 0x0044CC13
            *reinterpret_cast<std::uint32_t*>(node + 0x0C) =                  // 0x0044CC24: mov %ecx,(%eax) -> record+0x0C = record+0x10
                static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(node + 0x10));
            *reinterpret_cast<std::uint32_t*>(node + 0x14) =                  // 0x0044CC26: mov %eax,-0x4(%esi) -> record+0x14 = record+0x0C
                static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(node + 0x0C));
            *reinterpret_cast<std::uint32_t*>(node + 0x10) = 0U;              // 0x0044CC29: mov %ebx,(%ecx)
            *reinterpret_cast<std::uint32_t*>(node + 0x08) = 0U;              // 0x0044CC2B: mov %ebx,-0x10(%esi)
            node = node + 0x18;                                                // 0x0044CC1E
        } while (node < &::g_57c758[0x600]);                                   // 0x0044CC2E/0x0044CC34: retail bound 0x0057CD58
    }

    // 0x0044CC36: mov $0x57CD58,%esi; 0x0044CC3B: push %esi; 0x0044CC3C: mov %ebx,0x4(%esi); 0x0044CC3F: push $0x57E1B8;
    // 0x0044CC44: mov %ebx,(%esi); 0x0044CC46: call 0x004AF8F0; 0x0044CC4B: mov %ebp,0xC(%esi); 0x0044CC4E: mov %ebp,0x8(%esi);
    // 0x0044CC51: mov %ebx,0x10(%esi); 0x0044CC54: add $0x14,%esi; 0x0044CC5A: cmp $0x57D758,%esi; 0x0044CC60: jl 0x0044CC3B.
    // Do-while over 128 records of 0x14.
    {
        std::uint8_t* node = &::g_57cd58[0];
        do {
            *reinterpret_cast<std::uint32_t*>(node + 0x04U) = 0U;              // 0x0044CC3C
            *reinterpret_cast<std::uint32_t*>(node) = 0U;                      // 0x0044CC44
            Sys_InsertListTail_4AF8F0(&::g_57e1b8[0], node);                   // 0x0044CC46
            *reinterpret_cast<std::int32_t*>(node + 0x0C) = minus_one;         // 0x0044CC4B: mov %ebp,0xC(%esi)
            *reinterpret_cast<std::int32_t*>(node + 0x08) = minus_one;         // 0x0044CC4E: mov %ebp,0x8(%esi)
            *reinterpret_cast<std::uint32_t*>(node + 0x10) = 0U;               // 0x0044CC51: mov %ebx,0x10(%esi)
            node = node + 0x14;                                                // 0x0044CC54
        } while (node < &::g_57cd58[0xA00]);                                   // 0x0044CC5A/0x0044CC60: retail bound 0x0057D758
    }

    // 0x0044CC62: pop %esi; 0x0044CC63..0x0044CC8D: eight dword stores of zero at 0x5BDA38, 0x3C, 0x40, 0x44, 0x48, 0x4C, 0x50 and 0x54.
    ::g_5bda38[0] = 0U;
    ::g_5bda38[1] = 0U;
    ::g_5bda38[2] = 0U;
    ::g_5bda38[3] = 0U;
    ::g_5bda38[4] = 0U;
    ::g_5bda38[5] = 0U;
    ::g_5bda38[6] = 0U;
    ::g_5bda38[7] = 0U;

    // 0x0044CC93: pop %ebp; 0x0044CC94: pop %ebx; 0x0044CC95: add esp,0x3ED0 - the frame itself; 0x0044CC9B: ret.
    // **This body never calls 0x004B8810: "HumanAI" is NOT hash-registered (fourth instance in this lane); the names are registered by the
    // untranscribed 0x0044EEB0 it calls above.**
} // end of Task_DefineHumanAI

} // extern "C"
