#include "igi/retail/core/app_startup_natives.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

extern "C" {

std::int32_t Native_005C8BC8 = 0;

// Native: Native_0048F0C0; retail VA 0x0048F0C0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F0C0..0x0048F0CA.
// Behavior/CFG: one basic block loads the 32-bit argument, stores it at 0x005C8BC8, and returns the same bits in EAX.
// Status/limits: STRUCTURALLY_VERIFIED; the global's semantic name and paired startup runtime comparison remain unresolved.
std::int32_t IGI_RETAIL_CDECL Native_0048F0C0(std::int32_t value)
{
    Native_005C8BC8 = value;
    return value;
}

} // extern "C"

#undef IGI_RETAIL_CDECL
