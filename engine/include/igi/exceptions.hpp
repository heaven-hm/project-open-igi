#pragma once

#include <exception>
#include <string>
#include <string_view>

namespace igi::logging
{

class EngineException : public std::exception
{
  public:
    EngineException(std::string subsystem, std::string message);
    [[nodiscard]] const char* what() const noexcept override;
    [[nodiscard]] const std::string& subsystem() const noexcept;
    [[nodiscard]] const std::string& raw_message() const noexcept;

  private:
    std::string subsystem_;
    std::string message_;
    std::string formatted_;
};

class AssetLoadException : public EngineException
{
  public:
    explicit AssetLoadException(std::string message);
};

class PhysicsException : public EngineException
{
  public:
    explicit PhysicsException(std::string message);
};

class RenderException : public EngineException
{
  public:
    explicit RenderException(std::string message);
};

class AudioException : public EngineException
{
  public:
    explicit AudioException(std::string message);
};

class InputException : public EngineException
{
  public:
    explicit InputException(std::string message);
};

class ScriptException : public EngineException
{
  public:
    explicit ScriptException(std::string message);
};

[[noreturn]] void throw_asset_error(std::string_view message);
[[noreturn]] void throw_physics_error(std::string_view message);

} // namespace igi::logging
