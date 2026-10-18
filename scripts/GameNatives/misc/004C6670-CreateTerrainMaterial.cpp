// IGI native provenance: CreateTerrainMaterial - 0x004C6670 - CFG score 96.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C6670-CreateTerrainMaterial.md; retail R2 disassembly 0x004C6670..0x004C6755; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C6670-CreateTerrainMaterial.hpp"
int CreateTerrainMaterial(int context, float arg2)

// Sig: unknown CreateTerrainMaterial(unknown param1, unknown param2)
// Note: Registered at 0x4C6580 via registrar 0x4B8890 (push 0x4C6670 +
// 'CreateTerrainMaterial'). Pushes interleaved with inline strlen/rep-movs
// string setup; site also pushes 0x4C6670 at 0x4C655C.  Verified: r2
// forced-analysis confirmed exact start (size 230); P3 byte-level
// name<->address binding at registrar callsite 0x4C6580 via 0x4B8890; P6
// SHA-256(first 32 code
// bytes)=bd85673e252f99c903e7b8b25e1da01901ce82a84299e100833a25fef1c175b1;
// re-agent reverser/checker loop ran on this native (C decompilation pending
// ARM64 native port).
{
    int material_id = Script_GetArg(arg2, 0);
    int dest_1 = &g_c28fe0[material_id * 0x384];
    *(dest_1) = Script_GetArg(arg2, 1);
    int i = 3;
    char* dest_2 = dest_1;

    do
    {
        *(int*)(dest_2 + 4) = Script_GetArg(arg2, i - 1);
        *(float*)(dest_2 + 8) = Script_GetArgFloat(arg2, i);
        *(float*)(dest_2 + 12) = Script_GetArgFloat(arg2, i + 1);
        *(float*)(dest_2 + 16) = Script_GetArgFloat(arg2, i + 2) * 0.5f;
        *(int*)(dest_2 + 20) = Script_GetArg(arg2, i + 3);
        *(float*)(dest_2 + 24) = Script_GetArgFloat(arg2, i + 4) * 0.5f;
        *(int*)(dest_2 + 28) = Script_GetArg(arg2, i + 5);
        i += 7;
        dest_2 += 0x1c;
    } while (i < 0xe3);

    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy((void*)context, &item_18, 0x18);
    return context;
}
