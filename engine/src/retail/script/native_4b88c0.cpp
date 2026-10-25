// Native: Script_QHashNodeRegisterMode1_4B88C0 (placeholder until VERIFIED); retail VA 0x004B88C0-0x004B88E2 (35 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) bytes 8b442410 8b4c240c 8b542408 6a00 6a01 6a01 50 8b442414 51 52 50 e8e1210000 83c41c c3; radare2 6.2.0 af/afij at 0x004B88C0 (size 35, 1 block, 14 instructions, 0 edges); diffed against its transcribed siblings 0x004B8890 (engine/src/retail/script/native_4b8890.cpp, which pushes 0 in the sixth slot) and 0x004B88F0 (engine/src/retail/script/native_4b88f0.cpp, which pushes 2): all three are the same 35-byte body and differ only in that one constant. This file exists because it is the only callee of 0x0044CCA0 that had no implementation anywhere in the tree - the ten registrations at 0x0044CDD1-0x0044CE7A go through it.
// Behavior/CFG: one basic block, no branch. It loads its four arguments ([esp+0x10], [esp+0xC], [esp+8], [esp+4] before the pushes) and forwards them to 0x004BAAC0 as (arg0, arg1, arg2, arg3, 1, 1, 0) - the last three slots are the raw constants retail pushes - then cleans 0x1C and returns the callee's eax untouched.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 14-instruction listing and afij (35 B / 1 block / 0 edges); runtime UNOBSERVED. The name is an address-suffixed placeholder: the only retail fact about the 1 in the sixth slot is its position, and no evidence yet says what that slot selects - the three siblings push 0, 1 and 2 there, so the value is load-bearing somewhere.
#include <cstdint>

extern "C" void Script_QHashNodeCreate(char const* arg1, void* arg2, void const* arg3, void* arg4,
                                       void* arg5, void* arg6, void* arg7);

extern "C" {

void Script_QHashNodeRegisterMode1_4B88C0(void* const arg0, void* const arg1, char const* const arg2,
                                          void* const arg3)
{
    Script_QHashNodeCreate(static_cast<char const*>(arg0),                    // 0x004B88D9: push eax ([esp+0x14] = arg0)
                           arg1,                    // 0x004B88D8: push edx ([esp+8] = arg1)
                           arg2,                    // 0x004B88D7: push ecx ([esp+0xC] = arg2)
                           arg3,                    // 0x004B88D2: push eax ([esp+0x10] = arg3)
                           reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)), // 0x004B88D0: push 1
                           reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)), // 0x004B88CE: push 1 (0x004B8890 pushes 0, 0x004B88F0 pushes 2)
                           nullptr);                // 0x004B88CC: push 0
}

} // extern "C"
