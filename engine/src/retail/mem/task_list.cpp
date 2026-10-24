// Native: Mem_AllocIndexedBlock (placeholder name until VERIFIED; retail BN alias Sys_FreeMemory_2 is wrong - this allocates).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0B00-0x004B0B64.
// Tools/evidence: Binary Ninja MCP disassembly/decompile + radare2 6.2.0 bytes; 5 blocks, callee 0x004B0C60 Mem_Alloc; spec build/audit/main-0x48F530/level1-closure/20261024T023630Z/NEXT-COMMITS-SPEC.md §1.3.
// Behavior/CFG: alignment=max(arg4,4); stride=element_size rounded up to alignment; descriptor[5]=element_size stored before allocating; base=Mem_Alloc((stride+4)*count, alignment); descriptor = [0]=base, [1]=base+stride*count (index table), [2]=0, [3]=count, [4]=stride, [5]=element_size; index_table[i]=i; returns count (0 when count<=0).
// Status/limits: TRANSCRIBED; no runtime evidence; only transcribed caller will be TaskType_Register 0x401900.
#include "igi/retail/mem/task_list.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include <cstdint>

extern "C" {

std::int32_t Mem_AllocIndexedBlock(std::int32_t* descriptor, std::int32_t element_size,
                                          std::int32_t count, std::int32_t alignment)
{
    // 0x004B0B00-0x004B0B0E: alignment = max(alignment, 4).
    if (alignment < 4)
    {
        alignment = 4;
    }
    // 0x004B0B19-0x004B0B2A: stride = (element_size + alignment + 3) & ~(alignment - 1).
    const std::int32_t stride = (element_size + alignment + 3) & ~(alignment - 1);
    // 0x004B0B1D: descriptor[5] = element_size (stored before the allocation).
    descriptor[5] = element_size;
    // 0x004B0B2C-0x004B0B33: Mem_Alloc((stride+4)*count, alignment).
    void* const base = igi::retail::mem::Mem_Alloc((stride + 4) * count, alignment);
    descriptor[0] = reinterpret_cast<std::int32_t>(base);
    // 0x004B0B3F-0x004B0B51: descriptor[1]=base+stride*count, [2]=0, [3]=count, [4]=stride.
    descriptor[1] = reinterpret_cast<std::int32_t>(static_cast<std::uint8_t*>(base) + stride * count);
    descriptor[3] = count;
    descriptor[4] = stride;
    descriptor[2] = 0;
    // 0x004B0B46-0x004B0B5F: index_table[i] = i for i in 0..count-1.
    std::int32_t result = 0;
    if (count > 0)
    {
        std::int32_t* const index_table = reinterpret_cast<std::int32_t*>(descriptor[1]);
        do
        {
            index_table[result] = result;
            ++result;
        } while (result < count);
    }
    return result;
}

} // extern "C"
