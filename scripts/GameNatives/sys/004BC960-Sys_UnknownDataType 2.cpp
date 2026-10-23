// IGI native provenance: Sys_UnknownDataType - 0x004BC960 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BC960-Sys_UnknownDataType.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_UnknownDataType.h"
int Sys_UnknownDataType(void* context)

{
    int flagP;
    int item_4 = flagP;
    Camera_UpdateTransform_5(0x1008);
    int result_1 = 0;
    int i = *(context + 0x1c);
    int result = *(context + 0xc);
    int source_val = *(context + 0x18);
    int i_1 = i;

    if (i < result)
    {
        do
        {
            char* item_1c;

            switch (*(*(context + 0x28) + (i << 2)) - 1)
            {
                case 0:
                {
                    int item_18_1 = *(*(context + 0x24) + (source_val << 2));
                    item_1c = &g_53fde4;
                    sprintf(&return_addr, "%d");
                    source_val += 1;
                    break;
                }
                case 1:
                {
                    item_1c = *(*(context + 0x24) + (source_val << 2));
                    sprintf(&return_addr, "%f");
                    source_val += 1;
                    break;
                }
                case 2:
                {
                    int item_18_3 = *(context + 0x24) + (source_val << 2) + 4;
                    item_1c = ""%s"";
                    sprintf(&return_addr, ""%s"");
                    source_val = source_val + *(*(context + 0x24) + (source_val << 2)) + 1;
                    break;
                }
                case 3:
                {
                    int item_18_2 = *(context + 0x24) + (source_val << 2) + 4;
                    item_1c = &g_53f730;
                    sprintf(&return_addr, "%s");
                    source_val = source_val + *(*(context + 0x24) + (source_val << 2)) + 1;
                    break;
                }
                case 6:
                {
                    int context_val = *(*(context + 0x24) + (source_val << 2));
                    int item_18_4 = context_val;
                    item_1c = "%d words of data.";
                    sprintf(&return_addr, "%d words of data.");
                    source_val = source_val + context_val + 1;
                    i = i_1;
                    break;
                }
                default:
                {
                    item_1c = &return_addr;
                    sprintf(item_1c, "UNKNOWN DATA TYPE");
                    result_1 = 1;
                }
            }

            result = result_1;
            i += 1;
            i_1 = i;

            if (result)
                break;
        } while (i < *(context + 0xc));
    }

    return result;
}
