#include "igi/retail/core/native_0048f0f0.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::int32_t Native_005C8BCC = 0;

// Native: SetConfigFileName; retail VA 0x0048F0F0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F0F0..0x0048F0FA.
// Behavior/CFG: one basic block loads the 32-bit argument, stores it at 0x005C8BCC, and returns the same bits in EAX.
// Status/limits: VERIFIED_WITH_DOCUMENTED_SCOPE; the global's semantic name and paired startup runtime comparison remain unresolved. A byte search of the whole retail image finds no other reference to 0x005C8BCC, so the global has no reader the name could be derived from and the address-based identity stands.
std::int32_t NATIVE_DECL SetConfigFileName(std::int32_t value)
{
    Native_005C8BCC = value;
    return value;
}

} // extern "C"

#undef NATIVE_DECL
