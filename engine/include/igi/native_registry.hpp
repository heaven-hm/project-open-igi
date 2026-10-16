#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace igi::natives {

enum class CfgGate : uint8_t {
    Pass,
    Fail,
    Candidate,
    Unknown
};

struct LayerVerification {
    bool l0_static{false};
    bool l1_cfg{false};
    bool l2_signature{false};
    bool l3_link_str{false};
    bool l4_memory{false};
    bool l5_runtime{false};
};

struct NativeEntry {
    uint32_t address{};
    std::string hash;
    std::string name;
    std::string proto;
    std::string cc;
    std::string subsystem;
    CfgGate gate{CfgGate::Unknown};
    float score{0.0f};
    std::string confidence;
    LayerVerification layers;
};

struct SubsystemStats {
    std::string name;
    std::size_t total{};
    std::size_t pass{};
    std::size_t fail{};
};

struct RegistryStats {
    std::size_t total_natives{};
    std::size_t pass_count{};
    std::size_t fail_count{};
    std::size_t candidate_count{};
    std::size_t l5_runtime_count{};
    std::vector<SubsystemStats> subsystems;
};

class NativeRegistry final {
public:
    NativeRegistry();
    explicit NativeRegistry(const std::filesystem::path& catalog_path);

    [[nodiscard]] bool load_from_json(const std::filesystem::path& catalog_path);
    [[nodiscard]] bool is_loaded() const noexcept { return !entries_.empty(); }

    [[nodiscard]] std::size_t size() const noexcept { return entries_.size(); }
    [[nodiscard]] const std::vector<NativeEntry>& all() const noexcept { return entries_; }

    [[nodiscard]] const NativeEntry* find_by_address(uint32_t address) const;
    [[nodiscard]] const NativeEntry* find_by_name(std::string_view name) const;
    [[nodiscard]] std::vector<const NativeEntry*> find_by_subsystem(std::string_view subsystem) const;

    [[nodiscard]] RegistryStats stats() const;
    [[nodiscard]] static std::string_view deduce_subsystem(std::string_view name);

    [[nodiscard]] static NativeRegistry& instance();

private:
    std::vector<NativeEntry> entries_;
    void sort_and_index();
};

} // namespace igi::natives
