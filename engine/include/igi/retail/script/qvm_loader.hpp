#pragma once

#include "igi/retail/script/qvm_compiled_program.hpp"
#include "igi/retail/resource/resource_cache.hpp"

#include <filesystem>
#include <span>
#include <stdexcept>
#include <vector>

namespace igi::retail::script {

class QvmLoadError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class QvmLoadedScript {
public:
    QvmLoadedScript(const QvmLoadedScript&) = delete;
    QvmLoadedScript& operator=(const QvmLoadedScript&) = delete;
    QvmLoadedScript(QvmLoadedScript&&) noexcept = default;
    QvmLoadedScript& operator=(QvmLoadedScript&&) noexcept = default;

    [[nodiscard]] const QvmCompiledProgram& program() const noexcept;
    [[nodiscard]] std::span<const std::byte> file_bytes() const noexcept;

private:
    friend QvmLoadedScript Qvm_Load(
        resource::ResourceCache& resource_cache,
        const std::filesystem::path& qvm_path);

    QvmLoadedScript(resource::ResourceHandle resource_handle, QvmCompiledProgram program);

    resource::ResourceHandle resource_handle_;
    QvmCompiledProgram program_;
};

// Qvm_Load - original IGI.EXE native at VA 0x004B80B0.
// Verification: retail Level 1 runtime confirms objects.qsc selects objects.qvm.
// This slice selects the compiled sibling, gets bytes through Resource_Load and validates the compiled
// QVM path through Qvm_LoadCompiledProgram. Retail VFS aliases and QSC source fallback remain
// unimplemented. REA evidence: ev_7c38680fe595188ac5d682a3e51c35966527da65dc4388db2afed21f725b1e48.
[[nodiscard]] QvmLoadedScript Qvm_Load(
    resource::ResourceCache& resource_cache,
    const std::filesystem::path& qvm_path);

} // namespace igi::retail::script
