// IGI native provenance: Device_GetStreamPosition - 0x004B1780 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B1780-Device_GetStreamPosition.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1780-Device_GetStreamPosition.hpp"
int Device_GetStreamPosition(int* context)

{
    int entity_val = *context;
    return (*(entity_val * 0x8c + &g_9436d0))(entity_val, context);
}
