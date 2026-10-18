// IGI native provenance: Config_GraphicOptionsGetResolution - 0x004045B0 - CFG score 93.3% (PASS; structural only); runtime behavior not verified.
// Evidence: 004045B0-Config_GraphicOptionsGetResolution.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004045B0-Config_GraphicOptionsGetResolution.hpp"
int Config_GraphicOptionsGetResolution(int context, float arg2 )

// Sig: int Config_GraphicOptionsGetResolution()
// Note: Returns the active profile's display-mode/resolution index (same index
// space as Config_GraphicOptionsSetResolution). Use in video-options UI scripts
// to preselect the current value.
{
    int data_1 = Script_GetArg(arg2, 0);
    int flagP = *Config_GetActiveGraphicOptions();
    int entity_val = *(Config_GetActiveGraphicOptions() + 4);
    int temp_val = *(Config_GetActiveGraphicOptions() + 8);
    int* context_val = nullptr;
    int data_8 = data_1 * 0x404;
    int dest_val = 0x7fffffff;
    int* source_1 = data_8 + 0x567d98;
    float data_9 = *(data_8 + &g_568098);

    if (data_9 > 0)
    {
        arg2 = data_9;
        bool flag_0_1;

        do
        {
            int data_12;
            int temp_1;
            temp_1 = HIGHD(source_1[1] - entity_val);
            data_12 = LOWD(source_1[1] - entity_val);
            int data_15;
            int temp_2;
            temp_2 = HIGHD(*source_1 - flagP);
            data_15 = LOWD(*source_1 - flagP);
            int data_20;
            int temp_4;
            temp_4 = HIGHD(source_1[2] - temp_val);
            data_20 = LOWD(source_1[2] - temp_val);
            int entity_6 = ((data_12 ^ temp_1) - temp_1) * ((data_15 ^ temp_2) - temp_2)
                + (((data_20 ^ temp_4) - temp_4) << 0x18);

            if (entity_6 < dest_val)
            {
                context_val = source_1;
                dest_val = entity_6;
            }

            source_1 = &source_1[3];
            flag_0_1 = arg2 != 1;
            arg2 -= 1;
        } while (flag_0_1);
    }

    char* item_8 = &g_567c74;
    float data_30 = (context_val[2] << 0xd | *context_val) << 0xd | context_val[1];
    float item_18 = data_30;
    double item_10 = data_30;
    memcpy(context, &item_18, 0x18);
    return context;
}
