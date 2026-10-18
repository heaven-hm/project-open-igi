// IGI native provenance: MovieSubtitle_RegisterMedia - 0x004150E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004150E0-MovieSubtitle_RegisterMedia.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004150E0-MovieSubtitle_RegisterMedia.hpp"
int MovieSubtitle_RegisterMedia(void* self, int* args)

{
    int result_val;
    void* counter_val;
    int scratch_val;
    result_val = Resource_FileExists("LANGUAGE:movies.res");
    void* pThis_1;

    if (!result_val)
    {
        pThis_1 = self;
        *(pThis_1 + 0x30) = 0;
    }
    else
    {
        char* index = "LANGUAGE:movies.res";
        pThis_1 = self;
        *(pThis_1 + 0x30) = Resource_Load(result_val, scratch_val, counter_val, nullptr);
    }

    void* result_val_2 = Config_GetActivePlayerProfileRecord();

    if (result_val_2)
        Display_CloseDevice_3(result_val_2 + 0x11f, result_val_2 + 0x1f);

    *(pThis_1 + 0x24) = 0;
    *(pThis_1 + 0x20) = *args;
    *(pThis_1 + 0x34) = args[2];
    int* result_val_5 = args[1];
    *(pThis_1 + 0x28) = result_val_5;
    void* result_val_6 = *result_val_5;
    *(pThis_1 + 0x24) = 1;
    int result_1 = Display_OpenDevice_2(result_val_6, IntroScreen_Update, 0);
    *(pThis_1 + 0x2c) = result_1;
    return result_1;
}
