// IGI native provenance: SoundSys_StopSoundSample_6 - 0x004E6750 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E6750-SoundSys_StopSoundSample_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_StopSoundSample_6.hpp"
int SoundSys_StopSoundSample_6(int* context)

{
    void item_80;
    Sys_ReadFileBlock_7(&item_80, 0x80, context);
    int result;
    void* entity_val;
    int temp_val;
    result = Resource_FileExists(context);

    if (!result)
        return result;

    int* item_8c_1 = context;
    Resource_Load(result, temp_val, entity_val, nullptr);
    Sys_OpenFileStream_16(&item_80, sub_4e67b0, nullptr);
    return Resource_Unload(context);
}
