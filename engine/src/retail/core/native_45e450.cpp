// Native: Task_DefineHuman (behavior-derived placeholder name from the byte-identical type-name literal this body registers; retail symbol unknown); retail VA 0x0045E450-0x0045EAD1.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 1666 bytes (0x682), 5 blocks, 6 edges (two of
//   them self-loop back edges inside the two loops), 447 instructions. Extent closed by the `ret` at 0x0045EAD1; 0x0045EAD2 is a nop pad and any
//   following bytes are the next function's.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image for the block split, and the objdump listing
//   build/retail-dis.txt read over the whole extent instruction by instruction. Blocks: B0 0x0045E450-0x0045E9CF (380 instructions, ends with
//   `jmp 0x0045E9D0`), B1 0x0045E9D0-0x0045E9FA (14, do-while test at the bottom, `jl` back to 0x0045E9D0), B2 0x0045E9FB-0x0045EA01 (2),
//   B3 0x0045EA02-0x0045EA19 (9, do-while test at the bottom, `jl` back to 0x0045EA02), B4 0x0045EA1A-0x0045EAD1 (42, ends with `ret`). Call-site
//   census over the extent: 108 sites over 56 distinct callees - 0x00401400 x39, 0x00401530 x5, 0x00416930 x5, 0x004F1A80 x4, 0x004017C0 x2,
//   0x004169D0 x2, 0x00504A30 x2, and 49 callees with one site each. Every call site was decoded against its callee's own transcribed signature;
//   the argument-less getters were separated from the argument-taking calls by the deferred-push inversion that the already-landed registrars in
//   this lane establish (a `push` written before an argument-less getter survives as an argument of the following call, and the `add esp` after the
//   group balances both). Image reads (all in `.data`, which starts at 0x00536000 and has no `.bss` counterpart in this image): 0x0053F408
//   "Human"; 0x0053F3F8 "CRITERIA_HUMAN"; 0x0053F3E4 "CRITERIA_HUMAN%d"; 0x0053F3C8 "Human_GetCutSceneDeltaTime"; 0x0053F3BC "shadow1.tex";
//   0x0053F3AC "hole_blood1.tex"; 0x0053F39C "hole_blood2.tex"; 0x0053F38C "hole_blood3.tex"; 0x0053F37C "hole_blood4.tex"; 0x0053C148
//   "blood1.spr"; 0x0053F370 "blood2.spr"; 0x0053F348 and 0x0053F34C hold the POINTERS 0x0053F360 "HUMAN_TEAM_GOOD" and 0x0053F350
//   "HUMAN_TEAM_BAD"; 0x0053F316/0x0053F317 are the two bytes of the 0xFFFFFFFF dword at 0x0053F314 that this body overwrites.
// Behavior/CFG: one straight-line registration body with two loops and a 0xCF8-byte frame (`sub esp,0xcf8`, 3320 bytes, no stack probe because it is
//   below the 4 KB probe threshold - there is no probe call site to transcribe). Four saved registers are pushed after the frame (`push
//   ebx/ebp/esi/edi` at 0x0045E456-0x0045E459), one of which (%ebp) is zeroed at 0x0045E464 and used as the literal-zero source for every zero push
//   in the body. In retail order the body: ORs 1 into the dword at 0x00567C5C through 0x00401F60; registers the "Human" type of object size 0xCF8
//   - the same 0xCF8 as the frame - with the parent type Sys_GetGlobal16_4D9610() returns, storing the id at 0x005BDAF8; sets the inherited flags
//   to 1 through 0x00401FA0; allocates three event ids - QTask_AllocateEvent(0) twice, storing only AL at 0x0053F316 and 0x0053F317, and
//   QTaskList_New(0x80) storing the list at 0x005BDC24; then links 44 handlers in retail order: 39 through 0x00401400 (the context variant) and 5
//   through 0x00401530 (the no-context variant). Event ids come from literals (0, 1, 2, 4, 8, 9, 3, 12), from the two allocated bytes
//   (0x0053F316 at 0x0045E826), and from 33 distinct argument-less event-id getters; two handlers are linked twice (0x0045F920 on the event from
//   0x00416850 and again on the event from 0x004FDEF0, and again on the literal 12 - three links), and 9 handlers are shared with other types in
//   the image. It then allocates a 9-record task container into 0x005BDC18 through 0x004F1950 and appends four records through 0x004F1A80 whose
//   frame offsets are 0xE0, 0xF8, 0x100 and 0xFC against the frame base 0x00, with argument 5 = 7 and argument 6 = 0 and argument 4 from
//   0x004F1010, 0x004F0F20, 0x004F0FD0 and 0x004F0FA0. It then registers the criteria names: 0x00504A30("CRITERIA_HUMAN", 0x00460A70) into
//   0x005BDB00, and a do-while over the eight 32-byte string slots at 0x005BDB18 that sprintf's "CRITERIA_HUMAN%d" with the loop index and
//   registers each through 0x00504A30(., 0x00460A10) into the eight dwords at 0x005BDAD8; a do-while over the two name pointers at 0x0053F348
//   registering each through 0x004B8810 with the value 0 and 1; clears five dwords at 0x005BDC28-0x005BDC38 and registers
//   "Human_GetCutSceneDeltaTime" through 0x004B8890 with the handler 0x0045EAE0 and two zero arguments; resolves seven resource records through
//   0x00416930 (five) and 0x004169D0 (two) into 0x005BDB0C, 0x005BDB08, 0x005BDB10, 0x005BDB14, 0x005BDB04, 0x005BDAD4 and 0x005BDAD0; and
//   finally stores AiDetection_RegisterTaskMask_489640(0x005BDAF8) at 0x005BDC3C.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 447-instruction listing and the 5-block CFG; runtime UNOBSERVED. Census in the port
//   object matches retail 1:1 at every target (108 call sites / 56 distinct). Cleanup reconciliation: 193 pushes = 772 bytes, 4 pops = 16 bytes,
//   the frame `sub esp,0xcf8` = 3320 bytes, and the 46 `add esp` sites total 0xFEC = 4076 bytes = 772 - 16 + 3320 exactly, so esp is back at its
//   entry value at the `ret`. The four `add esp` sites that clean more than one group are 0x0045E4DD (0x40 = 16 dwords: the two deferred zeros,
//   the three registration pushes, the two of 0x00401FA0 and the three of each of the first three links), 0x0045E8F9-area groups cleaned together
//   at 0x0045E914 (0x1C = 7 dwords: link 44's three, link 43's three and the container alloc's one), and 0x0045E9BC (0x20 = 8 dwords: the fourth
//   append's six plus the two of 0x00504A30). Link status: NOT LINKABLE YET - 17 of the 56 callees are not transcribed anywhere in the tree
//   (0x00401F60, 0x004F1FC0, 0x0047CDC0, 0x004FDEF0, 0x00466230, 0x00484D30, 0x00450C80, 0x00450CC0, 0x0046D410, 0x0046C970,
//   0x00477C00-0x00477C40, 0x00477030, 0x00504A30), so check_native_linkable.py reports them as undefined and this file is compile-checked only.
//   Thirteen of the seventeen are already on this worker's second target list; 0x00401F60, 0x0047CDC0 and 0x00450C80/0x00450CC0 are not.
//   Documented shape differences, all value-preserving: (1) retail pushes FIVE dwords for 0x00401900 (the two leading zeros are the task-list
//   argument and the fifth dword that callee never reads - see task_types.hpp) and cleans them in the 0x40; the port's four-parameter signature
//   emits one push for the task-list argument; (2) the 0xCF8-byte frame is reproduced as one addressable dword whose address plus a constant
//   stands in for the frame base, which is sound because 0x004F1A80 reads arguments 2 and 3 only as their difference and dereferences neither, so
//   the recorded values are record parameter-block sizes rather than frame pointers; (3) at 0x0045EA49-0x0045EA8A retail pushes the NEXT string
//   argument before storing the PREVIOUS callee's result, so its instruction order is push/call/store/call; the port's call/store/call keeps the
//   observable order (each call completes before its result is stored and before the next call starts) and only moves a stack immediate write
//   that has no reader; (4) the two `.data` pointer constants at 0x0053F348/0x0053F34C are reproduced as a table of byte-identical string
//   literals, so retail's writable table of pointers into its own strings becomes a read-only table of literal addresses - retail only ever reads
//   both entries; (5) the `push %ebx/%ebp/%esi/%edi` at 0x0045E456-0x0045E459 are register saves, not call arguments, and are reproduced as the
//   port's locals.
//   Global ownership from the image-wide immediate census (a byte scan of `.text` for each four-byte address): 0x005BDAF8 has 49 reference sites
//   (id reads by the handlers this body links and by other types), so it is defined by 0x00460BE0 and declared `extern` here; 0x005BDC18 has 9
//   sites, defined by 0x00460C50 and declared `extern` here; 0x0053F316 has 4 sites (two here, two inside 0x00460B60/0x00460C31) and
//   0x0053F317 has 3, defined by 0x00460C30 and 0x00460C40 and declared `extern` here. 0x005BDAD0 (0x0045EAB5 + 0x00463EAD), 0x005BDAD4
//   (0x0045EAA4 + 0x00463D73), 0x005BDAD8 (0x0045E9C7 + 0x00460A46 + 0x00460B93), 0x005BDB00 (0x0045E9C0 + 0x00460B85), 0x005BDB04
//   (0x0045EA95 + 0x004641C0), 0x005BDB08 (0x0045EA68 + 0x00463CBF), 0x005BDB0C (0x0045EA59 + 0x0045EBDD), 0x005BDB10
//   (0x0045EA77 + 0x00463CB4), 0x005BDB14 (0x0045EA86 + 0x004641CB), 0x005BDC24 (0x0045E4AC, 0x0045EB31, 0x0045EF54, 0x00460B21,
//   0x00460C61) and 0x005BDC3C (0x0045EAC3 + 0x00464251) are referenced by the handlers this body links, which must declare them `extern`;
//   0x005BDC28-0x005BDC38 are referenced only at this body's five stores and 0x005BDB18 only at this body's loop setup, so those definitions are
//   owned here outright. All eleven are zero fill in the image (all are past `.data`'s raw size, so the port's initialisers are 0), and the two
//   bytes at 0x0053F316/0x0053F317 are 0xFF in the image, which is the initialiser their owners already carry.
//   The handlers 0x0045EB30, 0x0045EEA0, 0x0045EF40, 0x0045F280, 0x0045F050, 0x0045F500, 0x0045F800, 0x0045F880, 0x0045F890, 0x0045F900,
//   0x0045F910, 0x0045E3E0, 0x0045F930, 0x0045F920, 0x0045F9D0, 0x0045FA80, 0x00416AB0, 0x0045FCD0, 0x0045FDA0, 0x0045FE30, 0x0042FC50,
//   0x0045FE70, 0x0045FF80, 0x0045FFC0, 0x00460070, 0x00460090, 0x004600C0, 0x004601A0, 0x00460210, 0x004D3710, 0x00460280, 0x00460400,
//   0x004606D0, 0x004606F0, 0x004F64E0, 0x0045E3D0, 0x00460850, 0x004608D0, 0x00460950, 0x004609A0, 0x004609F0, 0x00460AC0 and the two
//   criteria handlers 0x00460A10/0x00460A70 are raw code immediates in retail and are not transcribed here; 0x004D3710 is the most-shared handler
//   in the image (163 reference sites). Domain: core/, because this is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>
#include <cstdio>

