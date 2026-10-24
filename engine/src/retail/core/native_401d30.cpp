// Native: TaskType_SetPhaseHandlerByte (placeholder until VERIFIED); retail VA 0x00401D30-0x00401D4C (29 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing 8b4c2408 8b442404 888124ff5400 25ff000000 8888e069a900 e962f7ffff - two byte stores then an unconditional jmp 0x4014B0, i.e. a tail call rather than an E8 call.
// Behavior/CFG: stores the low byte of arg1 at 0x54FF24 + arg2, stores the low byte of arg2 at 0xA969E0 + (arg1 & 0xFF), then tail-jumps to the landed 0x4014B0, whose ret returns straight to this native's caller; retail therefore leaves the two cdecl arguments on the stack for the caller's cleanup (0x4C5944 add esp,0x40).
// Status/limits: TRANSCRIBED (static only); the jmp is modelled as a normal call plus return, so this native pops its own two arguments where retail defers that to the only caller - reviewer attention requested; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

// Declares the retail native 0x004014B0, which this caller tail-jumps into. Its implementation lives in
// native_4014b0.cpp: one file per retail address, so no provenance block for that VA is repeated here.
void TaskType_RefreshHandlerMask();

void TaskType_SetPhaseHandlerByte(std::int32_t const value, std::int32_t const slot)
{
    *reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(0x0054FF24U) + static_cast<std::uint32_t>(slot)) =
        static_cast<std::uint8_t>(value); // 0x00401D38: mov byte [ecx + 0x54ff24], al (ecx = arg2)
    *reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(0x00A969E0U) +
                                     static_cast<std::uint32_t>(value & 0xFF)) =
        static_cast<std::uint8_t>(slot); // 0x00401D43: mov byte [eax + 0xa969e0], cl - the CL register, i.e. the low byte of arg2, not of arg1
    TaskType_RefreshHandlerMask();        // 0x00401D49: jmp 0x4014B0 (tail call)
}

} // extern "C"
