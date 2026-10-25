// Native: Config_GetActiveGraphicOptions; retail VA 0x00404590; signature/calling convention x86 cdecl void*(void).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/config/config_graphic_options.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x00404590-binaryninja.json and 0x00404590-radare2.json; unicorn 2.1.4 execution of the retail bytes against this candidate in scripts/native_unicorn_runner.py.
// Behavior/CFG: One basic block, no calls and no branches. ecx = dword [0x00BC2384] (the active profile index); eax = ecx + ecx*2; eax <<= 3; eax -= ecx; eax += eax*2; eax += eax*4; lea eax, [eax*4 + 0x00BC2394], which is the index * 0x564 stride plus the profile record base. Returns that pointer. The port returns the active record of its host store, whose index is clamped to the port's profile capacity instead of the unchecked retail multiply.
// Status/limits: STRUCTURALLY_VERIFIED against the retail instruction sequence; the stride arithmetic is reproduced by the store layout and the frozen profile count is recorded as a limitation.
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Config_GraphicOptionsSetPerformanceFlags; retail VA 0x00404F00; signature/calling convention x86 cdecl void(void* context, const void* script_value).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/config/config_graphic_options.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x00404F00-binaryninja.json and 0x00404F00-radare2.json; unicorn 2.1.4 execution of the retail bytes against this candidate in scripts/native_unicorn_runner.py.
// Behavior/CFG: One basic block with two calls (Script_GetArg 0x004B8A20 then Config_GetActiveGraphicOptions 0x00404590). The resolved integer is stored at record+0x0C, then the value block {int32 1, undefined +0x04, float64 1.0 via the 0x3FF00000 high dword, type 0x00567C74} is built on the stack and copied to the context with rep movsd x6. The host interface takes the integer already resolved because the port has no script VM at this boundary.
// Status/limits: STRUCTURALLY_VERIFIED with live retail-byte execution for the store and the block build; the fixture stubs Script_GetArg, which is the documented ABI boundary of the adapted interface.
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Config_GraphicOptionsGetPerformanceFlags; retail VA 0x00404F60; signature/calling convention x86 cdecl void(void* context).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/config/config_graphic_options.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x00404F60-binaryninja.json and 0x00404F60-radare2.json; unicorn 2.1.4 execution of the retail bytes against this candidate in scripts/native_unicorn_runner.py.
// Behavior/CFG: One basic block with two calls to Config_GetActiveGraphicOptions. The integer slot takes dword [record+0x0C]; the double slot takes fild dword [record+0x0C] (integer converted to float64); the type slot is 0x00567C74; the block is copied to the context with rep movsd x6. The +0x04 slot is never written by retail.
// Status/limits: STRUCTURALLY_VERIFIED with live retail-byte execution; the +0x04 slot is stack residue in retail and the fixture's zeroed stack makes both sides read zero.
// Native: Config_GraphicOptionsNumPerfLevels; retail VA 0x00405620; signature/calling convention x86 cdecl void(void* context).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/config/config_graphic_options.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x00405620-binaryninja.json and 0x00405620-radare2.json; unicorn 2.1.4 execution of the retail bytes against this candidate in scripts/native_unicorn_runner.py.
// Behavior/CFG: One basic block, no calls and no branches. Builds the value block {int32 7, undefined +0x04, float64 7.0 via the 0x401C0000 high dword, type 0x00567C74} and copies 24 bytes to the context.
// Status/limits: STRUCTURALLY_VERIFIED with live retail-byte execution; the constant is seven performance levels, matching the seven-entry flag table at 0x00537B74.
// Native: Config_GraphicOptionsGetPerfFlagsFromLevel; retail VA 0x004055C0; signature/calling convention x86 cdecl void(void* context, const void* script_value).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/config/config_graphic_options.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x004055C0-binaryninja.json and 0x004055C0-radare2.json; unicorn 2.1.4 execution of the retail bytes against this candidate in scripts/native_unicorn_runner.py.
// Behavior/CFG: One basic block with two Script_GetArg calls. The level resolves twice from the same argument, the flag bitmask comes from dword [level*4 + 0x00537B74], the integer slot takes that bitmask, the double slot takes fild dword of the same table entry, the type slot is 0x00567C74, and the block is copied to the context. Retail indexes the seven-dword table without a bounds check; the port clamps the level into the table range.
// Status/limits: STRUCTURALLY_VERIFIED with live retail-byte execution for levels 0..6; out-of-range levels would read memory past 0x00537B90 in retail and are clamped by the port instead, which is recorded as a limitation.
// Native: Config_GraphicOptionsGetPerfLevelFromFlags; retail VA 0x00405530; signature/calling convention x86 cdecl void(void* context, const void* script_value).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/config/config_graphic_options.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x00405530-binaryninja.json and 0x00405530-radare2.json; unicorn 2.1.4 execution of the retail bytes against this candidate in scripts/native_unicorn_runner.py.
// Behavior/CFG: Four blocks, one call (Script_GetArg) and a table loop. The best-distance slot starts at 0x1F; for each of the seven table dwords the body computes the 32-iteration popcount of table[i] ^ value with the inner loop at 0x00405557, and when that is strictly below the best it records the distance and the current level in its argument slot. The tail converts the winning level with fild dword / fld st0 / fistp qword, stores the low dword as the block integer, stores the same level as float64, stamps type 0x00567C74, and copies the block to the context. The port initialises the winning level to zero because retail only leaves the argument slot untouched when no entry wins within 31 bits, which is unreachable for every 32-bit input (the first entry alone gives a distance at most 27).
// Status/limits: STRUCTURALLY_VERIFIED with live retail-byte execution on flag values whose nearest level is well defined; the unreachable no-winner path is documented above and the +0x04 slot is zero in both worlds.

