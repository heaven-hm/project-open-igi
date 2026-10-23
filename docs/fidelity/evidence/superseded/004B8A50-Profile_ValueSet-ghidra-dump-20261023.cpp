// IGI native provenance: Profile_ValueSet - 0x004B8A50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B8A50-Profile_ValueSet.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Profile_ValueSet.h"
int Profile_ValueSet(float context, float arg2)

// Sig: void Profile_ValueSet(void* profile, int index)
// Note: Sets a profile value through the profile object/value-table path used
// by the retail music and sound-effects option setters. Ghidra/r2 CFG
// agreement: 3 basic blocks and 2 edges; see
// assets/IGI1-Native-Name-Evidence.json.
{
    long double fpu0;
    long double fpu1;

    if (!*(context + 4))
        return Sys_DispatchOpcode_24(fpu0, fpu1, *(*(context + 8) + (arg2 << 2)));
    int result;
    int temp_val;
    long double fp0;
    fp0 = Sys_DispatchOpcode_28(context, temp_val, arg2, context, arg2);
    return result;
}
