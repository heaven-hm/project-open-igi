#pragma once

#include <cstdint>

namespace igi::retail::dynobj {

// Native: DynObj_LinkDynamicCube; retail VA 0x004C67E0.
// Signature: x86 cdecl uint32_t(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t).
// Evidence: Binary Ninja, Radare2 CFG, angr CFGFast and paired Frida results are recorded under
// docs/fidelity/evidence/0x004C67E0-*.json. The direct-call runtime scope does not establish that
// gameplay currently reaches this candidate function.
[[nodiscard]] std::uint32_t DynObj_LinkDynamicCube(std::uint32_t min_x,
                                                      std::uint32_t min_y,
                                                      std::uint32_t min_z,
                                                      std::uint32_t max_x,
                                                      std::uint32_t max_y,
                                                      std::uint32_t max_z) noexcept;

} // namespace igi::retail::dynobj
