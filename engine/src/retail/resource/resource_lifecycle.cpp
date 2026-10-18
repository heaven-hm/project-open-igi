#include "igi/retail/resource/resource_lifecycle.hpp"

#include "igi/retail/error/error_show.hpp"
#include "igi/retail/qfile/alias_resolve.hpp"

namespace igi::retail::resource {
namespace {

SysFreeMemory16Fn g_sys_free_memory_16 = nullptr;
ResourcePackUnpackFn g_resource_pack_unpack = nullptr;
SysResourceFn g_sys_resource = nullptr;

} // namespace

// Native: Resource_Unload; retail VA 0x004B6380; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x004B6380-0x004B63C3, 67 bytes, digest 14f9f39cad1672b7ab5c4799581eff2192c542b44cb75b36f6e263c4c74a0345; CLEAN/resource/004B6380-Resource_Unload.c.
// Behavior/CFG: probes Is_ResourceLoaded(resource_path, &node) (0x004B5B90); when found, reads `node->kind` at offset +0x24 (`entity_val[9]`); if `kind != 3 && kind != 1`, calls Sys_FreeMemory_16(node) (0x004B5C90); otherwise returns `kind`. When not found, invokes Error_Show("Resource_Unload() : Resource '%s' is not loaded."). radare2 7 blocks/9 edges; Binary Ninja 7 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact control flow, offset +0x24 check, and callee dispatch.
std::int32_t Resource_Unload(const char* resource_path)
{
    ResourceHashNode* entity_val = nullptr;
    if (Is_ResourceLoaded(resource_path, &entity_val) != 0)
    {
        const std::int32_t result = entity_val->kind;
        if (result != 3 && result != 1)
        {
            if (g_sys_free_memory_16 != nullptr)
            {
                return g_sys_free_memory_16(entity_val);
            }
        }
        return result;
    }

    const auto error_result = error::Error_Show("Resource_Unload() : Resource '%s' is not loaded.", resource_path);
    if constexpr (sizeof(void*) == 4)
    {
        // Retail 0x004B63C1 spins if the fatal-error callback returns.
        for (;;) {}
    }
    return error_result;
}

// Native: Resource_Load; retail VA 0x004B5F00; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B5F00-0x004B5F7D, 125 bytes, digest 3250eca00c444e0774a6c8ca33df90e7bd63930021d542840e463b93421aa1d6; CLEAN/resource/004B5F00-Resource_Load.c.
// Behavior/CFG: probes Is_ResourceLoaded(resource_path, &node) (0x004B5B90); if cached, writes `*(node + 0x28)` to `*out_size` when `out_size != nullptr`, sets `*(node + 0x2C) = 1`, and returns `*(node + 0x34)`. Otherwise calls Resource_PackUnpack(resource_path, &size) (0x004B18B0); when non-null, writes `size` to `*out_size` (if non-null) and returns Sys_Resource(QFile_GetResolvedAlias(), data, size) (0x004B5F80). When null, calls Error_Show("Failed to load resource: '%s'"). radare2 10 blocks/12 edges; Binary Ninja 10 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact cache hit/miss branches and node offsets (+0x28, +0x2C, +0x34).
void* Resource_Load(const char* resource_path, std::int32_t* out_size)
{
    ResourceHashNode* entity_val = nullptr;
    if (Is_ResourceLoaded(resource_path, &entity_val) != 0)
    {
        if (out_size != nullptr)
        {
            *out_size = entity_val->size;
        }
        entity_val->loaded_flag = 1;
        return entity_val->data;
    }

    std::int32_t item_4 = 0;
    void* data_4 =
        g_resource_pack_unpack != nullptr ? g_resource_pack_unpack(resource_path, &item_4) : nullptr;
    if (data_4 != nullptr)
    {
        if (out_size != nullptr)
        {
            *out_size = item_4;
        }
        if (g_sys_resource != nullptr)
        {
            return g_sys_resource(qfile::QFile_GetResolvedAlias(), data_4, item_4);
        }
        return data_4;
    }

    error::Error_Show("Failed to load resource: '%s'", resource_path);
    if constexpr (sizeof(void*) == 4)
    {
        // Retail 0x004B5F5A spins if the fatal-error callback returns.
        for (;;) {}
    }
    return nullptr;
}

void Host_SetSysFreeMemory16(SysFreeMemory16Fn fn)
{
    g_sys_free_memory_16 = fn;
}

void Host_SetResourcePackUnpack(ResourcePackUnpackFn fn)
{
    g_resource_pack_unpack = fn;
}

void Host_SetSysResource(SysResourceFn fn)
{
    g_sys_resource = fn;
}

} // namespace igi::retail::resource