extern "C" {

void TaskType_SetInheritedFlags_401FA0(std::uint32_t type_id, std::uint32_t flags); // 0x00401FA0 (core/native_00401fa0.cpp)
std::uint16_t Sys_GetGlobal16_4D9610(void);                                         // 0x004D9610: argument-less word getter (parent type)
void* QTaskList_New(std::int32_t const capacity);                                   // 0x004C1800 (core/qtask_list.cpp)
void Core_OrGlobalFlags_401F60(std::uint32_t const flags);                          // 0x00401F60: ors the argument into the dword at 0x00567C5C

std::uint8_t Render_GetByte_4D14D0(void);                                           // 0x004D14D0: argument-less byte getter
std::uint8_t Script_PhaseIdGetE4_4F1A60(void);                                      // 0x004F1A60: argument-less byte getter
std::uint8_t Script_PhaseIdGetE5_4F1A70(void);                                      // 0x004F1A70: argument-less byte getter
std::uint8_t Sys_GetSettingByte_416830(void);                                       // 0x00416830: argument-less byte getter
std::uint8_t Sys_GetSettingByte_416840(void);                                       // 0x00416840: argument-less byte getter
std::uint8_t Sys_GetSettingByte_416850(void);                                       // 0x00416850: argument-less byte getter
std::uint8_t Sys_GetSettingByte_416860(void);                                       // 0x00416860: argument-less byte getter
std::uint8_t Sys_GetSettingByte_416880(void);                                       // 0x00416880: argument-less byte getter
std::uint8_t Sys_GetGlobal8_4FC840(void);                                           // 0x004FC840: argument-less byte getter
std::uint8_t Sys_GetGlobal8_487190(void);                                           // 0x00487190: argument-less byte getter
std::uint8_t Sys_GetGlobal8_46AB20(void);                                           // 0x0046AB20: argument-less byte getter
std::uint8_t Sys_GetGlobal8_489610(void);                                           // 0x00489610: argument-less byte getter
std::uint8_t Sys_GetGlobal8_489620(void);                                           // 0x00489620: argument-less byte getter
std::uint8_t QTask_PhaseIdGet548176_4CEA10(void);                                   // 0x004CEA10: argument-less byte getter
std::uint8_t QTask_PhaseIdGet548177_4CEA20(void);                                   // 0x004CEA20: argument-less byte getter
std::uint8_t Task_GetCachedEventId_467D90(void);                                    // 0x00467D90: argument-less byte getter
std::uint8_t Task_GetCachedEventId_4E6610(void);                                    // 0x004E6610: argument-less byte getter
std::uint8_t Task_GetCachedEventId_5096A0(void);                                    // 0x005096A0: argument-less byte getter

// 0x004F1FC0 (`mov al, byte [0x00A76C02]; ret`), 0x0047CDC0 (`mov al, byte [0x005C10E8]; ret`), 0x004FDEF0 (`mov al, byte [0x0054E56A]; ret`),
// 0x00466230 (`mov al, byte [0x0053F4FE]; ret`), 0x00484D30 (`mov al, byte [0x00540992]; ret`), 0x0046D410 (`mov al, byte [0x0053FE28]; ret`),
// 0x0046C970 (`mov al, byte [0x0053FDE0]; ret`), 0x00477C00-0x00477C40 (`mov al, byte [0x005407B8..0x005407BC]; ret`) and 0x00477030
// (`mov al, byte [0x005BE3B8]; ret`) are the same argument-less cached-event-id getter shape as the three already transcribed above; the
// 0x00450C80/0x00450CC0 pair is the lazy variant (`mov al,[..]; cmp 0xff; jne; push 0; call 0x004017C0; ...`). None of the seventeen is
// transcribed yet, so these are the callees this file cannot link against; the placeholder names follow the shapes in their own bodies, which were
// read for the signatures and are recorded in this file's Status/limits.
std::uint8_t Task_GetCachedEventId_4F1FC0(void);                                    // 0x004F1FC0
std::uint8_t Task_GetCachedEventId_47CDC0(void);                                    // 0x0047CDC0
std::uint8_t Task_GetCachedEventId_4FDEF0(void);                                    // 0x004FDEF0
std::uint8_t Task_GetCachedEventId_466230(void);                                    // 0x00466230
std::uint8_t Task_GetCachedEventId_484D30(void);                                    // 0x00484D30
std::uint8_t Task_GetCachedEventId_450C80(void);                                    // 0x00450C80: lazy one-shot event-id getter
std::uint8_t Task_GetCachedEventId_450CC0(void);                                    // 0x00450CC0: lazy one-shot event-id getter
std::uint8_t Task_GetCachedEventId_46D410(void);                                    // 0x0046D410
std::uint8_t Task_GetCachedEventId_46C970(void);                                    // 0x0046C970
std::uint8_t Task_GetCachedEventId_477C00(void);                                    // 0x00477C00
std::uint8_t Task_GetCachedEventId_477C10(void);                                    // 0x00477C10
std::uint8_t Task_GetCachedEventId_477C20(void);                                    // 0x00477C20
std::uint8_t Task_GetCachedEventId_477C30(void);                                    // 0x00477C30
std::uint8_t Task_GetCachedEventId_477C40(void);                                    // 0x00477C40
std::uint8_t Task_GetCachedEventId_477030(void);                                    // 0x00477030

std::uint32_t Sys_GetGlobal32_4F1210(void);                                         // 0x004F1210: argument-less dword getter
std::uint32_t Sys_GetGlobal32_4F1220(void);                                         // 0x004F1220: argument-less dword getter
std::uint32_t Sys_GetGlobal32_4F1010(void);                                         // 0x004F1010: argument-less dword getter
std::uint32_t Sys_GetGlobal32_4F0F20(void);                                         // 0x004F0F20: argument-less dword getter
std::uint32_t Core_GetRenderWordB_4F0FD0(void);                                     // 0x004F0FD0: argument-less dword getter
std::uint32_t Core_GetRenderWordA_4F0FA0(void);                                     // 0x004F0FA0: argument-less dword getter

void* Script_TaskContainer_Alloc(std::int32_t const count);                          // 0x004F1950 (script/native_4f1950.cpp)
void Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2, std::int32_t const arg3,
                                 std::int32_t const arg4, std::int32_t const arg5, std::int32_t const arg6); // 0x004F1A80
