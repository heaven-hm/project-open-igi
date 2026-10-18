// IGI native provenance: Display_RestoreDevice_18 - 0x0049A450 - CFG score 86.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 0049A450-Display_RestoreDevice_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_RestoreDevice_18.hpp"
struct status_msg* Display_RestoreDevice_18(unsigned int context, void* arg2, int arg3, int arg4, unsigned int arg5, unsigned int arg6, unsigned int arg7, unsigned int arg8, unsigned int arg9, char* arg10, unsigned int arg11)

{
    int kind;
    kind = g_543c64;
    struct status_msg* result = QTask_HandleInput(arg2, kind, 0);
    result->head[0x1a] = context;
    result->head[0x1b] = arg5;
    result->head[0x27] = arg6;
    result->head[0x28] = arg7;
    result->head[0x2a] = arg8;
    result->head[0x29] = arg9;
    result->text = arg10;
    result->head[0x2b] = arg11;
    result->head[0x26] = 0;

    if (!arg2)
    {
        result->sprite[0] = 0;
        result->sprite[1] = 0;
        result->sprite[2] = 0;
        result->sprite[3] = 0;
    }
    else
    {
        int* data_2 = QTask_LinkEventHandler_2(arg2);
        result->sprite[0] = data_2;
        result->sprite[1] = *(&data_2 + 1);
        result->sprite[2] = *(&data_2 + 2);
        result->sprite[3] = *(&data_2 + 3);
    }

    memcpy(&result->head[8], arg3, 0x18);
    memcpy(&result->head[0x1c], arg4, 0x28);

    if (arg8)
        DynObj_RegisterSymbol_8(context, result, 0);

    return result;
}
