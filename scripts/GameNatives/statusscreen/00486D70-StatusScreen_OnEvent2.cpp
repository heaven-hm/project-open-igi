// IGI native provenance: StatusScreen_OnEvent2 - 0x00486D70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00486D70-StatusScreen_OnEvent2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00486D70-StatusScreen_OnEvent2.hpp"
int StatusScreen_OnEvent2(void* self)

{
    Sys_ReadFileBlock_18(*(self + 0x4dc));
    Sys_ReadFileBlock_18(*(self + 0x4e0));
    Sys_ReadFileBlock_17(self + 0xa4);
    Sys_ReadFileBlock_17(self + 0x8c);
    Sys_ReadFileBlock_17(self + 0xd4);
    Sys_ReadFileBlock_17(self + 0xec);
    Sys_ReadFileBlock_17(self + 0x104);
    Sys_ReadFileBlock_17(self + 0x11c);
    Sys_ReadFileBlock_17(self + 0x134);
    Sys_ReadFileBlock_17(self + 0x14c);
    Sys_ReadFileBlock_17(self + 0x74);
    Sys_ReadFileBlock_17(self + 0x1c4);
    Sys_ReadFileBlock_17(self + 0x1dc);
    Sys_ReadFileBlock_17(self + 0x1f4);
    Sys_ReadFileBlock_17(self + 0x20c);
    Sys_ReadFileBlock_17(self + 0x224);
    Sys_ReadFileBlock_17(self + 0x23c);
    Sys_ReadFileBlock_17(self + 0x254);
    Sys_ReadFileBlock_17(self + 0x26c);
    Sys_ReadFileBlock_17(self + 0x284);
    Sys_ReadFileBlock_17(self + 0x29c);
    Sys_ReadFileBlock_17(self + 0x2b4);
    Sys_ReadFileBlock_17(self + 0x2cc);
    Sys_ReadFileBlock_17(self + 0x2e4);
    Sys_ReadFileBlock_17(self + 0x2fc);
    Sys_ReadFileBlock_17(self + 0x314);
    Sys_ReadFileBlock_17(self + 0x32c);
    Sys_ReadFileBlock_17(self + 0x344);
    Sys_ReadFileBlock_17(self + 0x35c);
    Sys_ReadFileBlock_17(self + 0x374);
    int* dest_ptr = self + 0x38c;
    int i_2 = 4;
    int i;

    do
    {
        Sys_ReadFileBlock_17(dest_ptr);
        dest_ptr = &dest_ptr[6];
        i = i_2;
        i_2 -= 1;
    } while (i != 1);
    int* dest_ptr_1 = self + 0x3ec;
    int i_3 = 0xa;
    int i_1;

    do
    {
        Sys_ReadFileBlock_17(dest_ptr_1);
        dest_ptr_1 = &dest_ptr_1[6];
        i_1 = i_3;
        i_3 -= 1;
    } while (i_1 != 1);
    Resource_Flush(*(self + 0x70));
    return Display_SwapBuffers_13(self + 0x20);
}
