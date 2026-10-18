#include "igi/retail/sys/ilff_navigation.hpp"

#include <cstddef>

namespace igi::retail::sys {
namespace {

// Global 0x00A43E5C.
std::int32_t ilff_error_flag = 0;

[[nodiscard]] std::int32_t read_i32(const std::uint8_t* base, std::size_t offset)
{
    std::int32_t value = 0;
    const auto* bytes = base + offset;
    value = static_cast<std::int32_t>(static_cast<std::uint32_t>(bytes[0])
                                      | (static_cast<std::uint32_t>(bytes[1]) << 8U)
                                      | (static_cast<std::uint32_t>(bytes[2]) << 16U)
                                      | (static_cast<std::uint32_t>(bytes[3]) << 24U));
    return value;
}

// The retail alignment check: `(~(alignment - 1) & alignment) != alignment`.
[[nodiscard]] bool is_power_of_two(std::uint32_t alignment)
{
    return (~(alignment - 1) & alignment) == alignment;
}

} // namespace

// Native: Ilff_GetNextChunk; retail VA 0x004B7470; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.
// Behavior/CFG: not restated here.
// Status/limits: STRUCTURALLY_VERIFIED; see verification record for scope and port limits.
// Previous catalog alias: Sys_PopStack_19.
void* Ilff_GetNextChunk(void* context)
{
    if (context != nullptr)
    {
        auto* base = static_cast<std::uint8_t*>(context);
        const std::uint32_t next_offset = static_cast<std::uint32_t>(read_i32(base, 0x0C));
        if (next_offset != 0U)
        {
            return base + next_offset;
        }
    }
    return nullptr;
}

// Native: Ilff_GetFirstChunk; retail VA 0x004B7490; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.
// Behavior/CFG: not restated here.
// Status/limits: STRUCTURALLY_VERIFIED; see verification record for scope and port limits.
// Previous catalog alias: Sys_ResolveSymbol_19.
void* Ilff_GetFirstChunk(void* context)
{
    if (context != nullptr)
    {
        auto* base = static_cast<std::uint8_t*>(context);
        const std::uint32_t declared_size = static_cast<std::uint32_t>(read_i32(base, 0x04));
        if (declared_size != 0)
        {
            const std::uint32_t alignment = static_cast<std::uint32_t>(read_i32(base, 0x08));
            if (!is_power_of_two(alignment))
            {
                ilff_error_flag = 1;
            }
            else
            {
                const std::uint32_t child_offset = (alignment + 0x13U) & ~(alignment - 1U);
                if (child_offset < declared_size)
                {
                    return base + child_offset;
                }
            }
        }
    }
    return nullptr;
}

// Native: Ilff_GetChunkPayload; retail VA 0x004B74D0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.
// Behavior/CFG: not restated here.
// Status/limits: STRUCTURALLY_VERIFIED; see verification record for scope and port limits.
// Previous catalog alias: Sys_CheckHeap_19.
void* Ilff_GetChunkPayload(void* context, void* arg2)
{
    if (arg2 != nullptr && context != nullptr)
    {
        auto* child_base = static_cast<std::uint8_t*>(context);
        auto* parent_base = static_cast<std::uint8_t*>(arg2);

        const std::uint32_t parent_alignment = static_cast<std::uint32_t>(read_i32(parent_base, 0x08));
        if (!is_power_of_two(parent_alignment))
        {
            ilff_error_flag = 1;
        }
        else
        {
            const std::uint32_t relative = static_cast<std::uint32_t>(child_base - parent_base);
            if (relative >= static_cast<std::uint32_t>(read_i32(parent_base, 0x04)))
            {
                ilff_error_flag = 1;
            }
            else
            {
                const std::uint32_t child_alignment = static_cast<std::uint32_t>(read_i32(child_base, 0x08));
                if (!is_power_of_two(child_alignment) || child_alignment > parent_alignment)
                {
                    ilff_error_flag = 1;
                }
                else if (read_i32(child_base, 0x04) != 0)
                {
                    const std::uint32_t payload =
                        (child_alignment + relative + 0x0FU) & ~(child_alignment - 1U);
                    return parent_base + payload;
                }
            }
        }
    }
    return nullptr;
}

// Native: Ilff_ReadContainerHeader; retail VA 0x004B7540; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B7540-0x004B7563, 35 bytes, digest 05157574f9770289393bb031702b9357e0efd4fbb7abd48d65be942ffd27e94f.
// Behavior/CFG: if `context` is non-null, writes chunk field +0x04 to `*out_size` (when non-null) and chunk field +0x08 to `*out_alignment` (when non-null). radare2 6 blocks/8 edges; Binary Ninja 6 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact field offsets and null guards.
// Previous catalog alias: Sys_AllocateMemory_20.
void Ilff_ReadContainerHeader(std::int32_t* out_size, std::int32_t* out_alignment, const void* context)
{
    if (context == nullptr)
    {
        return;
    }

    const auto* const base = static_cast<const std::uint8_t*>(context);
    if (out_size != nullptr)
    {
        *out_size = read_i32(base, 0x04);
    }
    if (out_alignment != nullptr)
    {
        *out_alignment = read_i32(base, 0x08);
    }
}

// Native: Ilff_GetFormType; retail VA 0x004B7570; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B7570-0x004B757F, 15 bytes, digest df8f7845ad39f9850ed36bbb4fc492c99e7163622d8ecf2d9561e1a5f6fc46dd.
// Behavior/CFG: returns the 32-bit form FourCC at `context + 0x10` when `context` is non-null, otherwise 0. radare2 3 blocks/2 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact instruction behavior.
// Previous catalog alias: Sys_FreeMemory_20.
std::int32_t Ilff_GetFormType(const void* context)
{
    if (context == nullptr)
    {
        return 0;
    }
    return read_i32(static_cast<const std::uint8_t*>(context), 0x10);
}

// Native: Ilff_GetChunkTag; retail VA 0x004B7580; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B7580-0x004B758E, 14 bytes, digest 26690bbe535ad6119bdd37bf2f477a8f95d7f34860c3b971e777e17e835fc784.
// Behavior/CFG: returns the 32-bit chunk tag FourCC at `context + 0x00` when `context` is non-null, otherwise 0. radare2 3 blocks/2 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact instruction behavior.
// Previous catalog alias: Sys_OpenFileStream_20.
std::int32_t Ilff_GetChunkTag(const std::int32_t* context)
{
    if (context == nullptr)
    {
        return 0;
    }
    return *context;
}

// Native: Ilff_FindChunk; retail VA 0x004B7590; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.
// Behavior/CFG: not restated here.
// Status/limits: STRUCTURALLY_VERIFIED; see verification record for scope and port limits.
// Previous catalog alias: Sys_ReadFileBlock_20.
std::int32_t* Ilff_FindChunk(std::int32_t* context, std::int32_t tag)
{
    for (auto* chunk = context; chunk != nullptr;
         chunk = static_cast<std::int32_t*>(Ilff_GetNextChunk(chunk)))
    {
        if (*chunk == tag)
        {
            return chunk;
        }
    }
    return nullptr;
}

std::int32_t Ilff_GetErrorFlag()
{
    return ilff_error_flag;
}

void Ilff_ClearErrorFlag()
{
    ilff_error_flag = 0;
}

} // namespace igi::retail::sys
