// Retail runtime observation: DynObj_LoadModelMesh - 0x004CF390 - 512 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: Model name entries observed, capped at 512 by probe; 0x004CF35F is the catalogue caller return address.
// IGI native provenance: DynObj_LoadModelMesh - 0x004CF390 - CFG score 99.1% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CF390-DynObj_LoadModelMesh.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004CF390-DynObj_LoadModelMesh.hpp"
int* DynObj_LoadModelMesh(int context)

{
    int* result = Mem_Alloc(0x18, 4);
    int item_20 = context;
    int* context_val = nullptr;
    result[1] = 0;
    *result = 0;
    result[4] = 0;
    result[5] = 0;
    int data_val;
    void* entity_val;
    int temp_val;
    data_val = Game_DataSymbolLoad(0xa4ec6c, "LOCAL:models/%s.mef");
    int item_30 = 0xa4ec6c;
    int* data_1 = Resource_Load(data_val, temp_val, entity_val, nullptr);
    result[2] = data_1;
    float* data_4 = Sys_CheckHeap_19(Sys_ReadFileBlock_20(Sys_ResolveSymbol_19(data_1), 0x4d455348), data_1);
    result[3] = data_4;

    if (*data_4 == 0x3dcccccd)
    {
        void* data_5 = result[2];
        *(result[3] + 0x8c) = Sys_CheckHeap_19(Sys_ReadFileBlock_20(Sys_ResolveSymbol_19(data_5), 0x41545441), data_5);
        void* data_9 = result[2];
        *(result[3] + 0x88) = Sys_CheckHeap_19(Sys_ReadFileBlock_20(Sys_ResolveSymbol_19(data_9), 0x4d565458), data_9);
        void* data_13 = result[2];
        *(result[3] + 0x98) = Sys_CheckHeap_19(Sys_ReadFileBlock_20(Sys_ResolveSymbol_19(data_13), 0x504f5254), data_13);
        void* data_17 = result[2];
        *(result[3] + 0x90) = Sys_CheckHeap_19(Sys_ReadFileBlock_20(Sys_ResolveSymbol_19(data_17), 0x50544643), data_17);
        void* data_21 = result[2];
        *(result[3] + 0x94) = Sys_CheckHeap_19(Sys_ReadFileBlock_20(Sys_ResolveSymbol_19(data_21), 0x50545658), data_21);

        if (Sys_ReadFileBlock_20(Sys_ResolveSymbol_19(result[2]), 0x4e415854))
        {
            int* data_29 = Sys_ReadFileBlock_20(Sys_ResolveSymbol_19(result[2]), 0x4e415854);
            int* dest_1 = data_29;
            int i_2 = 1;
            int* data_30;

            for (int* i = Matrix_InvalidateSlot(data_29); i; i = Matrix_InvalidateSlot(data_30))
            {
                data_30 = Sys_ReadFileBlock_20(i, 0x4e415854);

                if (!data_30)
                    break;

                i_2 += 1;
            }

            result[5] = Mem_Alloc(i_2 * 0x14, 4);
            result[4] = i_2;

            if (i_2)
            {
                int i_1;

                do
                {
                    int* data_32 = Sys_CheckHeap_19(dest_1, result[2]);
                    *(context_val + result[5] + 0x10) = &data_32[2];
                    *(context_val + result[5] + 8) = *data_32;
                    *(context_val + result[5]) = data_32[1];
                    *(context_val + result[5] + 4) = 0;
                    dest_1 = Matrix_InvalidateSlot(dest_1);

                    if (dest_1)
                        dest_1 = Sys_ReadFileBlock_20(dest_1, 0x4e415854);

                    context_val = &context_val[5];
                    i_1 = i_2;
                    i_2 -= 1;
                } while (i_1 != 1);
            }
        }

        int item_8 = result[3];
        int item_4 = result[2];

        if (*((Render_GetActiveBackendId() << 2) + &g_a94e84))
            (*((Render_GetActiveBackendId() << 2) + &g_a94e84))(&item_8);

        return result;
    }

    int saved_frame_1 = 0x3fb99999;
    int item_1c_1 = 0xa0000000;
    int item_28 = context;
    int item_2c = context;
    Error_Show("Mesh "%s" and source code are incompatible. (Mesh "%s" is version %0.2f, source code "
    "is version %0.2f.)");

    while (true)
        /* nop */
}
