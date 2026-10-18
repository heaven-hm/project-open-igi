// IGI native provenance: PictureBox_Update - 0x0041B530 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041B530-PictureBox_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0041B530-PictureBox_Update.hpp"
int PictureBox_Update(void* context, int arg2)

{
    Script_Isexists(context + 0x95c, context);
    Script_Isexists(context + 0x9ac, context);
    return (*(((Script_ParseEnumReal32_8() * 0x180 + Menu_PlayMusic_13()) << 2) + &g_a96ae0))(context, arg2);
}
