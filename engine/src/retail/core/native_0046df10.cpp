// Native: Task_DefineKnifeStab (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0046DF10-0x0046DF60.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 81 bytes, 1 block, 22 instructions.
// Tools/evidence: radare2 6.2.0 per-instruction listing of the complete single-block body (one ret, so the 22-instruction count is exact); callee bodies re-read at the exact VAs to fix the stack-argument layout - 0x004F45F0 is `mov ax,[0x54E200]; ret` (reads no arguments), 0x00413B20 is `mov al,[0x5391AE]; ret` (reads no arguments), and 0x00401400 reads [esp+4]/[esp+8]/[esp+0xC] as (class id, event id, handler) after its prologue, which is what makes the pre-pushed handler slot at 0x0046DF2F a call argument rather than padding. Literals 0x0053FE9C "KnifeStab" and 0x0053FE88 "TASKTYPE_KNIFESTAB" read as raw .rdata bytes.
// Behavior/CFG: registers the "KnifeStab" task type with TaskType_Register("KnifeStab", 0x20, <getter>, 0), stores the returned 16-bit id at 0x0053FE84, then pre-pushes the handler 0x0046DF70 once and calls the flag getter 0x00413B20 (which ignores the stack slot), and links the stored id together with that flag and handler through QTask_LinkQTaskEventHandler(id, flag, 0x0046DF70); finally registers the script name "TASKTYPE_KNIFESTAB" for the id through Script_QHashNodeRegister. Both epilogue cleanups are `add esp,0x14` (5 dwords) and both push groups measure 5 dwords: the first is 2 shared slots + 3 TaskType_Register arguments, the second is the shared handler slot + 2 link arguments + 2 script-registration arguments.
// Status/limits: BUILD_VERIFIED; the one zero pushed at 0x0046DF10 is an inert stack slot below the arguments (nothing reads it) and the zero at 0x0046DF12 is TaskType_Register's fourth argument, so the call reproduces the four arguments the callee reads. Handler 0x0046DF70 is a raw code immediate in retail and is not transcribed yet (native gap). Runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4F45F0(); // 0x004F45F0: word getter, takes no arguments
extern "C" std::uint8_t Core_GetFlagByte_413B20(); // 0x00413B20: byte getter, takes no arguments

extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (defined extern "C" in script/native_4b8810.cpp)

extern "C" {

std::uint16_t g_53fe84 = 0U; // retail 0x0053FE84: the "KnifeStab" task type id written here

void Task_DefineKnifeStab()
{
    // 0x0046DF10: push 0 (inert slot below the arguments); 0x0046DF12: push 0 (the fourth TaskType_Register argument);
    // 0x0046DF14: call 0x4F45F0; 0x0046DF19: push eax; 0x0046DF1A: push 0x20; 0x0046DF1C: push "KnifeStab"; 0x0046DF21: call 0x401900.
    g_53fe84 = TaskType_Register("KnifeStab",              // 0x0046DF1C: push 0x0053FE9C
                                 0x20U,                    // 0x0046DF1A: push 0x20 (object size)
                                 Sys_GetGlobal16_4F45F0(), // 0x0046DF14: call 0x004F45F0 (parent type)
                                 0U);                      // 0x0046DF12: push 0 (task list count)
    // 0x0046DF26: add esp,0x14; 0x0046DF29: mov word [0x53FE84],ax.

    const std::uint32_t class_id = static_cast<std::uint32_t>(g_53fe84); // 0x0046DF3A/0x0046DF46/0x0046DF4C: load word, mask 0xFFFF
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(class_id),     // 0x0046DF40: push eax (argument 1)
                                Core_GetFlagByte_413B20(),                // 0x0046DF39: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046DF70U))); // 0x0046DF2F: push handler (argument 3)
    // 0x0046DF52: push ecx (id masked 0xFFFF); 0x0046DF53: push 0x0053FE88; 0x0046DF58: call 0x4B8810; 0x0046DF5D: add esp,0x14.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_KNIFESTAB"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(class_id)));
    // 0x0046DF60: ret.
}

} // extern "C"
