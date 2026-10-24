// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Mem_AllocFn
// Retail VA: 0x004b0c60
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: void*(int32_t arg1, int32_t arg2)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_942300 -> g_live_alloc_count
//   g_9422fc -> g_heap_initialized
//   g_a960f0 -> g_heap_list_head
// Readable signature: void* Mem_AllocFn(int32_t num_bytes, int32_t alignment);

void* Mem_AllocFn(int32_t num_bytes, int32_t alignment)

// Sig: void* Mem_Alloc(int numBytes,int tag)
// Note: Engine heap allocator. Params: numBytes = bytes to allocate, tag =
// allocation bucket shown in the 'Total allocated by user' accounting. RETURNS
// pointer or 0 on failure (error text: 'Mem_Alloc() failed to allocate %d
// bytes'). Pair with MemoryDealloc/MemDealloc free path.
// Readability note: the second parameter is used as a power-of-two alignment
// mask below (`& ~(alignment - 1)`), so it is named alignment here.
{
    g_live_alloc_count += 1;
    void** chunk = Mem_AllocChunk(num_bytes + alignment + 0x1c);

    if (chunk)
    {
        void* user_ptr = (chunk + alignment + 0x1b) & ~(alignment - 1);
        *((user_ptr - 4) & 0xfffffffc) = chunk;
        chunk[1] = 0;
        *chunk = nullptr;

        if (!g_heap_initialized)
            Mem_InitHeapState();

        HeapList_Insert(&g_heap_list_head, chunk);
        chunk[2] = user_ptr;
        chunk[3] = num_bytes + alignment + 0x1c;
        chunk[4] = num_bytes;
        return user_ptr;
    }

    int32_t* list_node = g_heap_list_head;
    int32_t edx = 0;
    int32_t* i = *list_node;

    while (i)
    {
        int32_t node_bytes = list_node[3];
        int32_t node_tag = list_node[4];
        list_node = i;
        i = *i;
        edx += node_bytes;
        chunk += node_tag;
    }

    int32_t total_allocated = edx;
    void** failed_chunk = chunk;
    int32_t* requested_size = num_bytes + alignment + 0x1c;
    Debug_Error("Mem_Alloc() failed to allocate %d bytes.\r\nTotal allocated by user: %d.\r\nTotal "
    "by Mem_Alloc():%d.");

    while (true)
        /* nop */
}
