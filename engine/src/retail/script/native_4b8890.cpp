// Native: Script_QHashNodeRegisterEx_4B8890 (placeholder until VERIFIED); retail VA 0x004B8890-0x004B88B2 (35 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442410 8b4c240c 8b542408 6a00 6a00 6a01 50 8b442414 51 52 50 e811220000 83c41c c3); callee 0x4BAAC0 landed, same push shape as 0x4B8810.
// Behavior/CFG: forwards four arguments to 0x4BAAC0 as (arg0, arg1, arg2, arg3, 1, 0, 0) - the last three stack slots are the raw constants retail pushes - and returns nothing, discarding the callee's eax exactly as retail does.
// Status/limits: TRANSCRIBED (static only); the tail constants 1/0/0 and the argument order follow the push sequence literally; runtime UNOBSERVED.
#include <cstdint>

extern "C" void Script_QHashNodeCreate_4BAAC0(void* arg1, void* arg2, char const* arg3, void* arg4,
                                             void* arg5, void* arg6, void* arg7);

extern "C" {

void Script_QHashNodeRegisterEx_4B8890(void* const arg0, void* const arg1, char const* const arg2,
                                       void* const arg3)
{
    Script_QHashNodeCreate_4BAAC0(arg0,                    // 0x004B88A9: push eax ([esp+0x14] = arg0)
                                  arg1,                    // 0x004B88A8: push edx ([esp+8] = arg1)
                                  arg2,                    // 0x004B88A7: push ecx ([esp+0xC] = arg2)
                                  arg3,                    // 0x004B88A2: push eax ([esp+0x10] = arg3)
                                  reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)), // 0x004B88A0: push 1
                                  nullptr,                 // 0x004B889E: push 0
                                  nullptr);                // 0x004B889C: push 0
}

} // extern "C"
