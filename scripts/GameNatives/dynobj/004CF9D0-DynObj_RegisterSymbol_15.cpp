// IGI native provenance: DynObj_RegisterSymbol_15 - 0x004CF9D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CF9D0-DynObj_RegisterSymbol_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004CF9D0-DynObj_RegisterSymbol_15.hpp"
int* DynObj_RegisterSymbol_15(int* context)

{
    HeapList_Remove(context);
    int i = 0;

    if (context[0x11] > 0)
    {
        void* context_1 = nullptr;

        do
        {
            Texture_UploadPalettePacket(context_1 + context[0xf]);
            i += 1;
            context_1 += 0xc;
        } while (i < context[0x11]);
    }

    void* data_4 = context[0xf];

    if (data_4)
    {
        Sys_ExecuteBytecode_2(data_4);
        context[0xf] = 0;
    }

    int i_1 = 0;

    if (context[0x10] > 0)
    {
        void* dest_val = nullptr;

        do
        {
            int entity_1 = context[0xe];

            if (*(dest_val + entity_1 + 4))
                Texture_UploadPalettePacket(dest_val + entity_1);

            i_1 += 1;
            dest_val += 0xc;
        } while (i_1 < context[0x10]);
    }

    void* data_8 = context[0xe];

    if (data_8)
    {
        Sys_ExecuteBytecode_2(data_8);
        context[0xe] = 0;
    }

    int i_10 = context[0x10];

    if (i_10 - 1 >= 0)
    {
        int i_8 = i_10;
        int dest_2 = (i_10 - 1) * 0x24;
        int i_2;

        do
        {
            Struct_Clear36(dest_2 + context[0xd]);
            dest_2 -= 0x24;
            i_2 = i_8;
            i_8 -= 1;
        } while (i_2 != 1);
    }

    void* data_10 = context[0xd];

    if (data_10)
    {
        Sys_ExecuteBytecode_2(data_10);
        context[0xd] = 0;
    }

    int* i_6 = context[9];

    for (int* i_3 = i_6[1]; i_3; i_3 = i_3[1])
    {
        DynObj_InsertHashItem_15(i_6);
        i_6 = i_3;
    }

    int* i_7 = context[6];

    for (int* i_4 = i_7[1]; i_4; i_4 = i_4[1])
    {
        DynObj_FindSymbol_15(i_7);
        i_7 = i_4;
    }

    int* i_9 = context[0xc];

    for (int* i_5 = i_9[1]; i_5; i_5 = i_5[1])
    {
        HeapList_Remove(i_9);
        Sys_ExecuteBytecode_2(i_9);
        i_9 = i_5;
    }

    Resource_Flush(context[3]);

    if (!context[0x12])
        Resource_Flush(context[2]);

    Sys_ExecuteBytecode_2(context);
    return DynObj_FindSymbol_14();
}
