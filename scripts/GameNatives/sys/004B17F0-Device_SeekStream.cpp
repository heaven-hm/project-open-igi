// IGI native provenance: Device_SeekStream - 0x004B17F0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B17F0-Device_SeekStream.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B17F0-Device_SeekStream.hpp"
int Device_SeekStream(int* context, int arg2)

{
    int entity_val = *context;
    return (*(entity_val * 0x8c + &g_9436e4))(entity_val, context, arg2);
}
