// IGI native provenance: Resource_Unload - 0x004B6380 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B6380-Resource_Unload.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Resource_Unload.h"
int Resource_Unload(int* context)

// Sig: void Resource_Unload(int *resource_file)
// Note: Name from igi.exe error string inside function: "Resource_Unload() :
// Resource '%s' is not loaded." Unloads a resource by name/path.
{
    int* source_val = context;

    if (Is_ResourceLoaded(source_val, &context))
    {
        int* entity_val = context;
        int result = entity_val[9];

        if (result != 3 && result != 1)
            return Sys_FreeMemory_16(entity_val);

        return result;
    }

    int* item_8_1 = source_val;
    Error_Show("Resource_Unload() : Resource '%s' is not loaded.");

    while (true)
        /* nop */
}
