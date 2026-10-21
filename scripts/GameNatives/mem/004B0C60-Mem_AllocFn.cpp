// IGI native provenance: Mem_Alloc - 0x004B0C60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0C60-Mem_Alloc.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Mem_Alloc.hpp"
void* Mem_Alloc(int context, int arg2)

// Sig: void* Mem_Alloc(int numBytes,int tag)
// Note: Engine heap allocator. Params: numBytes = bytes to allocate, tag =
// allocation bucket shown in the 'Total allocated by user' accounting. RETURNS
// pointer or 0 on failure (error text: 'Mem_Alloc() failed to allocate %d
// bytes'). Pair with MemoryDealloc/MemDealloc free path.
{
    g_942300 += 1;
    void** source_2 = Mem_AllocChunk(context + arg2 + 0x1c);

    if (source_2)
    {
        void* result = (source_2 + arg2 + 0x1b) & ~(arg2 - 1);
        *((result - 4) & 0xfffffffc) = source_2;
        source_2[1] = 0;
        *source_2 = nullptr;

        if (!g_9422fc)
            Mem_InitHeapState();

        HeapList_Insert(&g_a960f0, source_2);
        source_2[2] = result;
        source_2[3] = context + arg2 + 0x1c;
        source_2[4] = context;
        return result;
    }

    int* i_1 = g_a960f0;
    int temp_val = 0;
    int* i = *i_1;

    while (i)
    {
        int dest_1 = i_1[3];
        int context_1 = i_1[4];
        i_1 = i;
        i = *i;
        temp_val += dest_1;
        source_2 += context_1;
    }

    int item_14_1 = temp_val;
    void** item_18 = source_2;
    int* item_1c = context + arg2 + 0x1c;
    Error_Show("Mem_Alloc() failed to allocate %d bytes.\r\nTotal allocated by user: %d.\r\nTotal by "
    "Mem_Alloc():%d.");

    while (true)
        /* nop */
}