// 0x00504A30 scans the 31-entry record table at 0x00A7A640 with stride 0x0C for a free slot (a zero first byte), stores the name at +4 and the
// handler at +8, registers the name through 0x004B8810 with the value 1 << index and returns that mask; with no free slot it prints the message
// at 0x0054E6BC through 0x004AF7B0 and loops forever at 0x00504A54. Not transcribed yet.
std::uint32_t Script_RegisterCriteria_504A30(char const* const name, void* const handler); // 0x00504A30
void Script_QHashNodeRegister(void* const arg1, void* const value);                  // 0x004B8810 (script/native_4b8810.cpp)
void Script_QHashNodeRegisterEx(void* const arg0, void* const arg1, char const* const arg2,
                                void* const arg3);                                   // 0x004B8890 (script/native_4b8890.cpp)
void* Sys_LookupOrAppendRecord_416930(char const* const name);                        // 0x00416930 (config/native_416930.cpp)
void* Sys_LookupOrAppendRecord_4169D0(char const* const name);                        // 0x004169D0 (config/native_4169d0.cpp)
std::uint32_t AiDetection_RegisterTaskMask_489640(std::uint16_t const arg1);          // 0x00489640 (core/native_489640.cpp)

// 0x005BDAF8: the "Human" task type id (word), defined by 0x00460BE0 (zero in the image; 49 reference sites).
extern std::uint16_t g_5bdaf8;

// 0x005BDC18: the container his body allocates is defined by 0x00460C50 (zero in the image; 9 reference sites), not here - this body only stores
// into it.
extern std::uint32_t g_5bdc18;

// 0x0053F316 / 0x0053F317: the two bytes this body overwrites with the low byte of the two QTask_AllocateEvent results. They are two of the four
// bytes of the 0xFFFFFFFF dword at 0x0053F314; defined by 0x00460C30 and 0x00460C40 with their image value 0xFF.
extern std::uint8_t g_53f316;
extern std::uint8_t g_53f317;

