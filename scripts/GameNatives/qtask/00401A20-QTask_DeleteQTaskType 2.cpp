// IGI native provenance: QTask_DeleteQTaskType - 0x00401A20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00401A20-QTask_DeleteQTaskType.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "QTask_DeleteQTaskType.h"
unsigned int QTask_DeleteQTaskType(short context)

{
    unsigned int result = context;
    unsigned int source_1 = result * 0x30;
    result = *(source_1 + 0xaf5f04);

    if (result)
        result = Sys_OpenFileStream_2(source_1 + 0xaf5eec);

    *(source_1 + &g_af5f05) = 0;
    return result;
}
