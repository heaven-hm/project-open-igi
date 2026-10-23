// IGI native provenance: AIFunction_SendResponse - 0x0044EE40 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044EE40-AIFunction_SendResponse.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIFunction_SendResponse.h"
int AIFunction_SendResponse(int context)

// Sig: void AIFunction_SendResponse()
// Note: Sends the AI's RESPONSE to the current event (used for ALERT_RESPONSE
// chains so buddy AIs react too). Takes no args. Call inside alert handlers.
{
    int* data_val = **&g_5bda60;

    if (*data_val)
    {
        AI_UpdateOrders(*data_val[0xa], 6, 0x24);
        int item_18 = 0;
        int item_10 = 0;
        int item_c = 0;
        char* item_8 = &g_567c74;
        memcpy(context, &item_18, 0x18);
        return context;
    }

    Error_Show("No EVENT to respond");

    while (true)
        /* nop */
}