// 0x005BDAD0, 0x005BDAD4, 0x005BDB00, 0x005BDB04, 0x005BDB08, 0x005BDB0C, 0x005BDB10, 0x005BDB14 and 0x005BDC3C: the criteria and resource
// handles this body stores. All are zero fill in the image; each is also read by the handlers this body links, which must declare them `extern`.
std::uint32_t g_5bdad0 = 0U;
std::uint32_t g_5bdad4 = 0U;
std::uint32_t g_5bdb00 = 0U;
std::uint32_t g_5bdb04 = 0U;
std::uint32_t g_5bdb08 = 0U;
std::uint32_t g_5bdb0c = 0U;
std::uint32_t g_5bdb10 = 0U;
std::uint32_t g_5bdb14 = 0U;
std::uint32_t g_5bdc24 = 0U;
std::uint32_t g_5bdc3c = 0U;

// 0x005BDAD8: the eight dword handle slots the criteria loop fills, contiguous in the image from 0x005BDAD8 to 0x005BDAF4 - the slot at 0x005BDC18
// the loop compares against is the *next global's address*, 0x100 bytes past the buffer base, not an element of this array. Zero fill in the image;
// also read by the handlers at 0x00460A46 and 0x00460B93, which must declare it `extern`.
std::uint32_t g_5bdad8[8] = {};

// 0x005BDB18: the eight 32-byte string slots the criteria loop sprintf's into, 0x100 bytes from 0x005BDB18 to the next global at 0x005BDC18. Zero
// fill in the image and referenced nowhere else in `.text`, so this definition is owned here.
char g_5bdb18[0x100] = {};

// 0x005BDC28-0x005BDC38: the five dwords this body clears before the 0x004B8890 registration. Zero fill in the image and referenced nowhere else,
// so the definitions are owned here.
std::uint32_t g_5bdc28 = 0U;
std::uint32_t g_5bdc2c = 0U;
std::uint32_t g_5bdc30 = 0U;
std::uint32_t g_5bdc34 = 0U;
std::uint32_t g_5bdc38 = 0U;

// 0x0053F348/0x0053F34C: the two name POINTERS retail's second loop walks, in table order. The image holds 0x0053F360 and 0x0053F350; the strings
// are byte-identical literals here, so this table holds the literals' addresses instead of retail's own writable pointer pair - retail only ever
// reads both entries.
char const* g_53f348[2] = {
    "HUMAN_TEAM_GOOD", // [0] retail 0x0053F348 -> 0x0053F360, registered with value 0
    "HUMAN_TEAM_BAD",  // [1] retail 0x0053F34C -> 0x0053F350, registered with value 1
};

