// IGI native provenance: Display_RestoreDevice_10 - 0x00496450 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00496450-Display_RestoreDevice_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_RestoreDevice_10.hpp"
int* Display_RestoreDevice_10(int context, int arg2)

{
    return Display_CheckCapabilities_10(0x5ca1b8, context, arg2);
}
