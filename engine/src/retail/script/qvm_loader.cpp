#include "igi/retail/script/qvm_loader.hpp"

#include <utility>

namespace igi::retail::script {

QvmLoadedScript::QvmLoadedScript(resource::ResourceHandle resource_handle, QvmCompiledProgram program)
    : resource_handle_(std::move(resource_handle)),
      program_(std::move(program))
{
}

const QvmCompiledProgram& QvmLoadedScript::program() const noexcept
{
    return program_;
}

std::span<const std::byte> QvmLoadedScript::file_bytes() const noexcept
{
    return resource_handle_->bytes;
}

QvmLoadedScript Qvm_Load(
    resource::ResourceCache& resource_cache,
    const std::filesystem::path& qvm_path)
{
    const resource::ResourceHandle resource_handle = resource::Resource_Load(resource_cache, qvm_path);
    try {
        QvmCompiledProgram compiled_program = Qvm_LoadCompiledProgram(resource_handle->bytes);
        return QvmLoadedScript(resource_handle, std::move(compiled_program));
    } catch (const QvmFormatError& format_error) {
        throw QvmLoadError(
            "Failed to load compiled QVM resource '" + qvm_path.string() + "': " + format_error.what());
    }
}

} // namespace igi::retail::script