void Task_DefineHuman()
{
    // 0x0045E450: sub esp,0xcf8 - the frame, 3320 bytes, no probe because it is below 4 KB. 0x0045E456-0x0045E459: push ebx/ebp/esi/edi - four
    // saved registers, not arguments. The frame is reproduced as one addressable dword whose address stands in for the frame base, because
    // 0x004F1A80 needs frame offsets 0x00, 0xE0, 0xF8, 0xFC and 0x100 and reads its two frame arguments only as their difference.
    std::int32_t append_frame_base = 0;
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0045E45A: push 0x1; 0x0045E45C: call 0x00401F60; 0x0045E461: add esp,0x4 - the only call in this body whose `add` cleans that one push
    // alone, which is what fixes the callee's arity at one.
    Core_OrGlobalFlags_401F60(0x1U);

    // 0x0045E464: xor %ebp,%ebp - the zero register this body uses for every literal-zero push.
    // 0x0045E466/0x0045E467: push 0; push 0 - arguments 4 and 5 of the 0x00401900 call below, pushed BEFORE the argument-less parent getter: the
    // deferred-push inversion. 0x0045E468: call 0x004D9610 (parent type); 0x0045E46D: push %eax; 0x0045E46E: push 0xcf8; 0x0045E473:
    // push 0x0053F408; 0x0045E478: call 0x00401900.
    const std::uint16_t human_id = TaskType_Register("Human", // 0x0045E473: push 0x0053F408
                                                     0xCF8U,  // 0x0045E46E: push 0xcf8 (object size, equal to the frame)
                                                     Sys_GetGlobal16_4D9610(), // 0x0045E468: call 0x004D9610 (parent type)
                                                     0U);     // 0x0045E467: push 0 (task-list argument)

    // 0x0045E47D: push 0x1 (argument 2, the flags); 0x0045E47F: push %eax (argument 1, the id); 0x0045E480: mov word [0x005BDAF8],ax;
    // 0x0045E486: call 0x00401FA0. The store precedes the call in retail and is kept before it here.
    ::g_5bdaf8 = human_id; // 0x0045E480
    TaskType_SetInheritedFlags_401FA0(static_cast<std::uint32_t>(human_id), 0x1U);

    // 0x0045E48B: push 0; 0x0045E48C: call 0x004017C0 (QTask_AllocateEvent); 0x0045E491: push 0 (the second call's argument, pushed before the
    // first call's result is stored); 0x0045E492: mov byte [0x0053F316],al - retail stores only AL; 0x0045E497: call 0x004017C0;
    // 0x0045E49C: push 0x80; 0x0045E4A1: mov byte [0x0053F317],al; 0x0045E4A6: call 0x004C1800 (QTaskList_New); 0x0045E4AB: mov [0x005BDC24],eax.
    ::g_53f316 = static_cast<std::uint8_t>(QTask_AllocateEvent(0));
    ::g_53f317 = static_cast<std::uint8_t>(QTask_AllocateEvent(0));
    ::g_5bdc24 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(QTaskList_New(0x80)));

    // 0x0045E4B0: mov %ax,[0x005BDAF8]; 0x0045E4B6: push 0x0045EB30 (handler); 0x0045E4BB: push %ebp (argument 2, literal event 0);
    // 0x0045E4BC: push %eax (argument 1); 0x0045E4BD: call 0x00401400.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45EB30U)));

    // 0x0045E4C2: mov %cx,[0x005BDAF8]; 0x0045E4C9: push 0x0045EEA0; 0x0045E4CE: push 0x1; 0x0045E4D0: push %ecx; 0x0045E4D1: call 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(::g_5bdaf8, 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45EEA0U)));

    // 0x0045E4D6: mov %dx,[0x005BDAF8]; 0x0045E4DD: add esp,0x40 (the first three links and the registration group together); 0x0045E4E0:
    // push 0x0045EF40; 0x0045E4E5: push 0x2; 0x0045E4E7: push %edx; 0x0045E4E8: call 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(::g_5bdaf8, 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45EF40U)));

    // 0x0045E4ED: mov %ax,[0x005BDAF8]; 0x0045E4F3: push 0x0045F280; 0x0045E4F8: push 0x4; 0x0045E4FA: push %eax; 0x0045E4FB: call 0x00401400.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, 0x4U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F280U)));

    // 0x0045E500: add esp,0x18; 0x0045E503: push 0x0045F050 (handler, deferred argument 3); 0x0045E508: call 0x004D14D0 (supplies argument 2);
    // 0x0045E50D: mov %cx,[0x005BDAF8]; 0x0045E514: push %eax; 0x0045E515: push %ecx; 0x0045E516: call 0x00401400.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Render_GetByte_4D14D0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F050U)));

    // 0x0045E51B: mov %dx,[0x005BDAF8]; 0x0045E522: push 0x0045F500; 0x0045E527: push 0x9; 0x0045E529: push %edx; 0x0045E52A: call 0x00401400.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, 0x9U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F500U)));

    // 0x0045E52F: mov %ax,[0x005BDAF8]; 0x0045E535: push 0x0045F800; 0x0045E53A: push 0x8; 0x0045E53C: push %eax; 0x0045E53D: call 0x00401400.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, 0x8U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F800U)));

    // 0x0045E542: add esp,0x24 (the three links above); 0x0045E545: push 0x0045F880 (deferred argument 3); 0x0045E54A: call 0x004F1A60;
    // 0x0045E54F: mov %cx,[0x005BDAF8]; 0x0045E556: push %eax; 0x0045E557: push %ecx; 0x0045E558: call 0x00401400; 0x0045E55D: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F880U)));

    // 0x0045E560: push 0x0045F890 (deferred argument 3); 0x0045E565: call 0x004F1A70; 0x0045E56A: mov %dx,[0x005BDAF8]; 0x0045E571: push %eax;
    // 0x0045E572: push %edx; 0x0045E573: call 0x00401400; 0x0045E578: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F890U)));

    // 0x0045E57B: push 0x0045F900 (deferred argument 3); 0x0045E580: call 0x004F1FC0; 0x0045E585: push %eax; 0x0045E586: mov %ax,[0x005BDAF8];
    // 0x0045E58C: push %eax; 0x0045E58D: call 0x00401400; 0x0045E592: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_4F1FC0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F900U)));

    // 0x0045E595: push 0x0045F910 (deferred argument 3); 0x0045E59A: call 0x0047CDC0; 0x0045E59F: mov %cx,[0x005BDAF8]; 0x0045E5A6: push %eax;
    // 0x0045E5A7: push %ecx; 0x0045E5A8: call 0x00401400; 0x0045E5AD: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_47CDC0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F910U)));

    // 0x0045E5B0: push 0x0045E3E0; 0x0045E5B5: call 0x00416830; 0x0045E5BA: mov %dx,[0x005BDAF8]; 0x0045E5C1: push %eax; 0x0045E5C2: push %edx;
    // 0x0045E5C3: call 0x00401400; 0x0045E5C8: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Sys_GetSettingByte_416830()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45E3E0U)));

    // 0x0045E5CB: push 0x0045F930; 0x0045E5D0: call 0x00416840; 0x0045E5D5: push %eax; 0x0045E5D6: mov %ax,[0x005BDAF8]; 0x0045E5DC: push %eax;
    // 0x0045E5DD: call 0x00401400; 0x0045E5E2: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F930U)));

    // 0x0045E5E5: push 0x0045F920; 0x0045E5EA: call 0x00416850; 0x0045E5EF: mov %cx,[0x005BDAF8]; 0x0045E5F6: push %eax; 0x0045E5F7: push %ecx;
    // 0x0045E5F8: call 0x00401400; 0x0045E5FD: add esp,0xc. Handler 0x0045F920 is linked three times in this body.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Sys_GetSettingByte_416850()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F920U)));

    // 0x0045E600: push 0x0045F920; 0x0045E605: call 0x004FDEF0; 0x0045E60A: mov %dx,[0x005BDAF8]; 0x0045E611: push %eax; 0x0045E612: push %edx;
    // 0x0045E613: call 0x00401400; 0x0045E618: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_4FDEF0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F920U)));

    // 0x0045E61B: push 0x0045F9D0; 0x0045E620: call 0x00416860; 0x0045E625: push %eax; 0x0045E626: mov %ax,[0x005BDAF8]; 0x0045E62C: push %eax;
    // 0x0045E62D: call 0x00401400; 0x0045E632: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Sys_GetSettingByte_416860()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F9D0U)));

    // 0x0045E635: push 0x0045FA80; 0x0045E63A: call 0x00466230; 0x0045E63F: mov %cx,[0x005BDAF8]; 0x0045E646: push %eax; 0x0045E647: push %ecx;
    // 0x0045E648: call 0x00401400; 0x0045E64D: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_466230()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45FA80U)));

    // 0x0045E650: push 0x00416AB0; 0x0045E655: call 0x00416880; 0x0045E65A: mov %dx,[0x005BDAF8]; 0x0045E661: push %eax; 0x0045E662: push %edx;
    // 0x0045E663: call 0x00401400; 0x0045E668: add esp,0xc. The handler here is 0x00416AB0, not a 0x0045xxxx address.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Sys_GetSettingByte_416880()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x416AB0U)));

    // 0x0045E66B: push 0x0045FCD0; 0x0045E670: call 0x00484D30; 0x0045E675: push %eax; 0x0045E676: mov %ax,[0x005BDAF8]; 0x0045E67C: push %eax;
    // 0x0045E67D: call 0x00401400; 0x0045E682: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_484D30()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45FCD0U)));

    // 0x0045E685: push 0x0045FDA0; 0x0045E68A: call 0x00450C80; 0x0045E68F: mov %cx,[0x005BDAF8]; 0x0045E696: push %eax; 0x0045E697: push %ecx;
    // 0x0045E698: call 0x00401400; 0x0045E69D: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_450C80()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45FDA0U)));

    // 0x0045E6A0: push 0x0045FE30; 0x0045E6A5: call 0x00489620; 0x0045E6AA: push %eax; 0x0045E6AB: mov %dx,[0x005BDAF8]; 0x0045E6B2: push %edx;
    // 0x0045E6B3: call 0x00401400; 0x0045E6B8: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Sys_GetGlobal8_489620()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45FE30U)));

    // 0x0045E6BB: push 0x0042FC50; 0x0045E6C0: call 0x00489610; 0x0045E6C5: push %eax; 0x0045E6C6: mov %ax,[0x005BDAF8]; 0x0045E6CC: push %eax;
    // 0x0045E6CD: call 0x00401400; 0x0045E6D2: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Sys_GetGlobal8_489610()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x42FC50U)));

    // 0x0045E6D5: push 0x0045FE70; 0x0045E6DA: call 0x00450CC0; 0x0045E6DF: mov %cx,[0x005BDAF8]; 0x0045E6E6: push %eax; 0x0045E6E7: push %ecx;
    // 0x0045E6E8: call 0x00401400; 0x0045E6ED: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_450CC0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45FE70U)));

    // 0x0045E6F0: push 0x0045FF80; 0x0045E6F5: call 0x0046D410; 0x0045E6FA: mov %dx,[0x005BDAF8]; 0x0045E701: push %eax; 0x0045E702: push %edx;
    // 0x0045E703: call 0x00401400; 0x0045E708: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_46D410()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45FF80U)));

    // 0x0045E70B: push 0x0045FFC0; 0x0045E710: call 0x0046C970; 0x0045E715: push %eax; 0x0045E716: mov %ax,[0x005BDAF8]; 0x0045E71C: push %eax;
    // 0x0045E71D: call 0x00401400; 0x0045E722: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_46C970()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45FFC0U)));

    // 0x0045E725: push 0x00460070; 0x0045E72A: call 0x00477C00; 0x0045E72F: mov %cx,[0x005BDAF8]; 0x0045E736: push %eax; 0x0045E737: push %ecx;
    // 0x0045E738: call 0x00401400; 0x0045E73D: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_477C00()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x460070U)));

    // 0x0045E740: push 0x00460090; 0x0045E745: call 0x00477C10; 0x0045E74A: mov %dx,[0x005BDAF8]; 0x0045E751: push %eax; 0x0045E752: push %edx;
    // 0x0045E753: call 0x00401400; 0x0045E758: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_477C10()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x460090U)));

    // 0x0045E75B: push 0x004600C0; 0x0045E760: call 0x00477C20; 0x0045E765: push %eax; 0x0045E766: mov %ax,[0x005BDAF8]; 0x0045E76C: push %eax;
    // 0x0045E76D: call 0x00401400; 0x0045E772: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_477C20()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4600C0U)));

    // 0x0045E775: push 0x004601A0; 0x0045E77A: call 0x00477C30; 0x0045E77F: mov %cx,[0x005BDAF8]; 0x0045E786: push %eax; 0x0045E787: push %ecx;
    // 0x0045E788: call 0x00401400; 0x0045E78D: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_477C30()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4601A0U)));

    // 0x0045E790: push 0x00460210; 0x0045E795: call 0x00477C40; 0x0045E79A: mov %dx,[0x005BDAF8]; 0x0045E7A1: push %eax; 0x0045E7A2: push %edx;
    // 0x0045E7A3: call 0x00401400; 0x0045E7A8: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_477C40()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x460210U)));

    // 0x0045E7AB: push 0x004D3710; 0x0045E7B0: call 0x004FC840; 0x0045E7B5: push %eax; 0x0045E7B6: mov %ax,[0x005BDAF8]; 0x0045E7BC: push %eax;
    // 0x0045E7BD: call 0x00401400; 0x0045E7C2: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Sys_GetGlobal8_4FC840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D3710U)));

    // 0x0045E7C5: push 0x00460280; 0x0045E7CA: call 0x00467D90; 0x0045E7CF: mov %cx,[0x005BDAF8]; 0x0045E7D6: push %eax; 0x0045E7D7: push %ecx;
    // 0x0045E7D8: call 0x00401400; 0x0045E7DD: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x460280U)));

    // 0x0045E7E0: push 0x00460400; 0x0045E7E5: call 0x00487190; 0x0045E7EA: mov %dx,[0x005BDAF8]; 0x0045E7F1: push %eax; 0x0045E7F2: push %edx;
    // 0x0045E7F3: call 0x00401400; 0x0045E7F8: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Sys_GetGlobal8_487190()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x460400U)));

    // 0x0045E7FB: push 0x004606D0; 0x0045E800: call 0x004E6610; 0x0045E805: push %eax; 0x0045E806: mov %ax,[0x005BDAF8]; 0x0045E80C: push %eax;
    // 0x0045E80D: call 0x00401400. Link 35's three pushes follow before the group's `add esp,0x18` at 0x0045E82B.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_4E6610()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4606D0U)));

    // 0x0045E812: mov %cl, byte [0x0053F316] - the event for this link is read out of the byte allocated near the top of the body;
    // 0x0045E818: mov %dx,[0x005BDAF8]; 0x0045E81F: push 0x004606F0; 0x0045E824: push %ecx; 0x0045E825: push %edx; 0x0045E826: call 0x00401400;
    // 0x0045E82B: add esp,0x18 (the 6 dwords of links 34 and 35 together).
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(::g_53f316),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4606F0U)));

    // 0x0045E82E: push 0x004F64E0; 0x0045E833: call 0x00477030; 0x0045E838: push %eax; 0x0045E839: mov %ax,[0x005BDAF8]; 0x0045E83F: push %eax;
    // 0x0045E840: call 0x00401400; 0x0045E845: mov %cx,[0x005BDAF8].
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_477030()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4F64E0U)));

    // 0x0045E84C: push 0x0045F920 (the third link of that handler in this body); 0x0045E851: push 0xc; 0x0045E853: push %ecx;
    // 0x0045E854: call 0x00401400; 0x0045E859: add esp,0x18 (links 36 and 37 together).
    QTask_LinkQTaskEventHandler(::g_5bdaf8, 0xCU,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45F920U)));

    // 0x0045E85C: push 0x0045E3D0; 0x0045E861: call 0x005096A0; 0x0045E866: mov %dx,[0x005BDAF8]; 0x0045E86D: push %eax; 0x0045E86E: push %edx;
    // 0x0045E86F: call 0x00401400; 0x0045E874: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Task_GetCachedEventId_5096A0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45E3D0U)));

    // 0x0045E877: push 0x00460850; 0x0045E87C: call 0x004F1210; 0x0045E881: push %eax; 0x0045E882: mov %ax,[0x005BDAF8]; 0x0045E888: push %eax;
    // 0x0045E889: call 0x00401530 (the no-context variant); 0x0045E88E: add esp,0xc.
    QTask_LinkQTaskEventHandlerNoContextFn(::g_5bdaf8, static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x460850U)));

    // 0x0045E891: push 0x004608D0; 0x0045E896: call 0x004F1220; 0x0045E89B: mov %cx,[0x005BDAF8]; 0x0045E8A2: push %eax; 0x0045E8A3: push %ecx;
    // 0x0045E8A4: call 0x00401530; 0x0045E8A9: add esp,0xc.
    QTask_LinkQTaskEventHandlerNoContextFn(::g_5bdaf8, static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4608D0U)));

    // 0x0045E8AC: push 0x00460950; 0x0045E8B1: call 0x004CEA10; 0x0045E8B6: mov %dx,[0x005BDAF8]; 0x0045E8BD: push %eax; 0x0045E8BE: push %edx;
    // 0x0045E8BF: call 0x00401400; 0x0045E8C4: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x460950U)));

    // 0x0045E8C7: push 0x004609A0; 0x0045E8CC: call 0x004CEA20; 0x0045E8D1: push %eax; 0x0045E8D2: mov %ax,[0x005BDAF8]; 0x0045E8D8: push %eax;
    // 0x0045E8D9: call 0x00401400; 0x0045E8DE: add esp,0xc.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4609A0U)));

    // 0x0045E8E1: push 0x004609F0; 0x0045E8E6: call 0x0046AB20; 0x0045E8EB: mov %cx,[0x005BDAF8]; 0x0045E8F2: push %eax; 0x0045E8F3: push %ecx;
    // 0x0045E8F4: call 0x00401400.
    QTask_LinkQTaskEventHandler(::g_5bdaf8, static_cast<std::uint32_t>(Sys_GetGlobal8_46AB20()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4609F0U)));

    // 0x0045E8F9: mov %dx,[0x005BDAF8]; 0x0045E900: push 0x00460AC0; 0x0045E905: push 0x3; 0x0045E907: push %edx; 0x0045E908: call 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(::g_5bdaf8, 0x3U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x460AC0U)));

    // 0x0045E90D: push 0x9; 0x0045E90F: call 0x004F1950 (Script_TaskContainer_Alloc); 0x0045E914: add esp,0x1c = seven dwords = the two links
    // above (three each) plus the alloc's one; 0x0045E91A: mov [0x005BDC18],eax.
    ::g_5bdc18 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(9))); // 0x0045E90F

    // 0x0045E917: push 0 (argument 6); 0x0045E918: push 7 (argument 5); 0x0045E91F: call 0x004F1010 (argument 4, argument-less getter);
    // 0x0045E92B: lea 0xfc(%esp),%eax - with esp at frame-0x1C there, that is frame+0xE0 (argument 3); 0x0045E932: lea 0x1c(%esp),%ecx -
    // frame+0x00 (argument 2); 0x0045E938: push %edx (argument 1, the container); 0x0045E939: call 0x004F1A80; 0x0045E93E: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc18)),
                                append_frame + 0x00,   // 0x0045E932: argument 2, the frame base
                                append_frame + 0xE0,   // 0x0045E92B: argument 3, frame+0xE0
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), // 0x0045E91F: argument 4
                                7,                     // 0x0045E918: argument 5
                                0);                    // 0x0045E917: argument 6

    // 0x0045E941: push 0 (argument 6); 0x0045E942: push 7 (argument 5); 0x0045E944: call 0x004F0F20 (argument 4); 0x0045E950:
    // lea 0x114(%esp),%eax - frame+0xF8 (argument 3); 0x0045E957: lea 0x1c(%esp),%ecx - frame+0x00 (argument 2); 0x0045E95D: push %edx;
    // 0x0045E95E: call 0x004F1A80; 0x0045E963: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc18)),
                                append_frame + 0x00,   // 0x0045E957: argument 2, the frame base
                                append_frame + 0xF8,   // 0x0045E950: argument 3, frame+0xF8
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F20()), // 0x0045E944: argument 4
                                7,                     // 0x0045E942: argument 5
                                0);                    // 0x0045E941: argument 6

    // 0x0045E966: push 0 (argument 6); 0x0045E967: push 7 (argument 5); 0x0045E969: call 0x004F0FD0 (argument 4); 0x0045E975:
    // lea 0x11c(%esp),%eax - frame+0x100 (argument 3); 0x0045E97C: lea 0x1c(%esp),%ecx - frame+0x00 (argument 2); 0x0045E982: push %edx;
    // 0x0045E983: call 0x004F1A80; 0x0045E988: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc18)),
                                append_frame + 0x00,   // 0x0045E97C: argument 2, the frame base
                                append_frame + 0x100,  // 0x0045E975: argument 3, frame+0x100
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), // 0x0045E969: argument 4
                                7,                     // 0x0045E967: argument 5
                                0);                    // 0x0045E966: argument 6

    // 0x0045E98B: push 0 (argument 6); 0x0045E98C: push 7 (argument 5); 0x0045E98E: call 0x004F0FA0 (argument 4, argument-less getter);
    // 0x0045E99A: lea 0x118(%esp),%eax - frame+0xFC (argument 3); 0x0045E9A1: lea 0x1c(%esp),%ecx - frame+0x00 (argument 2); 0x0045E9A7:
    // push %edx; 0x0045E9A8: call 0x004F1A80. Retail does not clean this call's six pushes separately: the `add esp,0x20` at 0x0045E9BC covers them
    // together with the two of 0x00504A30 below.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc18)),
                                append_frame + 0x00,   // 0x0045E9A1: argument 2, the frame base
                                append_frame + 0xFC,   // 0x0045E99A: argument 3, frame+0xFC
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), // 0x0045E98E: argument 4
                                7,                     // 0x0045E98C: argument 5
                                0);                    // 0x0045E98B: argument 6

    // 0x0045E9AD: push 0x00460A70 (argument 2, the criteria handler); 0x0045E9B2: push 0x0053F3F8 (argument 1, "CRITERIA_HUMAN");
    // 0x0045E9B7: call 0x00504A30; 0x0045E9BC: add esp,0x20 (8 dwords: the append's six plus these two); 0x0045E9BF: mov [0x005BDB00],eax.
    ::g_5bdb00 = Script_RegisterCriteria_504A30("CRITERIA_HUMAN", // 0x0045E9B2: the literal at 0x0053F3F8
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x460A70U))); // 0x0045E9AD: the handler immediate

    // 0x0045E9C4: xor %edi,%edi (the loop index and the sprintf vararg); 0x0045E9C6: mov $0x005BDAD8,%ebx (the destination slot);
    // 0x0045E9CB: mov $0x005BDB18,%esi (the string buffer). The test is at the bottom (`jl 0x0045E9D0` at 0x0045E9F9), so the body always runs.
    std::int32_t criteria_index = 0;           // %edi
    std::uint32_t* criteria_slot = ::g_5bdad8; // %ebx
    char* criteria_buffer = ::g_5bdb18;        // %esi
    do
    {
        // 0x0045E9D0: push %edi; 0x0045E9D1: push 0x0053F3E4 ("CRITERIA_HUMAN%d"); 0x0045E9D6: push %esi; 0x0045E9D7: call 0x004A53B3 (sprintf).
        // 0x0045E9DC: push 0x00460A10 (the criteria handler); 0x0045E9E1: push %esi (the name just formatted); 0x0045E9E2: call 0x00504A30;
        // 0x0045E9E7: add esp,0x14 (5 dwords: the sprintf's three plus this call's two); 0x0045E9EA: mov %eax,(%ebx).
        ::sprintf(criteria_buffer, "CRITERIA_HUMAN%d", criteria_index);
        *criteria_slot = Script_RegisterCriteria_504A30(criteria_buffer,
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x460A10U)));
        criteria_buffer += 0x20; // 0x0045E9EC: add $0x20,%esi - the next 32-byte string slot
        criteria_index += 1;     // 0x0045E9EF: inc %edi
        criteria_slot += 1;      // 0x0045E9F0: add $0x4,%ebx
    } while (criteria_buffer < reinterpret_cast<char const*>(&::g_5bdc18)); // 0x0045E9F3/0x0045E9F9: retail compares %esi against 0x005BDC18

    // 0x0045E9FB: xor %edi,%edi (the value, starting at 0); 0x0045E9FD: mov $0x0053F348,%esi (the name-pointer table). The test is at the
    // bottom (`jl 0x0045EA02` at 0x0045EA18); the bound 0x0053F350 is the table base plus two four-byte entries.
    std::int32_t team_value = 0;                // %edi
    char const* const* team_entry = ::g_53f348; // %esi
    do
    {
        // 0x0045EA02: mov (%esi),%eax (the name pointer, argument 1); 0x0045EA04: push %edi (argument 2, the value); 0x0045EA05: push %eax;
        // 0x0045EA06: call 0x004B8810 (Script_QHashNodeRegister); 0x0045EA0B: add esp,0x8.
        // The const_cast is required only because the port's placeholder signature for 0x004B8810 takes `void*`; its whole callee chain reads the
        // name as `char const*` and never writes through it.
        Script_QHashNodeRegister(const_cast<char*>(*team_entry),
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(team_value))));
        team_entry += 1; // 0x0045EA0E: add $0x4,%esi
        team_value += 1; // 0x0045EA11: inc %edi
    } while (team_entry < &::g_53f348[2]); // 0x0045EA12/0x0045EA18: retail compares %esi against 0x0053F350

    // 0x0045EA1A: push 0 (argument 4); 0x0045EA1B: push 0 (argument 3); 0x0045EA1C: push 0x0045EAE0 (argument 2, the handler);
    // 0x0045EA21: push 0x0053F3C8 (argument 1, "Human_GetCutSceneDeltaTime"); 0x0045EA26-0x0045EA3E: five `mov %ebp,<addr>` that clear
    // 0x005BDC28, 0x005BDC2C, 0x005BDC30, 0x005BDC34 and 0x005BDC38 in that retail order; 0x0045EA44: call 0x004B8890.
    ::g_5bdc28 = 0U; // 0x0045EA26
    ::g_5bdc2c = 0U; // 0x0045EA2C
    ::g_5bdc30 = 0U; // 0x0045EA32
    ::g_5bdc34 = 0U; // 0x0045EA38
    ::g_5bdc38 = 0U; // 0x0045EA3E
    Script_QHashNodeRegisterEx(const_cast<char*>("Human_GetCutSceneDeltaTime"), // 0x0045EA21: the literal at 0x0053F3C8
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x45EAE0U)),      // 0x0045EA1C: argument 2
        static_cast<char const*>(nullptr),                                     // 0x0045EA1B: push 0
        static_cast<void*>(nullptr));                                          // 0x0045EA1A: push 0

    // 0x0045EA49: push 0x0053F3BC ("shadow1.tex"); 0x0045EA4E: call 0x00416930; 0x0045EA53: push the next name; 0x0045EA58: mov the previous
    // result to [0x005BDB0C]. Retail pushes the next argument before storing the previous result; the port's call/store order keeps every call's
    // result store after that call and before the next call, which is the observable order.
    ::g_5bdb0c = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_416930("shadow1.tex")));
    ::g_5bdb08 = static_cast<std::uint32_t>( // 0x0045EA53: push 0x0053F3AC ("hole_blood1.tex"); 0x0045EA5D: call; 0x0045EA67: store
        reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_416930("hole_blood1.tex")));
    ::g_5bdb10 = static_cast<std::uint32_t>( // 0x0045EA62: push 0x0053F39C ("hole_blood2.tex"); 0x0045EA6C: call; 0x0045EA76: store
        reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_416930("hole_blood2.tex")));
    ::g_5bdb14 = static_cast<std::uint32_t>( // 0x0045EA71: push 0x0053F38C ("hole_blood3.tex"); 0x0045EA7B: call; 0x0045EA85: store
        reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_416930("hole_blood3.tex")));
    ::g_5bdb04 = static_cast<std::uint32_t>( // 0x0045EA80: push 0x0053F37C ("hole_blood4.tex"); 0x0045EA8A: call; 0x0045EA94: store
        reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_416930("hole_blood4.tex")));
    ::g_5bdad4 = static_cast<std::uint32_t>( // 0x0045EA8F: push 0x0053C148 ("blood1.spr"); 0x0045EA99: call 0x004169D0; 0x0045EAA3: store
        reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_4169D0("blood1.spr")));
    ::g_5bdad0 = static_cast<std::uint32_t>( // 0x0045EA9E: push 0x0053F370 ("blood2.spr"); 0x0045EAA8: call 0x004169D0; 0x0045EAB4: store
        reinterpret_cast<std::uintptr_t>(Sys_LookupOrAppendRecord_4169D0("blood2.spr")));

    // 0x0045EAAD: mov %cx,[0x005BDAF8]; 0x0045EAB9: push %ecx (argument 1); 0x0045EABA: call 0x00489640;
    // 0x0045EABF: add esp,0x30 = 12 dwords = the 0x004B8890 four, the five 0x00416930 calls and the two 0x004169D0 calls;
    // 0x0045EAC2: mov [0x005BDC3C],eax.
    ::g_5bdc3c = AiDetection_RegisterTaskMask_489640(::g_5bdaf8);

    // 0x0045EAC7-0x0045EACA: pop edi/esi/ebp/ebx (the four saved registers); 0x0045EACB: add esp,0xcf8 (the frame); 0x0045EAD1: ret. Nothing sets
    // eax after the label registration, so the return value is whatever 0x00489640 left in it, which is why this transcription returns void.
} // end of Task_DefineHuman

} // extern "C"
