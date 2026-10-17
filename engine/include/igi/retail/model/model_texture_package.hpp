#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace igi::retail::model {

class MtpFormatError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct MtpMaterialReference {
    std::uint32_t first_native_index = 0U;
    std::uint32_t second_native_index = 0U;
};

struct MtpInstance {
    std::uint32_t model_index = 0U;
    std::string instance_name;
    std::vector<std::uint32_t> material_reference_indices;
};

struct MtpPackage {
    std::uint32_t root_tag = 0U;
    std::vector<std::string> model_names;
    std::vector<std::string> instance_names;
    std::vector<MtpInstance> instances;
    std::vector<MtpMaterialReference> material_references;

    // DynObj_InsertHashItem_14 - 0x004CF080 - Native static evidence: CFG 1.000 (PASS), RE-Agent 5/6; port runtime parity unverified.
    // Tools: Ghidra/REA and Binary Ninja; runtime parity is not verified.
    // Evidence: REA ev_b115c5dcf8484fa999611be81df70c28a1e3299552c8cf4c96817bd6883cc409;
    // RE-Agent catalog assets/RE_AGENT_VERIFIED_NATIVES.md (0x004CF080, CFG 1.000, 5/6).
    // The catalog name is provisional: REA labels this procedure FUN_004cf080.
    // Chunk discovery follows the 8-byte sibling records in helper 0x004CEAA0.
    [[nodiscard]] static MtpPackage DynObj_InsertHashItem_14(std::span<const std::byte> file_bytes);
};

} // namespace igi::retail::model
