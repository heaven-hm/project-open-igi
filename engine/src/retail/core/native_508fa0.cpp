// Native: AIGraph_Register_508FA0 (placeholder name; retail symbol unknown; the class-name string it registers is "AIGraph" at 0x0054E888). retail VA 0x00508FA0-0x00509292 (755 bytes, 1 block, 0 edges, 196 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 complete byte+instruction listing 0x00508FA0-0x00509292, straight line, no branch); radare2 6.2.0 af/afij at 0x00508FA0 (755 bytes, 1 block, 0 edges, 196 instructions, outdegree 42); the image strings "AIGraph" 0x0054E888, "NODECRITERIA_DOOR" 0x0054E874, "NODECRITERIA_VIEW" 0x0054E860, "NODECRITERIA_STAIR" 0x0054E84C, "RouteTable" 0x0054E840 and the image word 0x0181 at 0x0054E83C; the deferred add esp amounts are 0x38 (0x00508FF9, fourteen words), 0xC after each of the eight getter links (0x00509014 onward), 0x18 after six of the seven appends (0x00509158 onward) and 0xF8 at 0x0050928C, which releases the 0xC0 frame plus the fourteen words still pushed.
// Behavior/CFG: one straight-line block. Registers the task type "AIGraph" with object size 0xC0 and parent Graph_Get_4F9210(), stores the returned word at 0x0054E83C, links eleven handlers (event 0 -> 0x00509570 and the eight getter events through 0x00401400; events 1 -> 0x004D3710 and 2 -> 0x00509610 through 0x00401530), writes the parameter block 0x00A7B2A8-0x00A7B2D8 (four doubles 20.0 / 0.3 / 1.0 / 0.01 at 0x00A7B2C0/B8/B0/A8, a grown container pointer at 0x00A7B2CC, three node-criteria handles at 0x00A7B2D0/D4/D8), appends seven 16-byte records to that container through Script_TaskContainer_Append (ranges 0x80/0x88/0x90/0x98/0xA0/0xBC/0xA8 inside its own 0xC0 frame, camera dwords from 0x004F0F80 six times and 0x004F1000 once, counts 7/5/7/7/7/7/7, sixth argument 0 except 0x00A7B2B8 in the fourth and 0x00A7B2A8 in the seventh), sets the node-size table entry to 0x240 for the class id, registers the three node-criteria names, and registers "RouteTable" with argument 0xA and handler 0x005092A0 through 0x004F9DF0.
// Status/limits: TRANSCRIBED (static only); STRUCTURALLY_VERIFIED only as the r2 one-block/196-instruction count; BUILD_VERIFIED (host and i686 single-file compile); runtime UNOBSERVED. Its caller 0x00508F80 is transcribed (core/native_508f80.cpp) but the chain that reaches it is not; the startup sequence 0x00402930 enters this class registration first there. LIMITS, all recorded from the instructions: (1) 0x004F9DF0 is not transcribed yet, so this file carries that undefined callee and stays link-blocked on it; (2) the nine handler bodies are addresses only (declared, no definitions) - they are other natives of the same 0x00509xxx cluster and are listed with their sizes below; (3) CORRECTED: 0x004F1A80 reads SIX arguments, not five - the byte read at 0x004F1AB1 is arg5 and the dword read at 0x004F1AC9, after the intervening `pop edi`, is arg6 at entry+0x18, and retail stores both (arg6 at +0x08, byte(arg5) at +0x0C); all seven call sites here now pass the sixth word, which is 0 except the fourth (0x00A7B2B8) and the seventh (0x00A7B2A8); (4) retail stores each double as two 32-bit immediates low-dword-first, the transcription stores the same IEEE-754 double once - identical memory image, no observable difference; (5) the two Append pointers are addresses in the registrar's own frame and the callee stores only their difference, so the transcription passes equivalent addresses of a local 0xC0 scratch range; (6) 0x00A7B2D0/D4/D8 are defined by core/native_509860/509870/509880.cpp (the getters that read them) and are written here, declared extern.
#include "dev_log.hpp"
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/core/graph_state.hpp"
#include "igi/retail/render/camera_state.hpp"
#include "igi/retail/script/script_state.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004F9DF0: the graph-stream node registrar this class registers "RouteTable" through, transcribed in
// engine/src/retail/core/native_4f9df0.cpp. Three words: the name, the 0xA value and the handler address, which retail passes as a
// plain dword (the third parameter is a code address, not a typed pointer).
void NATIVE_DECL GraphStream_RegisterNode_4F9DF0(char const* name, std::uint32_t value, std::uint32_t handler);

