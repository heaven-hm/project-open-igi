// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// IGI native provenance: Graph_Open - 0x004F9FF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F9FF0-Graph_Open.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Graph_Open.h"
int Graph_Open(void* context )

// Sig: void Graph_Open(unsigned int level,char* graph_str)
// Note: Open graph of level,Params: with str 'Graph4019.dat'
{
    void* saved_index_4 = context;
    int* data_val = QFile_ReadWrite(saved_index_4, "r");

    if (!data_val)
    {
        void* saved_index_1 = saved_index_4;
        return Warning_Show("Unable to open %s for reading");
    }

    void item_1c;
    void* item_14 = &item_1c;
    Resource_Unpack(data_val, &context, 4);
    void* arg_4;

    if (context != 0xffeeddcc)
    {
        Sys_ResolveSymbol_4(data_val);
        Sys_ExecuteBytecode_4(data_val, 0, 0);
        Script_SpawnNewTask_25(arg_4, &item_1c);
    }
    else
    {
        void* dest_1 = arg_4;
        void* item_10_1 = dest_1;
        int item_c_1 = 0;
        int item_8_1 = 0;

        if (!Sys_DispatchOpcode_4(data_val))
        {
            int i;

            do
            {
                int item_20;
                Script_FormatString_26(&item_1c, &item_20);
                int* data_6;
                unsigned int entity_1;
                data_6 = Script_FormatString_24(item_20);

                if (!data_6)
                {
                    int item_2c_5 = 2;
                    void* item_30_3 = &item_1c;
                    Script_ParseEnumInt32_25(entity_1);
                }
                else
                {
                    Script_CallNativeHandler_26(&item_1c, &arg_4);
                    unsigned int item_4_1 = arg_4;
                    data_6[4](&item_14);
                }

                i = Sys_DispatchOpcode_4(data_val);
            } while (!i);
        }

        int item_2c_7 = 0;
        void* item_30_4 = dest_1;
    }
    Sys_ResolveSymbol_4(data_val);
    return Device_CloseStream(data_val);
}
