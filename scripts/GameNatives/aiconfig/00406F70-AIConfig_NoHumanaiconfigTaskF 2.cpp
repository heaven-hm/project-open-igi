// IGI native provenance: AIConfig_NoHumanaiconfigTaskF - 0x00406F70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00406F70-AIConfig_NoHumanaiconfigTaskF.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIConfig_NoHumanaiconfigTaskF.h"
int AIConfig_NoHumanaiconfigTaskF(void* context)

{
    int source_val = g_5690d0;
    void* context_val = *(context + 0x14);

    if (!source_val)
        return Warning_Show("No HumanAIConfig task found");

    int* data_1 = Config_GetActivePlayerProfileRecord();

    if (!data_1)
        return Warning_Show("No active playerprofile found");

    int entity_val = *(context + 0x3d38);
    int saved_frame_1 = *data_1;
    void* source_1 = *(source_val + (((entity_val + 3) * 3 + saved_frame_1) << 2));

    if (!source_1)
    {
        int saved_frame = saved_frame_1;
        int item_1c_1 = AI_UpdatePatrol_4(entity_val);
        return Warning_Show("No configuration found for AIType: #%d, Difficulty: #%d");
    }

    *(context + 0x3d48) = *(source_1 + 0x230) * 30.0f;
    *(context + 0x3d44) = *(source_1 + 0x22c) * 30.0f;
    *(context + 0x3d4c) = *(source_1 + 0x228);
    *(context + 0x3d50) = *(source_1 + 0x268) * 30.0f;
    *(context + 0x3d54) = *(source_1 + 0x234);
    *(context + 0x3d58) = *(source_1 + 0x238);
    *(context + 0x3d5c) = *(source_1 + 0x23c);
    *(context + 0x3d60) = *(source_1 + 0x240);
    *(context + 0x3d64) = *(source_1 + 0x244);
    *(context + 0x3d68) = *(source_1 + 0x248);
    *(context + 0x3d6c) = *(source_1 + 0x24c);
    *(context + 0x3d70) = *(source_1 + 0x250);
    *(context + 0x3d74) = *(source_1 + 0x254);
    *(context + 0x3d78) = *(source_1 + 0x258);
    *(context + 0x3d7c) = *(source_1 + 0x25c);
    *(context + 0x3d80) = *(source_1 + 0x260);
    *(context + 0x3d84) = *(source_1 + 0x264);
    *(context + 0x3d88) = *(source_1 + 0x278);
    *(context + 0x3d8c) = *(source_1 + 0x27c);
    *(context + 0x3d90) = *(source_1 + 0x280) * 4096.0f;
    *(context + 0x3d94) = *(source_1 + 0x284) * 4096.0f;
    *(context + 0x3d98) = *(source_1 + 0x288);
    *(context + 0x3d9c) = *(source_1 + 0x28c);
    *(context + 0x3da0) = *(source_1 + 0x290);
    *(context + 0x3da4) = *(source_1 + 0x294);
    *(context + 0x3da8) = *(source_1 + 0x298);
    *(context + 0x3dac) = *(source_1 + 0x29c);
    *(context + 0x3dc0) = *(source_1 + 0x270) * 4096.0f;
    *(context + 0x3db8) = *(source_1 + 0x26c) * 4096.0f;
    *(context + 0x3dc8) = *(source_1 + 0x274);
    *(context_val + 0x258) = *(source_1 + 0x2a0);
    short result = AI_UpdateOrders_16();

    if (*(context_val + 0x1c) == result)
        *(context_val + 0xd04) = *(source_1 + 0x2a4);

    return result;
}