// The handler bodies this registrar links or hands to 0x004F9DF0, transcribed one file per native in this directory
// (native_5092a0.cpp .. native_509610.cpp). The argument counts are the ones each retail body actually reads; retail stores only
// the code address in the handler table, so the calls below cast the differently-shaped bodies to the table's pointer type.
void NATIVE_DECL AIGraph_Handler_509570(void* object, void* arg2);   // 0x00509570: 151 bytes, 1 block, 28 instructions (context handler)
void NATIVE_DECL AIGraph_Handler_509610(void* object);              // 0x00509610:  59 bytes, 3 blocks, 17 instructions (no-context handler)
void NATIVE_DECL AIGraph_Handler_5093B0(void* object);              // 0x005093B0:  12 bytes, 1 block,  5 instructions
void NATIVE_DECL AIGraph_Handler_509520(void* object, std::uint32_t const* arg2); // 0x00509520: 67 bytes, 3 blocks, 24 instructions
void NATIVE_DECL AIGraph_Handler_509450(void* object, std::uint32_t const* arg2); // 0x00509450: 39 bytes, 1 block, 14 instructions
void NATIVE_DECL AIGraph_Handler_509480(void* task, void* object);  // 0x00509480: 149 bytes, 5 blocks, 58 instructions
void NATIVE_DECL AIGraph_Handler_5093C0(void* object, void* arg2);  // 0x005093C0: 124 bytes, 7 blocks, 35 instructions
void NATIVE_DECL AIGraph_Handler_509440(void* context, void** out); // 0x00509440:  13 bytes, 1 block,  4 instructions
void NATIVE_DECL AIGraph_Handler_5092A0(void* context);             // 0x005092A0:  53 bytes, 3 blocks, 22 instructions (RouteTable handler)

// 0x004D3710: the one-byte QTask_NoOpHandler, transcribed in engine/src/retail/script/native_4d3710.cpp.
void NATIVE_DECL QTask_NoOpHandler();

// Callees already transcribed in engine/src/retail/script/ (no header exists for them yet, so the arguments are declared here
// exactly as those transcriptions take them).
// 0x004F1980 Script_TaskContainer_Grow, 0x004F1A80 Script_TaskContainer_Append,
// 0x004F9460 Script_NodeSizeTable_Set, 0x005186C0 Script_NodeCriteriaRegister.
void* Script_TaskContainer_Grow(std::int32_t* container, std::int32_t count);
void* Script_TaskContainer_Append(std::int32_t* container, std::int32_t arg2, std::int32_t arg3,
                                  std::int32_t arg4, std::int32_t arg5, std::int32_t arg6);
void Script_NodeSizeTable_Set(std::int16_t size, std::int32_t value);
std::int32_t Script_NodeCriteriaRegister(void* value);

// 0x0054E83C: the "AIGraph" class id word written by this registrar (0x00508FC7, the lowest retail write address for the word).
// It is defined by core/native_5098a0.cpp - the getter native 0x005098A0 that reads it - because that file already owns the
// symbol; this registrar is the writer, so the definition is extern here. Retail image value 0x0181 (file bytes 81 01 at
// 0x0054E83C), which the owner's initialiser must carry.
extern std::uint16_t g_54e83c;

