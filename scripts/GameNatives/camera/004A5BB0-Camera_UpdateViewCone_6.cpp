// IGI native provenance: Camera_UpdateViewCone_6 - 0x004A5BB0 - CFG score 47.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004A5BB0-Camera_UpdateViewCone_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A5BB0-Camera_UpdateViewCone_6.hpp"
char* Camera_UpdateViewCone_6(char* context, char* arg2)

{
    short temp_val;
    temp_val = *arg2;
    char* result = context;

    if (!temp_val)
        return result;

    *(&temp_val + 1) = arg2[1];

    if (!*(&temp_val + 1))
    {
        int data_2 = 0;
        data_2 = temp_val;
        /* tailcall */
        return sub_4aa0a6(data_2, context);
    }

    while (true)
    {
        char* entity_val = arg2;
        short data_val;
        data_val = *result;
        char* source_2 = &result[1];

        if (data_val != temp_val)
        {
            if (!data_val)
                return 0;

            while (true)
            {
                data_val = *source_2;
                source_2 = &source_2[1];
            label_4a5bdc:

                if (data_val == temp_val)
                    break;

                if (!data_val)
                    return 0;
            }
        }

        data_val = *source_2;
        source_2 = &source_2[1];

        if (data_val != *(&temp_val + 1))
            break;

        result = &source_2[0xffffffff];

        while (true)
        {
            *(&data_val + 1) = entity_val[2];

            if (*(&data_val + 1))
            {
                data_val = *source_2;
                source_2 = &source_2[2];

                if (data_val != *(&data_val + 1))
                    break;

                data_val = entity_val[3];

                if (data_val)
                {
                    *(&data_val + 1) = source_2[0xffffffff];
                    entity_val = &entity_val[2];

                    if (data_val != *(&data_val + 1))
                        break;

                    continue;
                }
            }

            return &result[0xffffffff];
        }
    }

    goto label_4a5bdc;
}
