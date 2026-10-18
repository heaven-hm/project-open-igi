// Native: Human_CameraEyePosition; retail VA 0x00465820; signature/calling convention x86 cdecl void(void* out, const void* human).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/human/human_camera_eye.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x00465820-binaryninja.json and 0x00465820-radare2.json; unicorn 2.1.4 execution of the retail bytes against this candidate in scripts/native_unicorn_runner.py.
// Behavior/CFG: Three basic blocks, one call (Human_IsCrouched 0x00463180). Stores the standing eye-height double 0x40A6147AE147AE14 (2826.24 units / 0.69 m) at out+0x10, then overwrites it with 0x4096666666666666 (1433.6 units / 0.35 m) when the crouch flag reads non-zero; copies the four dwords at human+0x20..0x2F (x and y as two float64) to out+0x00..0x0F; finally fld qword [human+0x30]; fadd qword [out+0x10]; fstp qword [out+0x10], so out holds (x, y, feet_z + eye_height) as three float64.
// Status/limits: STRUCTURALLY_VERIFIED with live retail-byte execution; the leftover eax (the last copied field) is not part of the contract, and only the 0x18 output bytes and the human fields are observed.
// Native: Human_IsCrouched; retail VA 0x00463180; signature/calling convention x86 cdecl std::uint8_t(const void* human).
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; implementation engine/src/retail/human/human_camera_eye.cpp.
// Tools/evidence: Binary Ninja basic blocks and Radare2 CFG in docs/fidelity/evidence/0x00463180-binaryninja.json and 0x00463180-radare2.json; unicorn 2.1.4 execution of the retail bytes against this candidate in scripts/native_unicorn_runner.py.
// Behavior/CFG: One basic block (11 bytes), no branches and no calls; reads the byte at human+0x33D into al. Retail leaves the upper 24 bits of eax holding the argument pointer, so only al is defined and the caller tests al.
// Status/limits: STRUCTURALLY_VERIFIED with live retail-byte execution; the catalog alias at this address does not describe the body (the true semantics are established by the call site at 0x00465820 and the C# transcription notes), which is recorded as a limitation.

#include "igi/retail/human/human_camera_eye.hpp"

#include <bit>
#include <cstdint>
#include <cstring>

namespace igi::retail::human {
namespace {

// Retail doubles stored by the two 'mov dword' pairs at 0x0046582A/0x00465832 and
// 0x00465845/0x0046584C.
constexpr std::uint64_t standing_eye_height_bits = 0x40A6147AE147AE14ULL; // 2826.24 units = 0.69 m
constexpr std::uint64_t crouched_eye_height_bits = 0x4096666666666666ULL; // 1433.6 units = 0.35 m

double load_double(const std::uint8_t* source, unsigned long offset)
{
    double value = 0.0;
    std::memcpy(&value, source + offset, sizeof(value));
    return value;
}

void store_double(std::uint8_t* destination, unsigned long offset, double value)
{
    std::memcpy(destination + offset, &value, sizeof(value));
}

} // namespace

std::uint8_t Human_IsCrouched(const void* human) noexcept
{
    return static_cast<const std::uint8_t*>(human)[human_camera_crouch_flag_offset];
}

void Human_CameraEyePosition(void* out, const void* human) noexcept
{
    auto* destination = static_cast<std::uint8_t*>(out);
    const auto* source = static_cast<const std::uint8_t*>(human);

    // Retail writes the standing height first and only then asks about the crouch flag; the
    // flag byte at human+0x33D is read through the same native the retail body calls.
    std::uint64_t eye_height_bits = standing_eye_height_bits;
    if (Human_IsCrouched(human) != 0U)
    {
        eye_height_bits = crouched_eye_height_bits;
    }

    // The four 'mov' pairs copy human+0x20..0x2F to out+0x00..0x0F before the x87 add.
    std::memcpy(destination, source + human_camera_position_offset, 0x10);

    const double eye_height = std::bit_cast<double>(eye_height_bits);
    const double feet_z = load_double(source, 0x30);
    // Retail: fld qword [human+0x30]; fadd qword [out+0x10]; fstp qword [out+0x10]. Both
    // operands are float64, so the double sum is identical to the x87 result.
    store_double(destination, 0x10, feet_z + eye_height);
}

} // namespace igi::retail::human
