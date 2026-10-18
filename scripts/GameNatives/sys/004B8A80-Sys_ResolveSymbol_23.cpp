// IGI native provenance: Sys_ResolveSymbol_23 - 0x004B8A80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B8A80-Sys_ResolveSymbol_23.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B8A80-Sys_ResolveSymbol_23.hpp"
char* Sys_ResolveSymbol_23(float context, int arg2, char* arg3, char* arg4)

{
    if (!*(context + 4))
        return Sys_SyntaxErrorInStringE(*(*(context + 8) + (arg2 << 2)), arg3, arg4);

    return Sys_PushStack_28(context, arg2, arg3, arg4);
}
