// IGI native provenance: AIFunction_SetSecondaryAlarmViewLength - 0x0044DE80 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0044DE80-AIFunction_SetSecondaryAlarmViewLength.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIFunction_SetSecondaryAlarmViewLength.h"
int AIFunction_SetSecondaryAlarmViewLength(long double context , int arg2, float arg3 )

// Sig: void AIFunction_SetSecondaryAlarmViewLength(int length)
// Note: Sets secondary view length during alarms. Param: length = distance in
// engine units.
{
    Profile_ValueSet(arg3, 0.0f);
    void* entity_val = g_5bda58;
    int item_18 = 1;
    *(entity_val + 0xd8) = context * 4096.0;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
