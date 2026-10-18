// IGI native provenance: Display_OpenDevice_18 - 0x00499FF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00499FF0-Display_OpenDevice_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_OpenDevice_18.h"
unsigned int Display_OpenDevice_18(int context) {
    if (context <= 0x3f800000)
        return 0;

    return (context - 0x3f800000) >> 0x14;
}
