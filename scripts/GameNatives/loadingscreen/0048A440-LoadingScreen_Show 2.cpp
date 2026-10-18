// IGI native provenance: LoadingScreen_Show - 0x0048A440 - CFG score 98.3% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048A440-LoadingScreen_Show.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "LoadingScreen_Show.h"
int* LoadingScreen_Show(int context)

// Sig: void LoadingScreen_Show(int picVariant)
// Note: Shows the LOADING SCREEN: allocates a 0x1C-byte screen object, queries
// the active display mode (0x491CF0), and loads
// "LOCAL:menusystem/loadingscreen.res" with picture
// "LOCAL:menusystem/loading.pic" - or "loading_us.pic" for USA country builds.
// Param: picVariant = loading-screen type selector. Call before long loads so
// the player sees feedback.
{
    int item_4c = 4;
    void* item_50 = 0x1c;
    int* result = Mem_Alloc(item_50, item_4c);
    result[1] = 0;
    *result = context;
    int data_3;
    int temp_val;
    temp_val = HIGHD(g_c28b44 - 0x280);
    data_3 = LOWD(g_c28b44 - 0x280);
    result[2] = ((data_3 - temp_val) >> 1) + 0x28;
    int data_9;
    int temp_1;
    temp_1 = HIGHD(g_c28b48 - 0x1e0);
    data_9 = LOWD(g_c28b48 - 0x1e0);
    result[3] = ((data_9 - temp_1) >> 1) + 0x1b8;
    Display_SetDisplayMode_16(&result[4], 0x230, 0xa);
    int entity_1;
    int dest_1;
    dest_1 = __memfill_u32(result[6], 0x7f00, 0x15e0);
    int item_34;
    Display_SetDisplayMode_16(&item_34, g_c28b44, g_c28b48);
    Sys_ExecuteBytecode_3(Display_CloseDevice_16());
    int* data_15;
    void* entity_2;
    int temp_3;
    data_15 = Display_SwapBuffers_16(&item_34);
    char* item_78 = "LOCAL:menusystem/loadingscreen.res";
    int* data_16 = Resource_Load(data_15, temp_3, entity_2, nullptr);
    char* item_4c_1;

    if (FX_SmokeEmit_26())
        item_4c_1 = "LOCAL:menusystem/loading_us.pic";
    else
        item_4c_1 = "LOCAL:menusystem/loading.pic";

    int* data_18 = Sys_ResolveSymbol_17(item_4c_1);
    data_18[8] = 0xffffffff;
    int data_21;
    int temp_4;
    temp_4 = HIGHD(g_c28b44 - 0x280);
    data_21 = LOWD(g_c28b44 - 0x280);
    data_18[1] = (data_21 - temp_4) >> 1;
    int data_26;
    int temp_5;
    temp_5 = HIGHD(g_c28b48 - 0x1e0);
    data_26 = LOWD(g_c28b48 - 0x1e0);
    data_18[2] = (data_26 - temp_5) >> 1;
    Sys_AllocateMemory_18(data_18);
    Sys_CloseFileStream_3();
    void item_28;
    memset(&item_28, 0, 0x28);
    int item_24 = Display_CreateSurface_4();
    int item_2c;
    int item_8 = item_2c;
    int data_31 = item_34;
    int item_10 = data_31;
    int item_18 = 0;
    item_50 = &item_28;
    int item_1c = data_31 << 2;
    int item_14 = 0;
    int item_30;
    int item_c = item_30;
    int item_4 = 1;
    Display_RestoreDevice_4(item_50);
    Display_SyncVSync_16(&item_34);
    Sys_CheckHeap_17(data_18);
    Resource_Flush(data_16);
    FX_FragmentSimulate_16(result, 0);
    return result;
}
