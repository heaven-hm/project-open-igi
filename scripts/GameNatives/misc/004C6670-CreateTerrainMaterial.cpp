// IGI native provenance: CreateTerrainMaterial - 0x004C6670 - CFG score 96.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C6670-CreateTerrainMaterial.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "CreateTerrainMaterial.h"
int CreateTerrainMaterial(int context, float arg2 )

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
    int data_1 = Script_GetArg(arg2, 0) * 5;
    int i_1 = 1;
    int dest_1 = data_1 * 0xb4;
    *(dest_1 + &g_c28fe0) = Script_GetArg(arg2, i_1);
    int i = 3;
    float* dest_2 = dest_1 + 0xc28fe8;

    do
    {
        int data_5 = Script_GetArg(arg2, i - 1);
        i_1 = i;
        dest_2[-1] = data_5;
        Profile_ValueSet(arg2, i_1);
🚫🚫🚫        *dest_2 =
        /* unimplemented  {fstp dword [dest_val], fp0} */
        Profile_ValueSet(arg2, i + 1);
🚫🚫🚫        dest_2[1] = /* dest_2[1] = unimplemented  {fstp dword [dest_val+0x4], fp0} */;
        /* unimplemented  {fstp dword [dest_val+0x4], fp0} */
        Profile_ValueSet(arg2, i + 2);
🚫🚫        /* unimplemented  {fmul fp0, dword [&g_533504]} */
🚫🚫🚫        dest_2[2] = /* dest_2[2] = unimplemented  {fstp dword [dest_val+0x8], fp0} */;
        /* unimplemented  {fstp dword [dest_val+0x8], fp0} */
        dest_2[3] = Script_GetArg(arg2, i + 3);
        Profile_ValueSet(arg2, i + 4);
🚫🚫        /* unimplemented  {fmul fp0, dword [&g_533504]} */
🚫🚫🚫        dest_2[4] = /* dest_2[4] = unimplemented  {fstp dword [dest_val+0x10], fp0} */;
        /* unimplemented  {fstp dword [dest_val+0x10], fp0} */
        short idx = idx + 4;
        dest_2[5] = Script_GetArg(arg2, i + 5);
        i += 7;
        dest_2 = &dest_2[7];
    } while (i < 0xe3);

    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
