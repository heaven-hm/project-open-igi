// IGI native provenance: Env_ActivateEntity_12 - 0x0044F380 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044F380-Env_ActivateEntity_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0044F380-Env_ActivateEntity_12.hpp"
void* Env_ActivateEntity_12(void* context)

{
    int i = 0;
    *(context + 0x818) = context + 0x810;
    int* dest_val = context + 0x87c;
    *(context + 0x810) = context + 0x814;
    *(context + 0x814) = 0;
    int* data_1 = context + 0x6f0;

    do
    {
        i += 1;
        *data_1 = &data_1[1];
        data_1[2] = data_1;
        data_1[1] = 0;
        *(context + i + 0x8e3) = 0;
        *dest_val = 0;
        dest_val = &dest_val[1];
        data_1 = &data_1[3];
    } while (i < 0x18);

    int* dest_1 = context + 0xf0;
    int i_2 = 0x20;
    void* result;
    int i_1;

    do
    {
        dest_1[1] = 0;
        *dest_1 = 0;
        result = HeapList_Insert(context + 0x810, dest_1);
        dest_1 = &dest_1[0xc];
        i_1 = i_2;
        i_2 -= 1;
    } while (i_1 != 1);
    *(context + 0x8e0) = 0;
    return result;
}
