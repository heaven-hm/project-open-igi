// IGI native provenance: Config_Write - 0x00405BD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00405BD0-Config_Write.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_Write.h"
int Config_Write(void* context)

// Sig: void Config_Write(char* cfg_file)
// Note: Write game config file, Params: File: 'LOCAL:config.qsc'
{
    int* data_val = QFile_ReadWrite(context, "w");

    if (data_val)
    {
        Config_SaveInputOptions(data_val);
        Config_Gocontentcontrolpw(data_val);
        int i = 0;

        if (g_bc2380 > 0)
        {
            do
            {
                Config_Goplayer(data_val, i);
                i += 1;
            } while (i < g_bc2380);
        }

        Device_CloseStream(data_val);
        int result = Script_GetForcePrecompiledState();

        if (!result)
            return result;

        Qsc_Compile(context);
        return QScript_Cleanup(context);
    }

    void* saved_index_1 = context;
    Error_Show("Could not create config file %s.");

    while (true)
        /* nop */
}
