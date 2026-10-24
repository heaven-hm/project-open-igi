// Native: ListBox_AddItem
// Retail VA: 0x41f2d0
// Source: Binary Ninja MCP decompile
// Signature: int32_t(int32_t arg1, int32_t arg2)

  int32_t ListBox_AddItem(int32_t arg1, int32_t arg2)

{
    void* 57bbd8_1 = g_57bbd8;
    int32_t eax = *(57bbd8_1 + 0xbe0);
    int32_t ecx_2 = 0;

    if (eax > 0)
    {
        int32_t* edx = 57bbd8_1 + 0xe0;

        do
        {
            if (*edx == arg2)
                return eax;

            ecx_2 += 1;
            edx = &edx[0xb];
        } while (ecx_2 < eax);
    }

    *(57bbd8_1 + eax * 0x2c + 0xe0) = arg2;
    int32_t edi_1 = arg1;
    int32_t i = 0xffffffff;

    while (i)
    {
        bool cond:0_1 = 0 != *edi_1;
        edi_1 += 1;
        i -= 1;

        if (!cond:0_1)
            break;
    }

    int32_t ecx_3 = ~i;
    int32_t esi_2;
    int32_t edi_4;
    edi_4 = __builtin_memcpy(57bbd8_1 + eax * 0x2c + 0xe4, edi_1 - ecx_3, ecx_3 & 0xfffffffc);
    __builtin_memcpy(edi_4, esi_2, ecx_3 & 3);
    *(57bbd8_1 + 0xbe0) += 1;
    return ecx_3;
}
