#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace igi::retail::model
{

class MtpFormatError final : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;
};

struct MtpMaterialReference
{
    std::uint32_t first_native_index = 0U;
    std::uint32_t second_native_index = 0U;
};

struct MtpInstance
{
    std::uint32_t model_index = 0U;
    std::string instance_name;
    std::vector<std::uint32_t> material_reference_indices;
};

struct MtpPackage
{
    std::uint32_t root_tag = 0U;
    std::vector<std::string> model_names;
    // The 'BANM' chunk names the animation files the package's characters use.
    // The retail loads each as "<prefix>anims/<name>.iff" (DynObj_LoadPackageAnimations,
    // 0x004CF910), and each file holds that character's skeleton.
    std::vector<std::string> animation_names;
    std::vector<std::string> texture_names;
    std::vector<std::string> instance_names;
    std::vector<MtpInstance> instances;
    std::vector<MtpMaterialReference> material_references;

    // Host adapter: parses MTP IFF container chunks into a C++ MtpPackage struct for diagnostic inspection.
    // The literal retail DynObj_InsertHashItem_14 (VA 0x004CF080) is in mtp_package_loader.hpp.
    // Tools: Ghidra/REA and Binary Ninja; runtime parity is not verified.
    // Evidence: REA ev_b115c5dcf8484fa999611be81df70c28a1e3299552c8cf4c96817bd6883cc409;
    // RE-Agent catalog assets/RE_AGENT_VERIFIED_NATIVES.md (0x004CF080, CFG 1.000, 5/6).
    // CFG: radare2 41 blocks/65 edges; Binary Ninja 41 blocks; 100% structural agreement.
    // Confidence: partial host parser adapter; literal native loader is in mtp_package_loader.cpp.
    // Signature: std::span<const std::byte> -> MtpPackage differs from retail void*(void*, const char*, int32_t).
    // Usage/tests: Used by host MTP inspection tests and diagnostic scene harness.
    [[nodiscard]] static MtpPackage Host_ParseModelTexturePackage(std::span<const std::byte> file_bytes);
};

} // namespace igi::retail::model
