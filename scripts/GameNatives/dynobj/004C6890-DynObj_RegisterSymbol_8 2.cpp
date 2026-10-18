// IGI native provenance: DynObj_RegisterSymbol_8 - 0x004C6890 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C6890-DynObj_RegisterSymbol_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_RegisterSymbol_8.h"
void* DynObj_RegisterSymbol_8(void* context, void* arg2, int arg3)

{
    if (arg3 == 0xffffffff)
    {
        void* entity_val = *(context + 0xd18);

        if (!entity_val)
        {
            *(context + 0xd18) = arg2;
            *(arg2 + 0x58) = 0;
            *(*(context + 0xd18) + 0x54) = 0;
            return context;
        }

        *(entity_val + 0x54) = arg2;
        void* entity_1 = *(context + 0xd18);
        *(*(entity_1 + 0x54) + 0x58) = entity_1;
        void* entity_3 = *(*(context + 0xd18) + 0x54);
        *(context + 0xd18) = entity_3;
        *(entity_3 + 0x54) = 0;
        return context;
    }

    void* temp_3 = *(context + (arg3 << 2) + 0xd1c);

    if (!temp_3)
    {
        *(context + (arg3 << 2) + 0xd1c) = arg2;
        *(arg2 + 0x58) = 0;
        void* data_1 = *(context + (arg3 << 2) + 0xd1c);
        *(data_1 + 0x54) = 0;
        return data_1;
    }

    *(temp_3 + 0x54) = arg2;
    void* temp_4 = *(context + (arg3 << 2) + 0xd1c);
    *(*(temp_4 + 0x54) + 0x58) = temp_4;
    void* temp_6 = *(*(context + (arg3 << 2) + 0xd1c) + 0x54);
    *(context + (arg3 << 2) + 0xd1c) = temp_6;
    *(temp_6 + 0x54) = 0;
    return arg3;
}
