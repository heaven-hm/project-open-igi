// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Mem_DeAllocFn
// Retail VA: 0x004b0d10
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(void* arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_942300 -> g_live_alloc_count
// Readable signature: int32_t Mem_DeAllocFn(void* user_ptr);

int32_t Mem_DeAllocFn(void* user_ptr)

{
    int32_t* header_ptr = user_ptr - 4;
    header_ptr &= 0xfc;
    g_live_alloc_count -= 1;
    int32_t* chunk = *header_ptr;
    int32_t* chunk_copy = chunk;
    return Camera_UpdateTransform_9(HeapList_Remove(chunk));
}
