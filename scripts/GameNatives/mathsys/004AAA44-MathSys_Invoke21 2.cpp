// IGI native provenance: MathSys_Invoke21 - 0x004AAA44 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AAA44-MathSys_Invoke21.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MathSys_Invoke21.h"

int __convention("regparm") MathSys_Invoke21(int arg1, int arg2, int arg3, int* arg4, void* arg5, void* arg6, int arg7)

{
    *(arg6 + (arg3 << 2) + 0x1c) = *(arg5 + (arg3 << 2) + 0x1c);
    *(arg6 + (arg3 << 2) + 0x18) = *(arg5 + (arg3 << 2) + 0x18);
    *(arg6 + (arg3 << 2) + 0x14) = *(arg5 + (arg3 << 2) + 0x14);
    *(arg6 + (arg3 << 2) + 0x10) = *(arg5 + (arg3 << 2) + 0x10);
    *(arg6 + (arg3 << 2) + 0xc) = *(arg5 + (arg3 << 2) + 0xc);
    *(arg6 + (arg3 << 2) + 8) = *(arg5 + (arg3 << 2) + 8);
    *(arg6 + (arg3 << 2) + 4) = *(arg5 + (arg3 << 2) + 4);
    int eax_6 = arg3 << 2;
    void* source_ptr = arg5 + eax_6;
    void* dest_ptr = arg6 + eax_6;

    switch (arg2)
    {
        case 0:
        {
            *arg4;
            return arg4[2];
            break;
        }
        case 1:
        {
            *(dest_ptr + 3) = *(source_ptr + 3);
            *arg4;
            return arg4[2];
            break;
        }
        case 2:
        {
            *(dest_ptr + 3) = *(source_ptr + 3);
            *(dest_ptr + 2) = *(source_ptr + 2);
            *arg4;
            return arg4[2];
            break;
        }
        case 3:
        {
            *(dest_ptr + 3) = *(source_ptr + 3);
            *(dest_ptr + 2) = *(source_ptr + 2);
            *(dest_ptr + 1) = *(source_ptr + 1);
            *arg4;
            return arg4[2];
            break;
        }
    }
}
