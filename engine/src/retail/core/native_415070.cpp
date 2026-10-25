// Native: Task_DefineIntroScreen_415070 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00415070-0x004150DD (110 bytes, 1 block, 0 edges, 30 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) bytes 6a00 6a00 e877c8feff 50 6a38 68cc945300 e87ac8feff 68e0504100 6a00 50 66a398b15700 e867c3feff 66a198b15700 6880534100 6a01 50 e884c4feff 668b0d98b15700 6850544100 6a02 51 e870c4feff 68b4945300 68a4945300 e821c20900 83c440 68b4945300 e8f4170d00 59 c3; radare2 6.2.0 af/afij (110 B, 1 block, 30 instructions, 0 edges); the three literals read byte-for-byte at 0x005394CC ("IntroScreen"), 0x005394A4 ("INTROSCREEN:") and 0x005394B4 ("LOCAL:screens/intro/"); the guard's call site 0x0040296C calls it with no arguments.
// Behavior/CFG: one basic block, no branch of its own. Registers the "IntroScreen" task type through 0x00401900 (object size 0x38, parent from the 0x004018F0 getter, task-list argument 0), stores the returned 16-bit id at 0x0057B198, links handler 0x004150E0 to event 0 through the context variant 0x00401400 and handlers 0x00415380 and 0x00415450 to events 1 and 2 through the no-context variant 0x00401530, registers the file alias "INTROSCREEN:" -> "LOCAL:screens/intro/" through 0x004B12F0, and returns the result of the directory dispatch 0x004E68D0 on that same path.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 30-instruction listing and afij (110 B / 1 block / 0 edges); runtime UNOBSERVED. 0x004E68D0 is a NATIVE GAP declared here, not transcribed: it is a thin wrapper whose body is `QFile_DispatchDevicePath_4B1420(path, 0x004E68F0, 0)` (its own callback 0x004E68F0 and a zero context), so this file cannot link until that native lands. The id slot 0x0057B198 is declared here with its image value 0 because no other file defines it; retail re-loads the word from memory for each of the three link calls. The three handler addresses 0x004150E0, 0x00415380 and 0x00415450 are raw code immediates in retail and are not transcribed yet. Retail never cleans the individual calls: the single `add $0x40,%esp` at 0x004150CF covers all sixteen dwords this routine pushes (2 + 3 + 3 + 3 + 3 + 2), and the compiler's own argument cleanups replace that one late add.
#include <cstdint>

#include "igi/retail/core/task_types.hpp"

// task_types.hpp defines NATIVE_DECL for its own declarations and #undefs it at its end (line 209), so this native restates the
// same guarded definition before its own use, exactly as the tree's other native files do.
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" void QFile_RegisterAlias_4B12F0(const char* key, const char* target); // 0x004B12F0 (qfile/native_4b12f0.cpp)

// 0x004E68D0: NATIVE GAP - not transcribed. Its body is `mov 0x4(%esp),%eax; push 0; push 0x4E68F0; push %eax; call 0x4B1420;
// add $0xc,%esp; ret`, i.e. the transcribed directory dispatcher 0x004B1420 with its own callback 0x004E68F0 and a zero context.
extern "C" std::int32_t QFile_DispatchDirectory_4E68D0(const char* path);

extern "C" {

// Retail 0x0057B198: the 16-bit task type id this registrar stores (image value 0).
std::uint16_t g_57b198 = 0;

std::int32_t NATIVE_DECL Task_DefineIntroScreen_415070()
{
    // 0x00415070/0x00415072: push $0x0; push $0x0. The getter 0x004018F0 takes no arguments, so these two zeros are the getter's
    // inert arguments and, at the 0x00401900 call below, its third and fourth slots: retail never cleans them here.
    const std::uint16_t class_id = TaskType_Register("IntroScreen",       // 0x0041507C: push 0x005394CC
                                                     0x38U,               // 0x0041507A: push 0x38 (object size)
                                                     QTask_GetCurrentType(), // 0x00415074: call 0x004018F0 (parent type)
                                                     0U);                 // 0x00415072: push 0 (the second of the two zeros)

    // 0x00415086: push 0x004150E0 (pre-pushed third argument); 0x0041508B: push 0; 0x0041508D: push eax;
    // 0x0041508E: mov %ax,0x57B198 (the store sits between the pushes and the call); 0x00415094: call 0x00401400.
    g_57b198 = class_id;
    QTask_LinkQTaskEventHandler(g_57b198,                                 // 0x00415099: mov 0x57B198,%ax (retail re-loads the word)
                                0U,                                   // 0x0041508B: push 0 (event id)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004150E0U))); // 0x00415086: push 0x004150E0 (handler)

    // 0x0041509F: push 0x00415380 (pre-pushed third argument); 0x004150A4: push 1; 0x004150A6: push eax;
    // 0x004150AC: mov 0x57B198,%cx; 0x004150A7: call 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_57b198,                      // 0x004150AC: mov 0x57B198,%cx
                                           1U,                          // 0x004150A4: push 1 (event id)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00415380U))); // 0x0041509F: push 0x00415380

    // 0x004150B3: push 0x00415450 (pre-pushed third argument); 0x004150B8: push 2; 0x004150BA: push ecx;
    // 0x004150BB: call 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_57b198,                      // 0x004150BA: push %ecx (the word re-loaded at 0x004150AC)
                                           2U,                          // 0x004150B8: push 2 (event id)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00415450U))); // 0x004150B3: push 0x00415450

    // 0x004150C0: push "LOCAL:screens/intro/" (argument 2); 0x004150C5: push "INTROSCREEN:" (argument 1, pushed last);
    // 0x004150CA: call 0x004B12F0.
    QFile_RegisterAlias_4B12F0("INTROSCREEN:",                            // 0x004150C5: push 0x005394A4 (the key)
                               "LOCAL:screens/intro/");                   // 0x004150C0: push 0x005394B4 (the target)

    // 0x004150CF: add $0x40,%esp (the late cleanup of all sixteen dwords above); 0x004150D2: push 0x005394B4;
    // 0x004150D7: call 0x004E68D0; 0x004150DC: pop %ecx; 0x004150DD: ret. The return value is that callee's eax.
    return QFile_DispatchDirectory_4E68D0("LOCAL:screens/intro/");        // 0x004150D2: push 0x005394B4
}

} // extern "C"
