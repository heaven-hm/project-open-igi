// IGI native provenance: Sys_DispatchOpcode_7 - 0x004B1FC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1FC0-Sys_DispatchOpcode_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1FC0-Sys_DispatchOpcode_7.hpp"
int Sys_DispatchOpcode_7(void* context)

{
    int* data_val = QFile_ReadWrite(context, "r");

    if (!data_val)
        return 0;

    Device_CloseStream(data_val);
    return 1;
}
