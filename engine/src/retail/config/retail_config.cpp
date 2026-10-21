#include "igi/retail/config/retail_config.hpp"
#include "igi/retail/config/input_remap.hpp"

#include "igi/retail/script/qvm_compiled_program.hpp"

#include <bit>
#include <cstring>
#include <fstream>
#include <iterator>
#include <sstream>
#include <utility>

// The opcode bytes below are the ones the shipped configuration files use. They were
// recovered from config.qvm, humanplayer.qvm and LOD.QVM by reading the raw code table
// next to the converter disassembly; the table and the decoded output are recorded in
// docs/fidelity/evidence/agent-c-config-20261020/.
// HOST HELPER: retail configuration reader.
// Artifact: config.qvm, humanplayer/humanplayer.qvm and LOD.QVM from the retail install.
// Tools/evidence: igi1conv 1.11.0 decompile plus the raw LOOP code-table opcodes; see docs/fidelity/human_player_movement.md.
// Behavior/CFG: reads constant-argument call blocks; it executes no retail native and claims no address.
// Status/limits: exact for the shipped files; unsupported opcodes and non-constant arguments fail the read.

namespace igi::retail::config
{
namespace
{

constexpr std::uint8_t opcode_brk = 0x00U;
constexpr std::uint8_t opcode_push = 0x02U;    // PUSH u32
constexpr std::uint8_t opcode_pushb = 0x03U;   // PUSHB u8
constexpr std::uint8_t opcode_pushw = 0x04U;   // PUSHW u16
constexpr std::uint8_t opcode_pushf = 0x05U;   // PUSHF f32
constexpr std::uint8_t opcode_pushsib = 0x09U; // PUSHSIB u8
constexpr std::uint8_t opcode_pushsiw = 0x0AU; // PUSHSIW u16
constexpr std::uint8_t opcode_pushiib = 0x0DU; // PUSHIIB u8
constexpr std::uint8_t opcode_push0 = 0x0FU;
constexpr std::uint8_t opcode_push1 = 0x10U;
constexpr std::uint8_t opcode_pop = 0x12U;
constexpr std::uint8_t opcode_bra = 0x14U; // BRA u32
constexpr std::uint8_t opcode_call = 0x18U; // CALL u32 count, count * u32 offset
constexpr std::uint8_t opcode_minus = 0x2CU;

constexpr std::size_t bra_size = 5U;

std::string hex_offset(std::size_t value)
{
    std::ostringstream stream;
    stream << "0x" << std::hex << value;
    return stream.str();
}

class CodeReader
{
  public:
    explicit CodeReader(std::span<const std::byte> code) : code_(code) {}

    [[nodiscard]] std::size_t position() const noexcept { return position_; }
    [[nodiscard]] bool at_end() const noexcept { return position_ >= code_.size(); }
    [[nodiscard]] std::size_t size() const noexcept { return code_.size(); }

    void seek(std::size_t position) noexcept { position_ = position; }
    void skip(std::size_t count) noexcept { position_ += count; }

    [[nodiscard]] bool read_u8(std::uint8_t& out) noexcept
    {
        if (code_.size() - position_ < 1U) {
            return false;
        }
        out = std::to_integer<std::uint8_t>(code_[position_]);
        ++position_;
        return true;
    }

    [[nodiscard]] bool read_u16(std::uint16_t& out) noexcept
    {
        if (code_.size() - position_ < 2U) {
            return false;
        }
        out = static_cast<std::uint16_t>(std::to_integer<std::uint16_t>(code_[position_]))
            | static_cast<std::uint16_t>(static_cast<std::uint16_t>(
                   std::to_integer<std::uint8_t>(code_[position_ + 1U]))
                << 8U);
        position_ += 2U;
        return true;
    }

    [[nodiscard]] bool read_u32(std::uint32_t& out) noexcept
    {
        if (code_.size() - position_ < 4U) {
            return false;
        }
        out = 0U;
        for (std::size_t index = 0U; index < 4U; ++index) {
            out |= static_cast<std::uint32_t>(
                       std::to_integer<std::uint8_t>(code_[position_ + index]))
                << static_cast<unsigned int>(index * 8U);
        }
        position_ += 4U;
        return true;
    }

    [[nodiscard]] bool read_f32(float& out) noexcept
    {
        std::uint32_t bits = 0U;
        if (!read_u32(bits)) {
            return false;
        }
        out = std::bit_cast<float>(bits);
        return true;
    }

