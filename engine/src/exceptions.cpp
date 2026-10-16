#include "igi/exceptions.hpp"

namespace igi::logging
{

EngineException::EngineException(std::string subsystem, std::string message)
    : subsystem_(std::move(subsystem)), message_(std::move(message)),
      formatted_("[" + subsystem_ + "] " + message_)
{
}

const char* EngineException::what() const noexcept
{
    return formatted_.c_str();
}

const std::string& EngineException::subsystem() const noexcept
{
    return subsystem_;
}

const std::string& EngineException::raw_message() const noexcept
{
    return message_;
}

AssetLoadException::AssetLoadException(std::string message) : EngineException("Asset", std::move(message))
{
}

PhysicsException::PhysicsException(std::string message) : EngineException("Physics", std::move(message))
{
}

RenderException::RenderException(std::string message) : EngineException("Renderer", std::move(message))
{
}

AudioException::AudioException(std::string message) : EngineException("Audio", std::move(message))
{
}

InputException::InputException(std::string message) : EngineException("Input", std::move(message))
{
}

ScriptException::ScriptException(std::string message) : EngineException("Script", std::move(message))
{
}

void throw_asset_error(std::string_view message)
{
    throw AssetLoadException(std::string(message));
}

void throw_physics_error(std::string_view message)
{
    throw PhysicsException(std::string(message));
}

} // namespace igi::logging
