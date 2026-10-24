// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Transcontext_SetActiveTransContext
// Retail VA: 0x00497e70
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(int32_t* arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_6e5ba4 -> g_render_pass_index
//   g_a94e84 -> g_render_callbacks
//   g_bcaae0 -> g_active_trans_context
//   g_6e5bb0 -> g_transition_active
//   g_6e5bb4 -> g_transition_generation
// Readable signature: int32_t Transcontext_SetActiveTransContext(int32_t* transform_context);

int32_t Transcontext_SetActiveTransContext(int32_t* transform_context)

// Sig: void Transcontext_SetActiveTransContext(void* ctx)
// Note: Installs a TRANSFORM CONTEXT as active: copies 0x2A dwords from ctx
// into global 0xBCAAE0 and bumps state counters at 0x6E5BB0/B4. Param: ctx =
// transform-context struct (IGIPatch uses this around custom rendering). Use
// when injecting your own render passes.
{
    int32_t* source_context = transform_context;
    int32_t render_callback = *((g_render_pass_index << 2) + &g_render_callbacks);
    // BN reuses the argument slot as a pointer to the installed active context.
    int32_t* active_context_ptr = &g_active_trans_context;
    __builtin_memcpy(&g_active_trans_context, source_context, 0xa8);

    if (render_callback)
        render_callback(&active_context_ptr);

    int32_t previous_generation = g_transition_generation;
    g_transition_active = 1;
    g_transition_generation = previous_generation + 1;
    return previous_generation + 1;
}
