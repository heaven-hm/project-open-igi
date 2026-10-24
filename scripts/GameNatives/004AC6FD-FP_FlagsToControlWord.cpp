// IGI native provenance: Camera_ApplyWideScreen_24 - 0x004AC6FD - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AC6FD-Camera_ApplyWideScreen_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AC6FD-Camera_ApplyWideScreen_24.hpp"
int Camera_ApplyWideScreen_24(int context) {
    int result = 0;

    if (context & 0x10)
        result = 1;

    if (context & 8)
        result |= 4;

    if (context & 4)
        result |= 8;

    if (context & 2)
        result |= 0x10;

    if (context & 1)
        result |= 0x20;

    if (context & 0x80000)
        result |= 2;

    int entity_1 = context & 0x300;

    if (entity_1)
    {
        if (entity_1 == 0x100)
            *(&result + 1) |= 4;
        else if (entity_1 == 0x200)
            *(&result + 1) |= 8;
        else if (entity_1 == 0x300)
            *(&result + 1) |= 0xc;
    }

    int entity_3 = context & 0x30000;

    if (!entity_3)
        result |= 0x300;
    else if (entity_3 == 0x10000)
        result |= 0x200;

    if (context & 0x40000)
        *(&result + 1) |= 0x10;

    return result;
}
