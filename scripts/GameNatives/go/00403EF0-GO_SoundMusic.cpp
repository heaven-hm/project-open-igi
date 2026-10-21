// IGI native provenance: GO_SoundMusic - 0x00403EF0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00403EF0-GO_SoundMusic.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GO_SoundMusic.h"
int GO_SoundMusic(long double context , int arg2, float arg3 )

// Sig: int GO_SoundMusic(float volume)
// Note: Sets music volume for the current profile. Param: volume = float
// 0.0-1.0. Use in config scripts. Returns 1.
{
    char data_val = Script_GetArg(arg3, 0);
    int temp_val = g_bc2380;
    float item_2c = 1.40129846e-45.0f;
    float item_30 = arg3;
    *(temp_val * 0x564 + &g_bc2055) = data_val;
    Profile_ValueSet(item_30, item_2c);
    int entity_5 = g_bc2380;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    *(entity_5 * 0x564 + &g_bc204c) = context;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
