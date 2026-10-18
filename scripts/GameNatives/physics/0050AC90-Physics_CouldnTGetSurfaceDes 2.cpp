// IGI native provenance: Physics_CouldnTGetSurfaceDes - 0x0050AC90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0050AC90-Physics_CouldnTGetSurfaceDes.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_CouldnTGetSurfaceDes.h"
int Physics_CouldnTGetSurfaceDes(int* context)

{
    int* data_val = context;
    int result = 0;

    if (data_val)
    {
        while (true)
        {
            int item_47c = 0x7c;
            int data_1 = (*(*data_val + 0x58))(data_val, &item_47c);
            void item_400;

            if (data_1)
            {
                Game_DataSymbolLoad(&item_400, "Couldn't get surface description");
                int item_4a4 = DDERR_GENERIC(data_1);
                void* item_4a8 = &item_400;
                Error_Show("%s\n\nDirectX error message: %s");

                while (true)
                    /* nop */
            }
            else
            {
                int (* item_48c)() = nullptr;
                int item_474;
                int item_470;
                int item_428;
                result += (item_428 >> 3) * item_474 * item_470;
                int* data_6 = context;
                int item_488_1 = 0;
                int item_484_1 = 0;
                item_48c = QTask_CreateTask;
                int item_480_1 = 0;
                int data_7 = (*(*data_6 + 0x30))(data_6, &item_48c, &context);

                if (data_7 == 0x887600ff)
                    break;

                if (data_7)
                {
                    Game_DataSymbolLoad(&item_400, "Couldn't get attached surface");
                    int item_4a4_1 = DDERR_GENERIC(data_7);
                    void* item_4a8_1 = &item_400;
                    Error_Show("%s\n\nDirectX error message: %s");

                    while (true)
                        /* nop */
                }
                else
                {
                    data_val = context;

                    if (!data_val)
                        break;
                }
            }
        }
    }

    return result;
}
