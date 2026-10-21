#include "igi/retail/core/app_startup_natives.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

extern "C" {

char const* CommandLine = nullptr;
void* InstanceHandle = nullptr;

// Native: StoreCommandLine; retail VA 0x0048F0C0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F0C0..0x0048F0CA.
// Behavior/CFG: one basic block loads the 32-bit argument, stores it at 0x005C8BC8, and returns the same bits in EAX. igi.exe's main pushes the command line and calls this native before it registers the window class.
// Status/limits: STRUCTURALLY_VERIFIED; the load, the store and the return are the retail's. The paired start-up runtime comparison is still required for acceptance.
std::int32_t IGI_RETAIL_CDECL StoreCommandLine(char const* command_line)
{
    CommandLine = command_line;
    return 0;
}

// Native: GetCommandLine; retail VA 0x0048F080.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F080..0x0048F086.
// Behavior/CFG: one basic block loads the 32-bit global at 0x005C8BC8 and returns it in EAX.
// Status/limits: STRUCTURALLY_VERIFIED; the load and the EAX return are the retail's.
char const* IGI_RETAIL_CDECL GetRetailCommandLine()
{
    return CommandLine;
}

// Native: StoreInstanceHandle; retail VA 0x0048F0D0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F0D0..0x0048F0DA.
// Behavior/CFG: one basic block loads the 32-bit argument, stores it at 0x005C8BC0, and returns the same bits in EAX. igi.exe's main pushes the instance handle and calls this native right after the command line.
// Status/limits: STRUCTURALLY_VERIFIED; the load, the store and the return are the retail's.
std::int32_t IGI_RETAIL_CDECL StoreInstanceHandle(void* instance)
{
    InstanceHandle = instance;
    return 0;
}

// Native: GetInstanceHandle; retail VA 0x0048F0B0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F0B0..0x0048F0B6.
// Behavior/CFG: one basic block loads the 32-bit global at 0x005C8BC0 and returns it in EAX.
// Status/limits: STRUCTURALLY_VERIFIED; the load and the EAX return are the retail's.
void* IGI_RETAIL_CDECL GetInstanceHandle()
{
    return InstanceHandle;
}

} // extern "C"

#undef IGI_RETAIL_CDECL