// 0x00A7B2A8/0x00A7B2B0/0x00A7B2B8/0x00A7B2C0: the four parameter doubles this registrar writes (0x005090D1-0x00509123). Retail
// stores each as two 32-bit immediates, low dword first: 0x00A7B2C0 = {0x00000000, 0x40340000} = 20.0, 0x00A7B2B8 = {0x33333333,
// 0x3FD33333} = 0.3, 0x00A7B2B0 = {0x00000000, 0x3FF00000} = 1.0, 0x00A7B2A8 = {0x47AE147B, 0x3F847AE1} = 0.01. These words sit
// in .data's zero-fill tail (no file bytes), so the definition starts zeroed like the loader leaves them.
double g_a7b2a8 = 0.0;
double g_a7b2b0 = 0.0;
double g_a7b2b8 = 0.0;
double g_a7b2c0 = 0.0;

// 0x00A7B2CC: the grown parameter container (0x00509131, the result of 0x004F1980).
void* g_a7b2cc = nullptr;

// 0x00A7B2D0/0x00A7B2D4/0x00A7B2D8: the three node-criteria handles (0x0050925D, 0x0050926C, 0x00509282). Defined by the three
// getter natives that read them - core/native_509860.cpp, core/native_509870.cpp, core/native_509880.cpp - so they are extern here.
extern std::uint32_t g_a7b2d0;
extern std::uint32_t g_a7b2d4;
extern std::uint32_t g_a7b2d8;

