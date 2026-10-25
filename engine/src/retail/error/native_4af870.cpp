// Native: Error_CallbackSet_4AF870 (placeholder until VERIFIED); retail VA 0x004AF870-0x004AF8709 (10 bytes incl. padding, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 listing (8b442404 a3.. c3 90*6); part of the 16-byte-spaced setter family 0x4AF860/0x4AF870/0x4AF880; writer of the callback global used by 0x4AF810.
// Behavior/CFG: stores its single stack argument into the retail global and returns nothing; no branches.
// Status/limits: STRUCTURALLY_VERIFIED (static only); declares the global it is the retail writer of; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

void* g_936270 = nullptr; // 0x00936270

void Error_CallbackSet_4AF870(void* const callback)
{
    g_936270 = callback; // 0x004AF8704: mov [global], eax
}

} // extern "C"
