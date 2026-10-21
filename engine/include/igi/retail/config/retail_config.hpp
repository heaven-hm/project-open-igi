#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

// Host-side reader for the retail configuration scripts.
//
// The retail game stores its settings as compiled LOOP scripts: config.qvm binds the
// input actions, humanplayer/humanplayer.qvm defines the human player parameters and
// LOD.QVM declares the model LOD task table. The port needs those values before the
// script VM can run a frame, so this module reads the constant-argument call sequence
// straight out of the compiled program.
//
// This is a host configuration reader, not a transcription of a retail native and not
// a QVM interpreter. It understands the instruction subset the three configuration
// files use to build their arguments:
//
//   PUSHIIB u8, PUSHB u8, PUSHW u16, PUSH u32, PUSHF f32, PUSHSIB u8, PUSHSIW u16,
//   PUSH0, PUSH1, MINUS, POP, BRK, BRA u32, CALL <u32 argc> <u32 offset>...
//
// The opcode bytes were recovered from the shipped files by correlating the converter
// disassembly with the raw code table; the evidence and the decoded output live in
// docs/fidelity/evidence/agent-c-config-20261020/ and
// docs/fidelity/human_player_movement.md. A call that uses an instruction outside the
// subset, a non-constant argument or an unknown symbol index fails the read instead of
// guessing a value.

// HOST HELPER: retail configuration reader declarations.
// Artifact: config.qvm, humanplayer/humanplayer.qvm and LOD.QVM from the retail install.
// Tools/evidence: igi1conv 1.11.0 decompile plus the raw LOOP code-table opcodes; see docs/fidelity/human_player_movement.md.
// Behavior/CFG: declares the constant-argument call model; it executes no retail native and claims no address.
// Status/limits: exact for the shipped files; unsupported opcodes and non-constant arguments fail the read.

namespace igi::retail::config
{

enum class ScriptArgumentKind : std::uint8_t
{
    integer = 0,
    real = 1,
    string_index = 2,
};

struct ScriptArgument
{
    ScriptArgumentKind kind = ScriptArgumentKind::integer;
    // Integer value for integer arguments, string-constant index for string_index.
    std::int64_t integer = 0;
    // Value for real arguments, and the integer value widened for convenience.
    double real = 0.0;
};

struct ScriptCall
{
    std::string callee;
    std::vector<ScriptArgument> arguments;
};

struct ScriptConstants
{
    std::vector<std::string> symbols;
    std::vector<std::string> strings;
};

struct ScriptProgram
{
    std::vector<ScriptCall> calls;
    ScriptConstants constants;

    // The text of a string-index argument, or an empty view when the index is out of range.
    [[nodiscard]] std::string_view string_argument(const ScriptArgument& argument) const noexcept;
    // The text of an identifier argument (a symbol-table index), or an empty view.
    [[nodiscard]] std::string_view symbol_argument(const ScriptArgument& argument) const noexcept;
};

// Reads the constant-argument call sequence from a compiled LOOP program.
// Returns false and sets error on a malformed program, an unsupported instruction or a
// call whose arguments are not constants.
[[nodiscard]] bool read_script_program(std::span<const std::byte> qvm_bytes, ScriptProgram& out,
                                       std::string& error);

// Reads and parses one configuration file from disk.
[[nodiscard]] bool read_script_file(const std::filesystem::path& path, ScriptProgram& out,
                                    std::string& error);

struct InputBinding
{
    std::string action;
    std::string device;
    std::string control;
};

// config.qvm. The values are the ones the shipped file sets; a value the file does not
// set stays empty instead of receiving a port default.
struct InputConfig
{
    std::vector<InputBinding> bindings;
    std::optional<float> mouse_sensitivity;
    std::optional<std::int32_t> mouse_inverted;
    std::optional<std::string> player_name;
    std::optional<std::int32_t> active_mission;
    std::optional<std::string> content_control_password;
    std::optional<std::int32_t> start_state;
};

[[nodiscard]] bool decode_input_config(const ScriptProgram& program, InputConfig& out,
                                       std::string& error);

// humanplayer/humanplayer.qvm DefineHumanPlayerGeneral.
//
// The retail call is positional. Only the leading arguments have an established meaning
// from the movement natives:
//   [0] root-motion scale applied by Human_RunUpdate (0x004109A0)
//   [1] jump horizontal speed, km/h
//   [2] jump vertical speed, km/h
//   [3] air-control speed, km/h
// The remaining arguments are kept in order and are not named because the evidence does
// not establish their meaning yet.
struct HumanPlayerGeneral
{
    std::vector<double> arguments;

    [[nodiscard]] std::optional<double> root_motion_scale() const noexcept;
    [[nodiscard]] std::optional<double> jump_horizontal_kmh() const noexcept;
    [[nodiscard]] std::optional<double> jump_vertical_kmh() const noexcept;
    [[nodiscard]] std::optional<double> air_control_kmh() const noexcept;
};

[[nodiscard]] bool decode_human_player_general(const ScriptProgram& program,
                                               HumanPlayerGeneral& out, std::string& error);

// LOD.QVM Task_New(-1, "ModelLODSettings", <task name>, <model>, d1, d2, d3, d4, cutoff).
struct ModelLodSettings
{
    std::string task_name;
    std::string model;
    float distance_to_lod1 = 0.0F;
    float distance_to_lod2 = 0.0F;
    float distance_to_lod3 = 0.0F;
    float distance_to_lod4 = 0.0F;
    float distance_to_cutoff = 0.0F;
};

struct ModelLodTable
{
    std::vector<ModelLodSettings> rows;
};

[[nodiscard]] bool decode_model_lod_table(const ScriptProgram& program, ModelLodTable& out,
                                          std::string& error);

} // namespace igi::retail::config
