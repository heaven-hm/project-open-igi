#pragma once

#include "igi/retail/config/retail_config.hpp"
#include "igi/retail/input/input_port.hpp"

#include <filesystem>
#include <string>

// Host adapter between the decoded config.qvm rows and the retail input port.
//
// The retail script binds actions by name (GOInRemap("Jump",
// "INPUTPORT_DEVICE_KEYBOARD", "KEY_J")); the port latches actions by Windows
// virtual-key code. This adapter is the only place the port translates the retail
// names, it covers exactly the names the shipped configuration uses, and an unknown
// name fails the read instead of silently leaving an action unbound.

// HOST HELPER: config.qvm bindings for the retail input port.
// Artifact: config.qvm from the retail install.
// Tools/evidence: the decoded GOInRemap rows in docs/fidelity/evidence/agent-c-config-20261020/config.decompiled.qsc.
// Behavior/CFG: maps decoded names to input::Action, input::Device and Win32 virtual-key codes; no native is executed.
// Status/limits: covers the names the shipped file uses; an unknown action, device or control fails the read.

namespace igi::retail::config
{

// Binds every decoded row to the port and applies the mouse sensitivity and inversion.
// Returns false and sets error when a row names an action, device or control that the
// adapter does not know.
[[nodiscard]] bool apply_input_config(const InputConfig& config, input::InputPort& port,
                                      std::string& error);

// Reads config.qvm from disk and applies it to the port.
[[nodiscard]] bool apply_input_config_file(const std::filesystem::path& path,
                                           input::InputPort& port, std::string& error);

} // namespace igi::retail::config
