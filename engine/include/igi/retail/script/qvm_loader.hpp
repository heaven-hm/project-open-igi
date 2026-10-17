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
    friend QvmLoadedScript Host_LoadQvmScript(
        resource::ResourceCache& resource_cache,
        const std::filesystem::path& qvm_path);

    QvmLoadedScript(resource::ResourceHandle resource_handle, QvmCompiledProgram program);

    resource::ResourceHandle resource_handle_;
    QvmCompiledProgram program_;
};

// Host adapter: loads a QVM file from the host ResourceCache and parses its header.
// The literal retail Qvm_Load (VA 0x004B80B0) is in qvm_native_loader.hpp.
// Tools: REA/Ghidra, Binary Ninja MCP, radare2; byte prefixes match the retail image.
// Evidence: ev_7c38680fe595188ac5d682a3e51c35966527da65dc4388db2afed21f725b1e48.
// CFG: r2 blocks=33, edges=50; BN blocks=33; source agreement=91.4%.
// Confidence: partial host adapter for scene/test harness.
// Signature: C++ ResourceCache/path -> QvmLoadedScript differs from retail void*(const char*).
// Usage/tests: Used by host QVM inspection tests and diagnostic scene harness.
[[nodiscard]] QvmLoadedScript Host_LoadQvmScript(
    resource::ResourceCache& resource_cache,
    const std::filesystem::path& qvm_path);

} // namespace igi::retail::script
