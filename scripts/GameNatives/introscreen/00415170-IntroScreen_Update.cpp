// IGI native provenance: IntroScreen_Update - 0x00415170 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00415170-IntroScreen_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00415170-IntroScreen_Update.hpp"
int IntroScreen_Update(int context, void* arg2)

{
    Game_DefineOptions("MovieSubtitle_RegisterFont", MovieSubtitle_RegisterFont, context, 0);
    Game_DefineOptions("MovieSubtitle_RegisterText", MovieSubtitle_RegisterText, context, 0);
    int* data_1 = Qvm_Load(arg2);

    if (data_1)
    {
        Qvm_Execute(data_1);
        Qvm_Cleanup(data_1);
    }

    Sys_CloseFileStream_23("MovieSubtitle_RegisterFont");
    int result = Sys_CloseFileStream_23("MovieSubtitle_RegisterText");
    result = 1;
    return result;
}
