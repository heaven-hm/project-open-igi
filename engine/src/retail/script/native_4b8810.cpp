// Native: Script_QHashNodeRegister (placeholder until VERIFIED); retail VA 0x004B8810-0x004B882F (32 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442408 8b4c2404 6a00 6a01 6a02 6a04 50 68 30884b00 51 e894220000 83c41c c3); seven pushes with add esp,0x1c; callee 0x4BAAC0 landed.
// Behavior/CFG: forwards to 0x4BAAC0(arg1, 0x004B8830, arg2, 4, 2, 1, 0) in retail push order - the handler address 0x004B8830 is stored as the record's arg2 field, with constants 4/2/1/0 - and returns nothing (the callee's eax is discarded by all callers).
// Status/limits: STRUCTURALLY_VERIFIED (static only); the constants and handler address are passed as raw values exactly as retail pushes them; runtime UNOBSERVED.
#include <cstdint>

extern "C" void Script_QHashNodeCreate(void* arg1, void* arg2, char const* arg3, void* arg4,
                                             void* arg5, void* arg6, void* arg7);

extern "C" {

void Script_QHashNodeRegister(void* const arg1, void* const value)
{
    Script_QHashNodeCreate(arg1,
                                  reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x004B8830U)), // handler address pushed by retail
                                  static_cast<char const*>(value),
                                  reinterpret_cast<void*>(static_cast<std::uintptr_t>(4U)),
                                  reinterpret_cast<void*>(static_cast<std::uintptr_t>(2U)),
                                  reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U)),
                                  nullptr);
}

} // extern "C"
