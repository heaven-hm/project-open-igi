// IGI native provenance: Texture_UploadPalettePacket - 0x004B7D30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7D30-Texture_UploadPalettePacket.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B7D30-Texture_UploadPalettePacket.hpp"
int Texture_UploadPalettePacket(int* context)

{
    return Sys_DispatchOpcode_3(*context, g_a84a40, context);
}
