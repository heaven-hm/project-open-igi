// IGI native provenance: Menu_UpdateLanguage_14 - 0x004248A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004248A0-Menu_UpdateLanguage_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_UpdateLanguage_14.h"
char* Menu_UpdateLanguage_14(void* context, void* arg2, int arg3, char* arg4, int arg5, int* arg6, char** arg7)

{
    void* context_val = context;
    char* result = *(context_val + 0x44);
    char* result_1;

    if (!result)
        result_1 = nullptr;
    else
    {
        result = Menu_RequestScreen_4(result);
        result_1 = result;
    }

    int entity_val = *(result_1 + 0xc);

    if (entity_val <= arg2)
        result = 0;
    else
    {
        int temp_val = *(result_1 + 0x10);

        if (temp_val <= arg3)
            result = 0;
        else
        {
            result = arg4;

            if (entity_val >= arg2 + result || temp_val >= arg3 + arg5)
                result = 0;
            else
                result = 1;
        }
    }

    char temp_1 = *result_1;
    context = result;
    char entity_2;

    if ((!temp_1 || !*(context_val + 0x4a)) && (!result_1[1] || !result))
        entity_2 = 0;
    else
        entity_2 = 1;

    if (!*(context_val + 0x4a) || !result_1[2] || !result)
        result = 0;
    else
        result = 1;

    if (!temp_1)
    {
        if (!context)
            temp_1 = 0;
        else
            temp_1 = 1;
    }
    else if (!entity_2 || !context)
        temp_1 = 0;
    else
        temp_1 = 1;

    *(context_val + 0x4a) = entity_2;
    char entity_3;

    if (*(context_val + 0x49) || result_1[3])
        entity_3 = 1;
    else
        entity_3 = 0;

    *(context_val + 0x48) = entity_3;
    *(context_val + 0x49) = temp_1;
    *(context_val + 0x4b) = result;

    if (arg6)
        *arg6 = *(result_1 + 0xc);

    if (arg7)
    {
        result = *(result_1 + 0x10);
        *arg7 = result;
    }

    return result;
}
