// IGI native provenance: Config_Moveleft - 0x00406280 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00406280-Config_Moveleft.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00406280-Config_Moveleft.hpp"
int Config_Moveleft(int context, int* arg2, void** arg3)

{
    int result = context;

    if (arg3)
        *arg3 = &"MoveLeft"[result * 4];

    if (arg2)
    {
        result *= 9;
        *arg2 = *((result << 2) + &g_536578);
    }

    return result;
}
