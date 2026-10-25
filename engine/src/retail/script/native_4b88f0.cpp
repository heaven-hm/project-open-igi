// Native: Script_QHashNodeRegisterMode2_4B88F0 (placeholder until VERIFIED); retail VA 0x004B88F0-0x004B8912 (35 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) bytes 8b442410 8b4c240c 8b542408 6a00 6a02 6a01 50 8b442414 51 52 50 e8b1210000 83c41c c3; radare2 6.2.0 af/afij at 0x004B88F0 (size 35, 1 block, 14 instructions, 0 edges); diffed against its transcribed sibling 0x004B8890 (engine/src/retail/script/native_4b8890.cpp), whose 14 instructions and byte layout are identical except that this one pushes 2 where the sibling pushes 0.
// Behavior/CFG: one basic block, no branch. It loads its four arguments ([esp+0x10], [esp+0xC], [esp+8], [esp+4] before the pushes) and forwards them to 0x004BAAC0 as (arg0, arg1, arg2, arg3, 1, 2, 0) - the last three slots are the raw constants retail pushes - then cleans 0x1C and returns the callee's eax untouched.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 14-instruction listing and afij (35 B / 1 block / 0 edges); runtime UNOBSERVED. The name is an address-suffixed placeholder: the only retail fact about the 2 is its slot position (the sixth argument of 0x004BAAC0), and no evidence yet says what that slot selects. It exists because 0x00402FF0 registers exactly one entry point through it ("Config_GraphicOptionsSetGamma"); the call site discards the return value.
#include <cstdint>

extern "C" void Script_QHashNodeCreate(void* arg1, void* arg2, char const* arg3, void* arg4,
                                       void* arg5, void* arg6, void* arg7);

extern "C" {

void Script_QHashNodeRegisterMode2_4B88F0(void* const arg0, void* const arg1, char const* const arg2,
                                          void* const arg3)
{
    Script_QHashNodeCreate(arg0,                    // 0x004B8909: push eax ([esp+0x14] = arg0)
                           arg1,                    // 0x004B8908: push edx ([esp+8] = arg1)
                           arg2,                    // 0x004B8907: push ecx ([esp+0xC] = arg2)
                           arg3,                    // 0x004B8902: push eax ([esp+0x10] = arg3)
                           reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1U)), // 0x004B8900: push 1
                           reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2U)), // 0x004B88FE: push 2 (0x004B8890 pushes 0 here)
                           nullptr);                // 0x004B88FC: push 0
}

} // extern "C"
