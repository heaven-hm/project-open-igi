// IGI native provenance: Config_YmbeAfp - 0x00402F50 - CFG score 94.4% (PASS; structural only); runtime behavior not verified.
// Evidence: 00402F50-Config_YmbeAfp.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_YmbeAfp.h"
int __fastcall Config_YmbeAfp(int context)

{
    int rootPathName = context;
    rootPathName = g_536408;

    for (char i = 0x41; i <= 0x5a; i += 1)
    {
        GetDriveTypeA(&rootPathName);
        int* data_1 = File_Open(".\ymbe.afp", "rb");

        if (data_1)
        {
            Camera_ApplyLineFilter_4(data_1, nullptr, FILE_BEGIN);
            int temp_1 = 0;
            temp_1 = Camera_SetFieldOfView_4(data_1) == 0x40;
            int result = Camera_UpdateTransform_4(data_1);

            if (temp_1)
                return result;
        }
    }

    Error_Show("IGI CD not found. Please insert CD in drive.");

    while (true)
        /* nop */
}
