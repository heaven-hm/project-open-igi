// IGI native provenance: Device_CloseStream - 0x004B1690 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B1690-Device_CloseStream.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1690-Device_CloseStream.hpp"
void Device_CloseStream(int* context)

{
    if (context == 0x943980)
    {
        g_943988 = 0;
        return;
    }

    int entity_val = *context;
    (*(entity_val * 0x8c + &g_9436bc))(entity_val, context);
}
