// Native: BackgroundFX_Init_419760 (placeholder name; retail symbol unknown, class name string "BackgroundFX" is the only evidence). retail VA 0x00419760 (0x00419760-0x004197E8, 137 bytes, 1 block, 0 edges, 38 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 complete byte+instruction listing 0x00419760-0x004197E8: 6a 00 6a 00 e8 87 81 fe ff 50 6a 24 68 18 9c 53 00 e8 ... 66 a3 f0 9a 53 00 ... 83 c4 20 ... 83 c4 24 ... 83 c4 0c c3); radare2 6.2.0 af/afij (size 137, 1 block, 0 edges, 38 instructions); called from the 0x00402C40 startup sequence (table entry #153 in docs/fidelity/evidence/startup-402930-call-sequence.md); the class name bytes are the image bytes at 0x00539C18 ("BackgroundFX\0").
// Behavior/CFG: parent = QTask_GetCurrentType(); registers "BackgroundFX" with object size 0x24 and task-list argument 0, stores the returned 16-bit class id at 0x00539AF0; links event 1 -> 0x00419800 through 0x00401530; links event = 0x004E7530() -> 0x00419820 through 0x00401530; links event 2 -> 0x004D3710 (QTask_NoOpHandler) through 0x00401530; links phase 0 -> 0x004197F0 through 0x00401400; links phase = 0x004F1A70() -> 0x004D3710 through 0x00401400. The deferred `add esp` amounts (0x20 at 0x00419789, 0x24 at 0x004197CB, 0xC at 0x004197E5) clear the fourteen pushed words, exactly as in the sibling registrar 0x00424B90. Single block, no branch.
// Status/limits: BUILD_VERIFIED (static only); runtime UNOBSERVED. 0x00539AF0 is defined here with the retail image value 0x0181 (file bytes 81 01 at 0x00539AF0); this is the lowest retail write address for that word (0x0041977E), the teardown wrapper 0x004199B0 rewrites it later. The 0x004F1A70 getter returns only AL; its upper bits in retail are stale, and the pushed word therefore carries garbage above bit 7 - that is unobservable because 0x00401400 masks the event word to its low byte (recorded in task_types.hpp), so this transcription zero-extends the byte. Two link targets are transcribed and referenced by symbol (0x00419800, 0x00419820, 0x004197F0); 0x004D3710 is the one-byte QTask_NoOpHandler. The class name string is the retail bytes at 0x00539C18.
#include "dev_log.hpp"
#include "igi/retail/core/task_types.hpp"
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

// 0x00539AF0: BackgroundFX class id word written by this registrar (0x0041977E). Retail image value 0x0181; no other
// translation unit owns it.
std::uint16_t g_539af0 = 0x0181U;

// 0x004E7530, transcribed in engine/src/retail/core/native_4e7530.cpp with C linkage: returns the event word.
std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530();

// Handler 0x004D3710, transcribed in engine/src/retail/script/native_4d3710.cpp. Declared here because the two link
// natives take it as a function word.
void NATIVE_DECL QTask_NoOpHandler();

// The three BackgroundFX handlers this registrar links, transcribed in the same wave (core/native_419800.cpp,
// core/native_419820.cpp, core/native_4197f0.cpp). Declared here because the link natives take them as handler words.
void NATIVE_DECL BackgroundFX_AdvanceField20_419800(void* object);
void NATIVE_DECL BackgroundFX_UpdateRecord_419820(void* record);
void NATIVE_DECL BackgroundFX_ZeroField20_4197F0(void* object);

// 0x00402C40 calls this registrar with no arguments; it returns nothing.
void NATIVE_DECL BackgroundFX_Init_419760()
{
    NATIVE_LOG_ENTER("BackgroundFX_Init_419760", "0x00419760");
    // 0x00419760-0x00419764: `push $0x0; push $0x0; call 0x004018F0` - the two zero words are deferred argument words
    // for the calls below; the getter itself takes no argument.
    const std::uint16_t parent = QTask_GetCurrentType();

    // 0x00419769-0x00419771: `push %eax; push $0x24; push $0x00539C18; call 0x00401900` -
    // TaskType_Register("BackgroundFX", 0x24, parent, 0).
    g_539af0 = TaskType_Register("BackgroundFX", 0x24U, parent, 0U);

    // 0x00419776-0x00419784: `push $0x00419800; push $0x1; push %eax; mov %ax,0x00539AF0; call 0x00401530`.
    QTask_LinkQTaskEventHandlerNoContextFn(
        g_539af0, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(&BackgroundFX_AdvanceField20_419800));
    // 0x00419789: `add $0x20,%esp` - clears the words pushed so far.

    // 0x0041978C-0x00419791: `push $0x00419820; call 0x004E7530` - the event word for the second link.
    igi::retail::core::FrameHandler const record_handler =
        reinterpret_cast<igi::retail::core::FrameHandler>(&BackgroundFX_UpdateRecord_419820);
    const std::uint32_t event_word = Core_GetEventWord_4E7530();                // 0x004E7530, transcribed in core/

    // 0x00419796-0x0041979E: `push %eax; mov 0x00539AF0,%ax; push %eax; call 0x00401530`.
    QTask_LinkQTaskEventHandlerNoContextFn(g_539af0, event_word, record_handler);

    // 0x004197A3-0x004197B2: `mov 0x00539AF0,%cx; push $0x004D3710; push $0x2; push %ecx; call 0x00401530`.
    QTask_LinkQTaskEventHandlerNoContextFn(
        g_539af0, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler));

    // 0x004197B7-0x004197C6: `mov 0x00539AF0,%dx; push $0x004197F0; push $0x0; push %edx; call 0x00401400`.
    QTask_LinkQTaskEventHandler(
        g_539af0, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(&BackgroundFX_ZeroField20_4197F0));
    // 0x004197CB: `add $0x24,%esp`.

    // 0x004197CE-0x004197D3: `push $0x004D3710; call 0x004F1A70` - the phase byte for the last link.
    const std::uint32_t noop_phase = Script_PhaseIdGetE5_4F1A70();

    // 0x004197D8-0x004197E0: `push %eax; mov 0x00539AF0,%ax; push %eax; call 0x00401400`.
    QTask_LinkQTaskEventHandler(
        g_539af0, noop_phase, reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler));
    // 0x004197E5: `add $0xc,%esp`; 0x004197E8: `ret`.
}

} // extern "C"

#undef NATIVE_DECL
