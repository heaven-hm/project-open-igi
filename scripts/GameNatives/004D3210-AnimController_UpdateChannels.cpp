// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: AnimController_UpdateChannels
// Retail VA: 0x004d3210
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(int32_t arg1, int32_t* arg2)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_a542f0 -> g_anim_mesh_counts
// Readable signature: int32_t AnimController_UpdateChannels(int32_t render_context, int32_t* channel_set);

int32_t AnimController_UpdateChannels(int32_t render_context, int32_t* channel_set)

{
    int32_t draw_result = 0;
    int32_t mesh_count = *(*channel_set * 0xc + &g_anim_mesh_counts);

    if (mesh_count <= 0)
        return 0;

    int32_t mesh_offset = 0;
    int32_t remaining = mesh_count;
    bool has_more;

    do
    {
        void* mesh = mesh_offset + channel_set[2];
        void* mesh_resource = *(mesh + 0x80);

        if (mesh_resource)
        {
            *(mesh + 4) &= 0xfffeffff;
            draw_result = Render_DrawRigidMesh_6(render_context, draw_result, mesh, mesh_resource);
            Render_BindTexture_6(mesh);

            if (!*(mesh + 0x80))
                *(mesh + 4) &= 0xfff87eff;
        }

        mesh_offset += 0x84;
        has_more = remaining != 1;
        remaining -= 1;
    } while (has_more);
    return draw_result;
}