// The AI-graph class registrar: 0x00508F80 reaches it by tail jump and no other call site exists.
void NATIVE_DECL AIGraph_Register_508FA0()
{
    NATIVE_LOG_ENTER("AIGraph_Register_508FA0", "0x00508FA0");
    // 0x00508FA0: `sub $0xC0,%esp`. The frame is the parameter range the seven appends below describe; retail never writes it.
    std::uint8_t parameter_range[0xC0];

    // 0x00508FA6-0x00508FAA: `push $0x0; push $0x0; call 0x004F9210` - the two zero words are deferred argument words (the getter
    // takes none) and the first of them is later reused as the task-list argument of the registration below.
    const std::uint16_t parent = Graph_Get_4F9210();

    // 0x00508FAF-0x00508FBA: `push %eax; push $0xC0; push $0x0054E888; call 0x00401900` -
    // TaskType_Register("AIGraph", 0xC0, parent, 0). The fourth word is the leftover zero from the getter's argument pair.
    g_54e83c = TaskType_Register("AIGraph", 0xC0U, parent, 0U);

    // 0x00508FBF-0x00508FCD: `push $0x00509570; push $0x0; push %eax; mov %ax,0x0054E83C; call 0x00401400` - event 0 -> 0x00509570.
    QTask_LinkQTaskEventHandler(
        g_54e83c, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<std::uintptr_t>(&AIGraph_Handler_509570)));

    // 0x00508FD2-0x00508FE0: `mov 0x0054E83C,%ax; push $0x004D3710; push $0x1; push %eax; call 0x00401530` - event 1 -> NoOp.
    QTask_LinkQTaskEventHandlerNoContextFn(
        g_54e83c, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler));

    // 0x00508FE5-0x00508FF4: `mov 0x0054E83C,%cx; push $0x00509610; push $0x2; push %ecx; call 0x00401530` - event 2 -> 0x00509610.
    QTask_LinkQTaskEventHandlerNoContextFn(
        g_54e83c, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<std::uintptr_t>(&AIGraph_Handler_509610)));
    // 0x00508FF9: `add $0x38,%esp` - the fourteen pushed words from the prologue to here (compiler-generated in C++).

    // 0x00508FFC-0x00509014: `push $0x005093B0; call 0x004F9280; mov 0x0054E83C,%dx; push %eax; push %edx; call 0x00401400` -
    // event = 0x004F9280() -> 0x005093B0.
    QTask_LinkQTaskEventHandler(
        g_54e83c, Graph_Get_4F9280(),
        reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<std::uintptr_t>(&AIGraph_Handler_5093B0)));

    // 0x00509017-0x0050902E: `push $0x00509520; call 0x004F9230; ...` - event = 0x004F9230() -> 0x00509520.
    QTask_LinkQTaskEventHandler(
        g_54e83c, Graph_Get_4F9230(),
        reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<std::uintptr_t>(&AIGraph_Handler_509520)));

    // 0x00509031-0x00509049: `push $0x004D3710; call 0x004F9250; ...` - event = 0x004F9250() -> NoOp.
    QTask_LinkQTaskEventHandler(
        g_54e83c, Graph_Get_4F9250(), reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler));

    // 0x0050904C-0x00509064: `push $0x004D3710; call 0x004F9260; ...` - event = 0x004F9260() -> NoOp.
    QTask_LinkQTaskEventHandler(
        g_54e83c, Graph_Get_4F9260(), reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler));

    // 0x00509067-0x0050907E: `push $0x00509450; call 0x004F9220; ...` - event = 0x004F9220() -> 0x00509450.
    QTask_LinkQTaskEventHandler(
        g_54e83c, Graph_Get_4F9220(),
        reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<std::uintptr_t>(&AIGraph_Handler_509450)));

    // 0x00509081-0x00509099: `push $0x00509480; call 0x004F9240; ...` - event = 0x004F9240() -> 0x00509480.
    QTask_LinkQTaskEventHandler(
        g_54e83c, Graph_Get_4F9240(),
        reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<std::uintptr_t>(&AIGraph_Handler_509480)));

    // 0x0050909C-0x005090B4: `push $0x005093C0; call 0x004F1A70; ...` - event = 0x004F1A70() -> 0x005093C0.
    QTask_LinkQTaskEventHandler(
        g_54e83c, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<std::uintptr_t>(&AIGraph_Handler_5093C0)));

    // 0x005090B7-0x005090CE: `push $0x00509440; call 0x004F1A60; ...` - event = 0x004F1A60() -> 0x00509440.
    QTask_LinkQTaskEventHandler(
        g_54e83c, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(reinterpret_cast<std::uintptr_t>(&AIGraph_Handler_509440)));

    // 0x005090D1-0x00509123: the parameter doubles, in retail's store order (0x00A7B2C0 first, then 0xB8, 0xB0, 0xA8). Each retail
    // store is one 32-bit immediate; the pair per value is recorded in the definition comments above.
    g_a7b2c0 = 20.0;  // 0x005090D1 (lo 0) / 0x005090DD (hi 0x40340000)
    g_a7b2b8 = 0.3;   // 0x005090E7 (lo 0x33333333) / 0x005090F1 (hi 0x3FD33333)
    g_a7b2b0 = 1.0;   // 0x005090FB (lo 0) / 0x00509105 (hi 0x3FF00000)
    g_a7b2a8 = 0.01;  // 0x0050910F (lo 0x47AE147B) / 0x00509119 (hi 0x3F847AE1)

    // 0x00509123-0x00509131: `call 0x004F9200; push %eax; call 0x004F1980; add $0x8,%esp; mov %eax,0x00A7B2CC` - grows the graph task
    // container (the dword 0x004F9200 returns) by seven entries and keeps the new container pointer.
    g_a7b2cc = Script_TaskContainer_Grow(
        reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(Graph_Get_4F9200())), 7);

    // The seven appends: 0x00509136, 0x0050915B, 0x00509180, 0x005091A5, 0x005091CD, 0x005091F2, 0x00509217. Retail passes the
    // registrar's own frame as the two range words (frame base and frame base + 0x80/0x88/0x90/0x98/0xA0/0xBC/0xA8); the callee
    // stores only the difference, so a local 0xC0 range reproduces the same range values. The words pushed before each getter are
    // the fifth and sixth argument words: count 7 (5 in the second) and 0, or 0x00A7B2B8 / 0x00A7B2A8 in the fourth and seventh.
    // 0x004F1A80 DOES read the sixth word: after `push ebx; push esi; push edi` it takes byte [esp+0x20] = arg5 (a byte), and after
    // the `pop edi` at 0x004F1ABD the same textual offset is entry+0x18, so the dword read at 0x004F1AC9 is arg6, stored at +0x08.
    // Both argument words are therefore passed below; the earlier note here that the sixth was unread was wrong.
    const std::int32_t parameter_base =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&parameter_range[0]));

    // 0x00509136-0x00509158: `push $0x0; push $0x7; call 0x004F0F80; lea 0x88(%esp)/0xC(%esp); call 0x004F1A80; add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(g_a7b2cc), parameter_base,
                               parameter_base + 0x80, static_cast<std::int32_t>(Camera_Get_4F0F80()), 7, 0);

    // 0x0050915B-0x0050917D: count 5 instead of 7. `push $0x0; push $0x5; call 0x004F0F80`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(g_a7b2cc), parameter_base,
                               parameter_base + 0x88, static_cast<std::int32_t>(Camera_Get_4F0F80()), 5, 0);

    // 0x00509180-0x005091A2: `push $0x0; push $0x7; call 0x004F0F80`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(g_a7b2cc), parameter_base,
                               parameter_base + 0x90, static_cast<std::int32_t>(Camera_Get_4F0F80()), 7, 0);

    // 0x005091A5-0x005091CA: `push $0x00A7B2B8; push $0x7; call 0x004F0F80` - the sixth word is the address of the 0.3 double.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(g_a7b2cc), parameter_base,
                               parameter_base + 0x98, static_cast<std::int32_t>(Camera_Get_4F0F80()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&g_a7b2b8)));

    // 0x005091CD-0x005091EF: `push $0x0; push $0x7; call 0x004F0F80`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(g_a7b2cc), parameter_base,
                               parameter_base + 0xA0, static_cast<std::int32_t>(Camera_Get_4F0F80()), 7, 0);

    // 0x005091F2-0x00509214: `push $0x0; push $0x7; call 0x004F1000` - the only append fed by the second camera dword.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(g_a7b2cc), parameter_base,
                               parameter_base + 0xBC, static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x00509217-0x0050923C: `push $0x00A7B2A8; push $0x7; call 0x004F0F80` - the sixth word is the address of the 0.01 double. Retail
    // leaves these six words pushed (no add of its own); the final add esp,0xF8 releases them.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(g_a7b2cc), parameter_base,
                               parameter_base + 0xA8, static_cast<std::int32_t>(Camera_Get_4F0F80()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&g_a7b2a8)));

    // 0x0050923C-0x00509249: `mov 0x0054E83C,%cx; push $0x240; push %ecx; call 0x004F9460` -
    // Script_NodeSizeTable_Set(class id, 0x240).
    Script_NodeSizeTable_Set(static_cast<std::int16_t>(g_54e83c), 0x240);

    // 0x0050924E-0x0050925D: `push $0x0054E874; call 0x005186C0; push $0x0054E860; mov %eax,0x00A7B2D0` - registers
    // "NODECRITERIA_DOOR" and keeps its handle while the next name is pushed (retail's store-before-next-call order).
    g_a7b2d0 = static_cast<std::uint32_t>(Script_NodeCriteriaRegister(const_cast<char*>("NODECRITERIA_DOOR")));

    // 0x00509262-0x0050926C: registers "NODECRITERIA_VIEW" into 0x00A7B2D4.
    g_a7b2d4 = static_cast<std::uint32_t>(Script_NodeCriteriaRegister(const_cast<char*>("NODECRITERIA_VIEW")));

    // 0x00509271-0x00509282: registers "NODECRITERIA_STAIR" into 0x00A7B2D8.
    g_a7b2d8 = static_cast<std::uint32_t>(Script_NodeCriteriaRegister(const_cast<char*>("NODECRITERIA_STAIR")));

    // 0x00509276-0x00509287: `push $0x005092A0; push $0xA; push $0x0054E840; call 0x004F9DF0` -
    // registers "RouteTable" with argument 0xA and handler 0x005092A0.
    GraphStream_RegisterNode_4F9DF0(
        "RouteTable", 0xA,
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&AIGraph_Handler_5092A0)));

    // 0x0050928C: `add $0xF8,%esp` (0xC0 frame + 0x38 deferred words); 0x00509292: `ret`.
}

} // extern "C"

#undef NATIVE_DECL
