// Native: Human_TaskViewReset; retail VA 0x004659E0; signature/calling convention x86 cdecl void*(void*).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/human/human_view.cpp.
// Tools/evidence: Binary Ninja + Radare2 CFG (docs/fidelity/evidence/0x004659E0-*.json); unicorn 2.1.4 execution of the retail bytes vs this candidate (docs/fidelity/evidence/0x004659E0-behavioral.json).
// Behavior/CFG: one basic block, no branches, no calls; fld qword [0x005335E8] + fptan -> f32 [human+0x1E4]; fld qword [0x005339C0] + fptan -> f32 [human+0x1E8]; dword [human+0x1EC] = 0; eax returns the human pointer.
// Status/limits: STRUCTURALLY_VERIFIED with live retail-byte execution; x87 fptan is not supported by Radare2 ESIL or angr VEX, so the retail oracle runs under unicorn with the true PE image.

#include "igi/retail/human/human_view.hpp"

#include <cmath>
#include <cstdint>
#include <cstring>

namespace igi::retail::human {
namespace {

// Retail doubles read from the image: 0x005335E8 = 0.7853981852531433, 0x005339C0 = 0.5235987901687622.
// Both are float-precision values promoted to double (the constants the ENHANCER::FOV_SET path also uses).
constexpr double view_tangent_angle = 0.7853981852531433;
constexpr double view_model_tangent_angle = 0.5235987901687622;

} // namespace

void* Human_TaskViewReset(void* human)
{
    auto* bytes = static_cast<std::uint8_t*>(human);
    auto store_float = [bytes](unsigned long offset, double value)
    {
        const float narrowed = static_cast<float>(std::tan(value));
        std::memcpy(bytes + offset, &narrowed, sizeof(narrowed));
    };
    // Retail store order: +0x1EC is zeroed first, then the two tangent stores.
    const std::int32_t cleared = 0;
    std::memcpy(bytes + human_view_aux_offset, &cleared, sizeof(cleared));
    store_float(human_view_tangent_x_offset, view_tangent_angle);
    store_float(human_view_tangent_viewmodel_offset, view_model_tangent_angle);
    return human;
}

} // namespace igi::retail::human
