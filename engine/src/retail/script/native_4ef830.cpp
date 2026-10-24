// Native: Script_Handler_4EF830 (placeholder until VERIFIED); retail VA 0x004EF830-0x004EF844 (21 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442408 8b542404 8b08 51 52 e83f1ffdff 83c408 c3); one of the 83 handler pointers embedded by the 0x004EE970 catalogue.
// Behavior/CFG: reads the dword pointed to by its second argument and forwards it, together with its first argument, to the task-list registrar 0x004C1780, then returns. Retail pushes both words and cleans 8.
// Status/limits: TRANSCRIBED (static only); the [eax] load is emitted through a volatile access because Release -O3 removed a plain unused read, and the frozen-artifact check must confirm the load is present before this native is signed off; 0x004C1780 is a one-byte `ret` (transcribed in native_4c1780.cpp) whose ABI consumes nothing, so the two pushed words are dead at the callee, but the argument load in [eax] is preserved because it is a real memory read; runtime UNOBSERVED.
#include "igi/retail/core/native_4c1780.hpp"

#include <cstdint>

extern "C" {

void Script_Handler_4EF830(void* const argument, std::uint32_t* const value)
{
    // The read is observable (retail faults here on an invalid pointer) and -O3 will delete a plain
    // read whose value is unused, which it did in 7faa872e (the whole body collapsed to a tail call).
    // volatile is the minimal form that keeps the load in the emitted code; it changes no value, no
    // ordering and no control flow.
    (void)*reinterpret_cast<volatile std::uint32_t*>(value); // 0x004EF838: mov ecx,[eax] (eax = [esp+8], second argument)
    AnimSound_LoadParameters();                   // 0x004EF83C: call 0x4c1780 (callee is `ret`; the pushed words are dead)
}

} // extern "C"
