// IGI native provenance: Menu_LocalLanguageSS - 0x00417FF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00417FF0-Menu_LocalLanguageSS.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_LocalLanguageSS.h"
int Menu_LocalLanguageSS(void* context, int arg2)

{
    for (int i = 0; i < 6; )
    {
        int item_114_1 = arg2;
        void* item_118_1 = Config_English_2(i);
        void item_100;
        int data_2;
        int temp_1;
        data_2 = Game_DataSymbolLoad(&item_100, "LOCAL:language/%s/%s");
        void* item_128_1 = &item_100;
        *(context + (*(context + 0x25c) << 2) + 0x1dc) = Resource_Load(data_2, temp_1, &item_100, nullptr);
        i += 1;
        *(context + 0x25c) += 1;
    }

    return 6;
}
