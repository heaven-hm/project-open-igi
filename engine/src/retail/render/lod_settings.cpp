// Native: LODSettings_LoadParameters; retail VA 0x004EDB60; signature/calling convention x86 cdecl void*(void* context, LODSettingsRecord* (*lookup)(const char*)).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/render/lod_settings.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x004EDB60-binaryninja.json and 0x004EDB60-radare2.json; unicorn 2.1.4 execution of the retail bytes against this candidate in scripts/native_unicorn_runner.py.
// Behavior/CFG: Five blocks with one call (DynObj_Missing 0x004CEC10). The loop at 0x004EDB6D walks the five float32 slots at context+0x30..0x43; for slot 1..4 it compares the slot with the previous one (fcom), and stores the previous value over it whenever the C0 condition holds, which is current < previous and also the unordered/NaN case. It then looks the name string at context+0x20 up (host boundary for the dyn-object registry), and when the lookup returns a record writes int32 1 at record+0x24 and the five floats scaled by the float 4096.0 at 0x005333B8 (0x45800000) to record+0x28..+0x3B. Returns the lookup result (zero when nothing was found).
// Status/limits: STRUCTURALLY_VERIFIED with live retail-byte execution for the clamp and for the null-lookup path; the positive path (an existing dyn object) is transcribed from the disassembly but stays unexercised because the fixture registry is empty, and the port has no retail dyn-object registry.

#include "igi/retail/render/lod_settings.hpp"

#include <cstdint>
#include <cstring>

namespace igi::retail::render {
namespace {

// Retail float constant at 0x005333B8 (0x45800000).
constexpr float metres_to_units_scale = 4096.0F;

} // namespace

std::int32_t LODSettingsRecord::applied_flag() const noexcept
{
    std::int32_t value = 0;
    std::memcpy(&value, bytes.data() + flag_offset, sizeof(value));
    return value;
}

float* LODSettingsRecord::distances() noexcept
{
    return reinterpret_cast<float*>(bytes.data() + distances_offset);
}

const float* LODSettingsRecord::distances() const noexcept
{
    return reinterpret_cast<const float*>(bytes.data() + distances_offset);
}

LODSettingsRecord* LODSettings_LoadParameters(void* context, LODSettingsLookup lookup) noexcept
{
    auto* context_bytes = static_cast<std::uint8_t*>(context);
    auto* distances = reinterpret_cast<float*>(context_bytes + 0x30);

    // Retail keeps the loop counter in ecx, the walk pointer in edx, and skips the compare for
    // the first slot (test ecx,ecx / je). fcomp + fnstsw ax + test ah,1 stores the previous
    // value when the C0 flag is up: the slot is smaller, or the comparison was unordered.
    for (int index = 1; index < 5; ++index)
    {
        const float current = distances[index];
        const float previous = distances[index - 1];
        if (!(current >= previous))
        {
            distances[index] = previous;
        }
    }

    // Retail: add edi, 0x20; push edi; call 0x004CEC10 (DynObj_Missing). The port supplies the
    // registry lookup through the host boundary because it has no dyn-object registry.
    LODSettingsRecord* record =
        lookup(reinterpret_cast<const char*>(context_bytes + 0x20));
    if (record != nullptr)
    {
        std::int32_t applied = 1;
        std::memcpy(record->bytes.data() + LODSettingsRecord::flag_offset, &applied,
                    sizeof(applied));
        float* target = record->distances();
        for (std::size_t index = 0; index < LODSettingsRecord::distance_count; ++index)
        {
            // Retail: fld dword [eax]; fmul dword [0x5333B8]; fstp dword [ecx-4]. Both operands
            // are float32, so the float product is identical to the x87 result.
            target[index] = distances[index] * metres_to_units_scale;
        }
    }
    return record;
}

} // namespace igi::retail::render