#include "igi/retail/config/config_graphic_options.hpp"

#include <algorithm>
#include <cstring>

namespace igi::retail::config {
namespace {

void store_value_block(void* context, const ScriptValueBlock& block) noexcept
{
    // Retail builds the block on the stack and copies it with rep movsd x6.
    std::memcpy(context, &block, sizeof(block));
}

std::uint32_t population_count(std::uint32_t value) noexcept
{
    std::uint32_t count = 0;
    for (int bit = 0; bit < 32; ++bit)
    {
        count += value & 1U;
        value >>= 1U;
    }
    return count;
}

} // namespace

std::int32_t GraphicOptionsRecord::performance_flags() const noexcept
{
    std::int32_t value = 0;
    std::memcpy(&value, bytes.data() + performance_flags_offset, sizeof(value));
    return value;
}

void GraphicOptionsRecord::set_performance_flags(std::int32_t flags) noexcept
{
    std::memcpy(bytes.data() + performance_flags_offset, &flags, sizeof(flags));
}

GraphicOptionsStore& GraphicOptionsStore::active() noexcept
{
    static GraphicOptionsStore store;
    return store;
}

void GraphicOptionsStore::set_active_index(std::uint32_t index) noexcept
{
    active_index_ = index < profile_capacity ? index : profile_capacity - 1U;
}

GraphicOptionsRecord& GraphicOptionsStore::record(std::uint32_t index) noexcept
{
    return records_[index < profile_capacity ? index : profile_capacity - 1U];
}

const GraphicOptionsRecord& GraphicOptionsStore::record(std::uint32_t index) const noexcept
{
    return records_[index < profile_capacity ? index : profile_capacity - 1U];
}

GraphicOptionsRecord* Config_GetActiveGraphicOptions() noexcept
{
    // Retail: index * 0x564 + 0x00BC2394 computed with lea/shl/sub chains; the store owns the
    // equivalent arithmetic through its record array.
    GraphicOptionsStore& store = GraphicOptionsStore::active();
    return &store.record(store.active_index());
}

void Config_GraphicOptionsSetPerformanceFlags(void* context, std::int32_t flags) noexcept
{
    // Retail resolves the value with Script_GetArg(arg3, 0) and stores the resolved integer.
    Config_GetActiveGraphicOptions()->set_performance_flags(flags);

    ScriptValueBlock block{};
    block.integer = 1;
    block.real = 1.0;
    block.type_descriptor = script_real_type_descriptor;
    store_value_block(context, block);
}

void Config_GraphicOptionsGetPerformanceFlags(void* context) noexcept
{
    const std::int32_t flags = Config_GetActiveGraphicOptions()->performance_flags();

    ScriptValueBlock block{};
    block.integer = flags;
    // Retail converts the integer with fild dword, so the double is the exact integer value.
    block.real = static_cast<double>(flags);
    block.type_descriptor = script_real_type_descriptor;
    store_value_block(context, block);
}

void Config_GraphicOptionsNumPerfLevels(void* context) noexcept
{
    ScriptValueBlock block{};
    const auto levels = static_cast<std::int32_t>(performance_level_count);
    block.integer = levels;
    block.real = static_cast<double>(levels);
    block.type_descriptor = script_real_type_descriptor;
    store_value_block(context, block);
}

void Config_GraphicOptionsGetPerfFlagsFromLevel(void* context, std::int32_t level) noexcept
{
    // Retail reads dword [level*4 + 0x00537B74] without a bounds check; the port clamps to the
    // table so the read cannot leave it.
    const auto clamped = std::clamp(level, 0, static_cast<std::int32_t>(performance_level_count) - 1);
    const std::int32_t flags = static_cast<std::int32_t>(performance_flag_table[static_cast<std::size_t>(clamped)]);

    ScriptValueBlock block{};
    block.integer = flags;
    block.real = static_cast<double>(flags);
    block.type_descriptor = script_real_type_descriptor;
    store_value_block(context, block);
}

void Config_GraphicOptionsGetPerfLevelFromFlags(void* context, std::int32_t flags) noexcept
{
    const auto value = static_cast<std::uint32_t>(flags);

    // Retail: the best-distance slot starts at 0x1F; the inner loop at 0x00405557 counts the
    // set bits of table[i] ^ value and a strictly smaller distance replaces the winner.
    std::uint32_t best_distance = 0x1FU;
    std::int32_t best_level = 0;
    for (std::size_t index = 0; index < performance_flag_table.size(); ++index)
    {
        const std::uint32_t distance = population_count(performance_flag_table[index] ^ value);
        if (distance < best_distance)
        {
            best_distance = distance;
            best_level = static_cast<std::int32_t>(index);
        }
    }

    // Retail converts the winning level with fild dword and then takes both the low dword of an
    // int64 conversion and the float64 itself.
    ScriptValueBlock block{};
    block.integer = best_level;
    block.real = static_cast<double>(best_level);
    block.type_descriptor = script_real_type_descriptor;
    store_value_block(context, block);
}

} // namespace igi::retail::config
