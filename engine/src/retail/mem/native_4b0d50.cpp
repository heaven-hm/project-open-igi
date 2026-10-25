// Native: Mem_QHashFlagClear (placeholder until VERIFIED); retail VA 0x004B0D50-0x004B0D57 (7 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 listing (c605fd2294.. c3): mov byte [0x9422FD],0; ret. Byte-reference search /r found no other clearer at this address. No callees.
// Behavior/CFG: writes the byte 0 to the retail live flag at 0x009422FD and returns.
// Status/limits: BUILD_VERIFIED (static only); single store, no branches; runtime UNOBSERVED.
#include "igi/retail/mem/mem_alloc.hpp"

extern "C" {

void Mem_QHashFlagClear()
{
    igi::retail::mem::g_9422fd = 0;   // 0x004B0D50: mov byte [0x9422FD],0
}

} // extern "C"
