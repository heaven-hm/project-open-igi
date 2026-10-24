// Native: Mem_QHashFlagSet_4B0D40 (placeholder until VERIFIED); retail VA 0x004B0D40-0x004B0D47 (7 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 listing (c605fd2294.. c3): mov byte [0x9422FD],1; ret. Byte-reference search /r confirmed this is the only setter. No callees.
// Behavior/CFG: writes the byte 1 to the retail live flag at 0x009422FD (aliasing the second byte of the dword at 0x009422FC) and returns.
// Status/limits: TRANSCRIBED (static only); single store, no branches; runtime UNOBSERVED.
#include "igi/retail/mem/mem_alloc.hpp"

extern "C" {

void Mem_QHashFlagSet_4B0D40()
{
    igi::retail::mem::g_9422fd = 1;   // 0x004B0D40: mov byte [0x9422FD],1
}

} // extern "C"
