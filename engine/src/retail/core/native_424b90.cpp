// Native: Cursor_Init_424B90; retail VA 0x00424B90 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00424B90-0x00424C08 (121 bytes, 1 block, 0 edges, 33 instructions).
// Tools/evidence: radare2 6.2.0 af/pd over the exact bytes; called with no arguments from the startup sequence at 0x00402BC9 (return address 0x00402BCE).
// Behavior/CFG: parent = QTask_GetCurrentType(); registers "Cursor" with object size 0x40 and task-list argument 0, stores the returned class id at 0x0057BC60, links phase 0 -> 0x00424C10 through 0x00401400 and phases 1 -> 0x00424CE0 and 2 -> 0x004D3710 through 0x00401530; the `add esp,0x38` at 0x00424BE0 clears the fourteen deferred argument words pushed so far. It then pushes the handler word 0x00424C40, reads the event word through 0x004E7530 (the already-pushed word is only the following link's third argument; the getter reads no stack argument) and links phase = event word -> 0x00424C40, registers the window-procedure handler 0x00424CA0 through 0x0048F490 and clears the remaining 0x10 bytes, returning whatever 0x0048F490 leaves in EAX. Single block, no branch.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. The five handler words are pushed as retail-VA literals, matching the precedent of native_005310e0.cpp: two of them have no transcribed engine native yet (0x00424C40 is blocked on the untranscribed leaf 0x004B4E00 and 0x00424CE0 exists only as the scripts/GameNatives reference candidate Cursor_RunHandler), so the two literals cannot be replaced by symbols. 0x00424C10 and 0x00424CA0 are transcribed in native_424c10.cpp and native_424ca0.cpp and can replace their literals once they join the build. The class-id word 0x0057BC60 lies beyond the .data raw bytes (raw .data ends at VA 0x00552000) and is zero-initialised.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0057BC60: Cursor class id, written here. No other translation unit owns it.
std::uint16_t g_57bc60 = 0U;

// 0x004E7530, transcribed in engine/src/retail/core/native_4e7530.cpp: returns the word at 0x005488C8.
std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530();

// 0x0048F490, transcribed in engine/src/retail/core/native_48f490.cpp: takes the handler word, returns the chain insert result in EAX.
std::int32_t* NATIVE_DECL WinProc_RegisterHandler(std::int32_t* handler);

std::int32_t* NATIVE_DECL Cursor_Init_424B90()
{
    std::uint16_t const parent = QTask_GetCurrentType();                       // 0x00424B94: call 0x004018F0
    g_57bc60 = TaskType_Register("Cursor", 0x40U, parent, 0U);                 // 0x00424BA1: call 0x00401900
    QTask_LinkQTaskEventHandler(g_57bc60, 0U,                                  // 0x00424BB4: call 0x00401400
                                reinterpret_cast<igi::retail::core::FrameHandler>(0x00424C10U));
    QTask_LinkQTaskEventHandlerNoContextFn(g_57bc60, 1U,                       // 0x00424BC7: call 0x00401530
                                           reinterpret_cast<igi::retail::core::FrameHandler>(0x00424CE0U));
    QTask_LinkQTaskEventHandlerNoContextFn(g_57bc60, 2U,                       // 0x00424BDB: call 0x00401530
                                           reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710U));
    // 0x00424BE0: add esp,0x38 - clears the fourteen deferred argument words of the calls above.

    igi::retail::core::FrameHandler const event_handler =
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00424C40U);        // 0x00424BE3: push $0x00424C40
    std::uint32_t const event_word = Core_GetEventWord_4E7530();               // 0x00424BE8: call 0x004E7530
    QTask_LinkQTaskEventHandlerNoContextFn(g_57bc60, event_word, event_handler); // 0x00424BF6: call 0x00401530

    std::int32_t* const result = WinProc_RegisterHandler(                      // 0x00424C00: call 0x0048F490
        reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(0x00424CA0U)));
    return result;                                                             // 0x00424C05: add esp,0x10; 0x00424C08: ret
}

} // extern "C"

#undef NATIVE_DECL