  private:
    std::span<const std::byte> code_;
    std::size_t position_ = 0U;
};

ScriptArgument integer_argument(std::int64_t value)
{
    ScriptArgument argument;
    argument.kind = ScriptArgumentKind::integer;
    argument.integer = value;
    argument.real = static_cast<double>(value);
    return argument;
}

ScriptArgument real_argument(double value)
{
    ScriptArgument argument;
    argument.kind = ScriptArgumentKind::real;
    argument.integer = 0;
    argument.real = value;
    return argument;
}

ScriptArgument string_argument(std::uint32_t index)
{
    ScriptArgument argument;
    argument.kind = ScriptArgumentKind::string_index;
    argument.integer = static_cast<std::int64_t>(index);
    argument.real = 0.0;
    return argument;
}

bool push_instruction_argument(CodeReader& reader, std::uint8_t opcode,
                               std::vector<ScriptArgument>& stack, std::string& error)
{
    switch (opcode) {
    case opcode_push: {
        std::uint32_t value = 0U;
        if (!reader.read_u32(value)) {
            error = "truncated PUSH";
            return false;
        }
        stack.push_back(integer_argument(static_cast<std::int64_t>(value)));
        return true;
    }
    case opcode_pushb: {
        std::uint8_t value = 0U;
        if (!reader.read_u8(value)) {
            error = "truncated PUSHB";
            return false;
        }
        stack.push_back(integer_argument(static_cast<std::int64_t>(value)));
        return true;
    }
    case opcode_pushw: {
        std::uint16_t value = 0U;
        if (!reader.read_u16(value)) {
            error = "truncated PUSHW";
            return false;
        }
        stack.push_back(integer_argument(static_cast<std::int64_t>(value)));
        return true;
    }
    case opcode_pushf: {
        float value = 0.0F;
        if (!reader.read_f32(value)) {
            error = "truncated PUSHF";
            return false;
        }
        stack.push_back(real_argument(static_cast<double>(value)));
        return true;
    }
    case opcode_pushsib: {
        std::uint8_t value = 0U;
        if (!reader.read_u8(value)) {
            error = "truncated PUSHSIB";
            return false;
        }
        stack.push_back(string_argument(value));
        return true;
    }
    case opcode_pushsiw: {
        std::uint16_t value = 0U;
        if (!reader.read_u16(value)) {
            error = "truncated PUSHSIW";
            return false;
        }
        stack.push_back(string_argument(value));
        return true;
    }
    case opcode_pushiib: {
        std::uint8_t value = 0U;
        if (!reader.read_u8(value)) {
            error = "truncated PUSHIIB";
            return false;
        }
        stack.push_back(integer_argument(static_cast<std::int64_t>(value)));
        return true;
    }
    case opcode_push0:
        stack.push_back(integer_argument(0));
        return true;
    case opcode_push1:
        stack.push_back(integer_argument(1));
        return true;
    default:
        error = "not a push opcode";
        return false;
    }
}

// Evaluates one constant argument expression. The arguments of the configuration calls
// are short constant blocks that end at BRK; anything else fails the read.
bool evaluate_argument(std::span<const std::byte> code, std::size_t offset, ScriptArgument& out,
                       std::string& error)
{
    if (offset >= code.size()) {
        error = "argument offset " + hex_offset(offset) + " is outside the code table";
        return false;
    }

    CodeReader reader(code);
    reader.seek(offset);
    std::vector<ScriptArgument> stack;
    while (!reader.at_end()) {
        std::uint8_t opcode = 0U;
        if (!reader.read_u8(opcode)) {
            error = "truncated argument expression";
            return false;
        }
        if (opcode == opcode_brk) {
            if (stack.size() != 1U) {
                error = "argument expression at " + hex_offset(offset)
                    + " does not produce exactly one value";
                return false;
            }
            out = stack.back();
            return true;
        }
        switch (opcode) {
        case opcode_push:
        case opcode_pushb:
        case opcode_pushw:
        case opcode_pushf:
        case opcode_pushsib:
        case opcode_pushsiw:
        case opcode_pushiib:
        case opcode_push0:
        case opcode_push1: {
            if (!push_instruction_argument(reader, opcode, stack, error)) {
                error = "argument expression at " + hex_offset(offset) + ": " + error;
                return false;
            }
            break;
        }
        case opcode_minus: {
            if (stack.empty()) {
                error = "MINUS without an operand at " + hex_offset(offset);
                return false;
            }
            ScriptArgument& value = stack.back();
            value.real = -value.real;
            value.integer = -value.integer;
            break;
        }
        case opcode_pop: {
            if (stack.empty()) {
                error = "POP without a value at " + hex_offset(offset);
                return false;
            }
            stack.pop_back();
            break;
        }
        default:
            error = "argument expression at " + hex_offset(offset) + " uses opcode "
                + hex_offset(opcode) + ", which is outside the constant-argument subset";
            return false;
        }
    }
    error = "argument expression at " + hex_offset(offset) + " has no BRK";
    return false;
}

std::vector<std::string> copy_indexed_table(
    const std::optional<script::QvmIndexedStringTable>& table)
{
    std::vector<std::string> result;
    if (!table.has_value()) {
        return result;
    }
    const std::uint32_t entry_count = table->index_limit_bytes / 4U;
    result.reserve(static_cast<std::size_t>(entry_count));
    for (std::uint32_t index = 0U; index < entry_count; ++index) {
        const std::optional<std::string_view> text = table->try_get(index);
        result.emplace_back(text.value_or(std::string_view{}));
    }
    return result;
}

double double_of(const ScriptArgument& argument, const char* callee, std::string& error)
{
    if (argument.kind == ScriptArgumentKind::real) {
        return argument.real;
    }
    if (argument.kind == ScriptArgumentKind::integer) {
        return static_cast<double>(argument.integer);
    }
    error = std::string(callee) + " expects a numeric argument";
    return 0.0;
}

} // namespace

std::string_view ScriptProgram::string_argument(const ScriptArgument& argument) const noexcept
{
    if (argument.kind != ScriptArgumentKind::string_index || argument.integer < 0) {
        return {};
    }
    const auto index = static_cast<std::size_t>(argument.integer);
    if (index >= constants.strings.size()) {
        return {};
    }
    return constants.strings[index];
}

std::string_view ScriptProgram::symbol_argument(const ScriptArgument& argument) const noexcept
{
    if (argument.kind != ScriptArgumentKind::integer || argument.integer < 0) {
        return {};
    }
    const auto index = static_cast<std::size_t>(argument.integer);
    if (index >= constants.symbols.size()) {
        return {};
    }
    return constants.symbols[index];
}

bool read_script_program(std::span<const std::byte> qvm_bytes, ScriptProgram& out,
                         std::string& error)
{
    out = ScriptProgram{};
    script::QvmCompiledProgram program;
    try {
        program = script::Qvm_LoadCompiledProgram(qvm_bytes);
    } catch (const script::QvmFormatError& failure) {
        error = std::string("not a LOOP program: ") + failure.what();
        return false;
    }

    out.constants.symbols = copy_indexed_table(program.symbol_names);
    out.constants.strings = copy_indexed_table(program.string_constants);

    CodeReader reader(program.code);
    std::vector<ScriptArgument> stack;
    while (!reader.at_end()) {
        const std::size_t instruction_offset = reader.position();
        std::uint8_t opcode = 0U;
        if (!reader.read_u8(opcode)) {
            error = "truncated instruction at " + hex_offset(instruction_offset);
            return false;
        }
        switch (opcode) {
        case opcode_push:
        case opcode_pushb:
        case opcode_pushw:
        case opcode_pushf:
        case opcode_pushsib:
        case opcode_pushsiw:
        case opcode_pushiib:
        case opcode_push0:
        case opcode_push1:
            if (!push_instruction_argument(reader, opcode, stack, error)) {
                error = "instruction at " + hex_offset(instruction_offset) + ": " + error;
                return false;
            }
            break;
        case opcode_pop:
            if (!stack.empty()) {
                stack.pop_back();
            }
            break;
        case opcode_minus:
            if (stack.empty()) {
                error = "MINUS without an operand at " + hex_offset(instruction_offset);
                return false;
            }
            stack.back().real = -stack.back().real;
            stack.back().integer = -stack.back().integer;
            break;
        case opcode_brk:
            break;
        case opcode_bra:
            reader.skip(bra_size - 1U);
            break;
        case opcode_call: {
            std::uint32_t argument_count = 0U;
            if (!reader.read_u32(argument_count)) {
                error = "truncated CALL at " + hex_offset(instruction_offset);
                return false;
            }
            std::vector<std::uint32_t> argument_offsets;
            argument_offsets.reserve(static_cast<std::size_t>(argument_count));
            for (std::uint32_t index = 0U; index < argument_count; ++index) {
                std::uint32_t argument_offset = 0U;
                if (!reader.read_u32(argument_offset)) {
                    error = "truncated CALL argument list at " + hex_offset(instruction_offset);
                    return false;
                }
                argument_offsets.push_back(argument_offset);
            }
            if (stack.empty()) {
                error = "CALL at " + hex_offset(instruction_offset)
                    + " has no callee on the value stack";
                return false;
            }
            if (stack.back().kind != ScriptArgumentKind::integer) {
                error = "CALL at " + hex_offset(instruction_offset)
                    + " has a non-identifier callee";
                return false;
            }
            const auto callee_index = static_cast<std::size_t>(stack.back().integer);
            if (stack.back().integer < 0 || callee_index >= out.constants.symbols.size()) {
                error = "CALL at " + hex_offset(instruction_offset) + " references symbol "
                    + std::to_string(stack.back().integer) + ", which is outside the symbol table";
                return false;
            }
            ScriptCall call;
            call.callee = out.constants.symbols[callee_index];
            call.arguments.reserve(argument_offsets.size());
            for (const std::uint32_t argument_offset : argument_offsets) {
                ScriptArgument argument;
                if (!evaluate_argument(program.code, argument_offset, argument, error)) {
                    error = "CALL " + call.callee + ": " + error;
                    return false;
                }
                call.arguments.push_back(std::move(argument));
            }
            out.calls.push_back(std::move(call));
            // The arguments were evaluated from their own expression blocks, so the
            // linear scan drops whatever those blocks left behind before the next call.
            stack.clear();
            break;
        }
        default:
            error = "unsupported opcode " + hex_offset(opcode) + " at "
                + hex_offset(instruction_offset);
            return false;
        }
    }
    return true;
}

bool read_script_file(const std::filesystem::path& path, ScriptProgram& out, std::string& error)
{
    std::ifstream stream(path, std::ios::binary);
    if (!stream) {
        error = "cannot open " + path.string();
        return false;
    }
    const std::vector<char> bytes{std::istreambuf_iterator<char>(stream),
                                  std::istreambuf_iterator<char>()};
    if (bytes.empty()) {
        error = "empty file " + path.string();
        return false;
    }
    std::vector<std::byte> buffer(bytes.size());
    std::memcpy(buffer.data(), bytes.data(), bytes.size());
    return read_script_program(buffer, out, error);
}

bool decode_input_config(const ScriptProgram& program, InputConfig& out, std::string& error)
{
    out = InputConfig{};
    // The retail's action count starts at one: the native's slot arithmetic
    // uses count - 1 for the first action.
    InputRemap_ActionCount = 1U;
    for (const ScriptCall& call : program.calls) {
        if (call.callee == "GOInRemap") {
            if (call.arguments.size() != 3U) {
                error = "GOInRemap expects three arguments";
                return false;
            }
            // The retail executes the script, and the GOInRemap native at
            // 0x004037C0 stores the three argument values in the remap table.
            // The port fills the same table, then reads the bindings back out
            // of it, so the table stays the single source of truth.
            std::uint32_t values[3]{};
            for (std::size_t index = 0; index < 3U; ++index) {
                const ScriptArgument& argument = call.arguments[index];
                if (argument.kind != ScriptArgumentKind::integer || argument.integer < 0 ||
                    static_cast<std::size_t>(argument.integer) >= program.constants.symbols.size()) {
                    error = "GOInRemap references an identifier outside the symbol table";
                    return false;
                }
                values[index] = static_cast<std::uint32_t>(argument.integer);
            }
            const std::uint32_t action = InputRemap_ActionCount;
            if (action * igi::retail::config::action_table_stride >=
                igi::retail::config::action_table_size) {
                error = "GOInRemap exceeds the retail action table";
                return false;
            }
            InputRemap_ActionTable[action * igi::retail::config::action_table_stride] = action;
            igi::retail::config::QvmFrameView frame{};
            frame.argument_values = values;
            GOInRemap(&frame);
            ++InputRemap_ActionCount;
            const std::uint32_t slot = (action - 1U) * 115U + action;
            const std::uint32_t* const entry =
                InputRemap_Table + slot * igi::retail::config::remap_table_stride;
            InputBinding binding;
            binding.action = program.constants.symbols[entry[0]];
            binding.device = program.constants.symbols[entry[1]];
            binding.control = program.constants.symbols[entry[2]];
            if (binding.action.empty() || binding.device.empty() || binding.control.empty()) {
                error = "GOInRemap references an identifier outside the symbol table";
                return false;
            }
            out.bindings.push_back(std::move(binding));
        } else if (call.callee == "GOInMouSens") {
            if (call.arguments.size() != 1U) {
                error = "GOInMouSens expects one argument";
                return false;
            }
            out.mouse_sensitivity = static_cast<float>(
                double_of(call.arguments[0], "GOInMouSens", error));
            if (!error.empty()) {
                return false;
            }
        } else if (call.callee == "GOInMouInv") {
            if (call.arguments.size() != 1U
                || call.arguments[0].kind != ScriptArgumentKind::integer) {
                error = "GOInMouInv expects one integer argument";
                return false;
            }
            out.mouse_inverted = static_cast<std::int32_t>(call.arguments[0].integer);
        } else if (call.callee == "GOPlayer") {
            if (call.arguments.size() != 1U) {
                error = "GOPlayer expects one argument";
                return false;
            }
            out.player_name = std::string(program.string_argument(call.arguments[0]));
        } else if (call.callee == "GOActiveMission") {
            if (call.arguments.size() != 1U
                || call.arguments[0].kind != ScriptArgumentKind::integer) {
                error = "GOActiveMission expects one integer argument";
                return false;
            }
            out.active_mission = static_cast<std::int32_t>(call.arguments[0].integer);
        } else if (call.callee == "GOContentControlPW") {
            if (call.arguments.size() != 1U) {
                error = "GOContentControlPW expects one argument";
                return false;
            }
            out.content_control_password = std::string(program.string_argument(call.arguments[0]));
        } else if (call.callee == "GOStart") {
            if (call.arguments.size() != 1U
                || call.arguments[0].kind != ScriptArgumentKind::integer) {
                error = "GOStart expects one integer argument";
                return false;
            }
            out.start_state = static_cast<std::int32_t>(call.arguments[0].integer);
        }
    }
    return true;
}

std::optional<double> HumanPlayerGeneral::root_motion_scale() const noexcept
{
    if (arguments.size() <= 0U) {
        return std::nullopt;
    }
    return arguments[0];
}

std::optional<double> HumanPlayerGeneral::jump_horizontal_kmh() const noexcept
{
    if (arguments.size() <= 1U) {
        return std::nullopt;
    }
    return arguments[1];
}

std::optional<double> HumanPlayerGeneral::jump_vertical_kmh() const noexcept
{
    if (arguments.size() <= 2U) {
        return std::nullopt;
    }
    return arguments[2];
}

std::optional<double> HumanPlayerGeneral::air_control_kmh() const noexcept
{
    if (arguments.size() <= 3U) {
        return std::nullopt;
    }
    return arguments[3];
}

bool decode_human_player_general(const ScriptProgram& program, HumanPlayerGeneral& out,
                                 std::string& error)
{
    out = HumanPlayerGeneral{};
    for (const ScriptCall& call : program.calls) {
        if (call.callee != "DefineHumanPlayerGeneral") {
            continue;
        }
        if (call.arguments.empty()) {
            error = "DefineHumanPlayerGeneral has no arguments";
            return false;
        }
        out.arguments.reserve(call.arguments.size());
        for (const ScriptArgument& argument : call.arguments) {
            std::string argument_error;
            const double value = double_of(argument, "DefineHumanPlayerGeneral", argument_error);
            if (!argument_error.empty()) {
                error = argument_error;
                return false;
            }
            out.arguments.push_back(value);
        }
        return true;
    }
    error = "no DefineHumanPlayerGeneral call";
    return false;
}

bool decode_model_lod_table(const ScriptProgram& program, ModelLodTable& out, std::string& error)
{
    out = ModelLodTable{};
    for (const ScriptCall& call : program.calls) {
        if (call.callee != "Task_New" || call.arguments.size() < 9U) {
            continue;
        }
        if (program.string_argument(call.arguments[1]) != "ModelLODSettings") {
            continue;
        }
        ModelLodSettings row;
        row.task_name = std::string(program.string_argument(call.arguments[2]));
        row.model = std::string(program.string_argument(call.arguments[3]));
        if (row.task_name.empty() || row.model.empty()) {
            error = "ModelLODSettings row has an empty task name or model";
            return false;
        }
        float distances[5] = {};
        for (std::size_t index = 0U; index < 5U; ++index) {
            std::string distance_error;
            distances[index] = static_cast<float>(
                double_of(call.arguments[4U + index], "ModelLODSettings", distance_error));
            if (!distance_error.empty()) {
                error = distance_error;
                return false;
            }
        }
        row.distance_to_lod1 = distances[0];
        row.distance_to_lod2 = distances[1];
        row.distance_to_lod3 = distances[2];
        row.distance_to_lod4 = distances[3];
        row.distance_to_cutoff = distances[4];
        out.rows.push_back(std::move(row));
    }
    if (out.rows.empty()) {
        error = "no ModelLODSettings rows";
        return false;
    }
    return true;
}

} // namespace igi::retail::config
