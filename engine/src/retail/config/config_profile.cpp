// Native transcriptions of the config.qvm player-profile natives.
//
// Artifact: retail IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja decompilations of 0x00403660, 0x004036B0, 0x00403750, 0x00403870,
// 0x004038E0, 0x00403C10, 0x00406220, 0x00405010 and 0x00405050 in
// assets/BinaryNinja/natives/{misc,go,config}; Radare2/angr records in docs/fidelity/evidence/
// 0x004036B0-*, 0x00403750-*, 0x00403870-*, 0x004038E0-* and the paired Frida runtime report
// docs/fidelity/evidence/0x004037c0-frida-runtime.json.
// Behavior/CFG: each function below is the retail body with the retail global addresses and
// strides; see the header for the record layout.
// Status/limits: STRUCTURALLY_VERIFIED against the retail instruction sequences; the argument
// resolution through the frame reader 0x004B8A20 is the documented ABI boundary (the port has
// no script VM), and the returned value block is a private static because the port's decoder
// discards it, exactly like the GOInRemap transcription.

#include "igi/retail/config/config_profile.hpp"

#include "igi/retail/config/config_graphic_options.hpp"

#include <cstring>

extern "C" {

// Retail 0x00BC2380. GOStart zeroes it, GO_Player increments it.
std::uint32_t Config_ProfileCount = 0U;

// The same global under the name the native differential harnesses use.
std::uint32_t& InputRemap_ActionCount = Config_ProfileCount;

// Retail 0x00BC2384.
std::uint32_t Config_ActiveProfileIndex = 0U;

// Retail 0x00BC79C8.
char Config_ContentControlPassword[igi::retail::config::content_control_password_capacity] = {};

namespace {

// Retail 0x004B8A20 reads the argument value of the frame at the given index. The host frame
// carries the already-resolved values, so the transcription indexes them directly.
[[nodiscard]] std::uint32_t resolved_argument(const igi::retail::config::QvmFrameView* frame,
                                             std::uint32_t index) noexcept
{
    return frame->argument_values[index];
}

// Retail 0x004B8A20 stores the argument's float bits in the frame slot; GO_InMouSens moves
// them into the record as a float.
[[nodiscard]] float resolved_real(const igi::retail::config::QvmFrameView* frame,
                                 std::uint32_t index) noexcept
{
    float value = 0.0F;
    const std::uint32_t bits = resolved_argument(frame, index);
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

// Sys_ResolveSymbol_23(frame, index, dest, 0x20): the retail copies the argument's text into
// the destination with a 0x20-byte limit. The host caller passes the text already resolved.
void copy_resolved_text(char* destination, std::size_t capacity, const char* text) noexcept
{
    std::size_t length = 0U;
    if (text != nullptr) {
        while (length + 1U < capacity && text[length] != '\0') {
            destination[length] = text[length];
            ++length;
        }
    }
    destination[length] = '\0';
}

// Retail builds {int32 0, undefined, float64 0.0, 0x00567C74} on the stack and copies 24 bytes
// to the destination buffer, then returns the destination. The port's decoder discards the
// result, so the transcription returns a private block instead (same deviation as GOInRemap).
[[nodiscard]] std::uint32_t* value_block() noexcept
{
    static std::uint32_t block[6] = {0U, 0U, 0U, 0U, 0x00567C74U, 0U};
    return block;
}

} // namespace

// Native: GOStart; retail VA 0x00403660.
// 0x00403660:  mov dword [0x00BC2380], 0
//              call 0x004B8A20 Script_GetArg(frame, 0) -> eax
//              mov [0x00BC2384], eax
//              build the six-dword value block and copy it to the destination
std::uint32_t* GOStart(const igi::retail::config::QvmFrameView* frame)
{
    Config_ProfileCount = 0U;
    Config_ActiveProfileIndex = resolved_argument(frame, 0U);
    return value_block();
}

// Native: GO_Player; retail VA 0x004036B0.
// 0x004036B0:  eax = [0x00BC2380] + 1; [0x00BC2380] = eax
//              dword [0x00BC2358 + 0x564 * count] = 0
//              call 0x00417090 (mov eax, 1; ret) -> eax
//              dword [0x00BC2384 + 0x564 * count] = eax
//              call 0x004B8A80 Sys_ResolveSymbol_23(frame, 0, &0x00BC2364[count * 345], 0x20)
// Retail obtains the default active mission by calling the constant-1 helper at
// 0x00417090; the transcription stores the same constant.
std::uint32_t* GOPlayer(const igi::retail::config::QvmFrameView* frame, const char* name)
{
    using namespace igi::retail::config;
    const std::uint32_t count = ++Config_ProfileCount;
    std::uint32_t* const block = InputRemap_ActionTable + (count * action_table_stride);
    block[record_action_count_offset] = 0U;
    block[record_active_mission_offset] = 1U;
    copy_resolved_text(reinterpret_cast<char*>(block + record_name_offset), profile_name_capacity,
                       name);
    (void)frame;
    return value_block();
}

// Native: GO_ActiveMission; retail VA 0x00403750.
// 0x00403750:  eax = [0x00BC2380]
//              call 0x004B8A20 Script_GetArg(frame, 0)
//              dword [0x00BC2384 + 0x564 * count] = eax
std::uint32_t* GOActiveMission(const igi::retail::config::QvmFrameView* frame)
{
    using namespace igi::retail::config;
    const std::uint32_t count = Config_ProfileCount;
    InputRemap_ActionTable[(count * action_table_stride) + record_active_mission_offset] =
        resolved_argument(frame, 0U);
    return value_block();
}

// Native: GO_InMouInv; retail VA 0x00403870.
// 0x00403870:  eax = [0x00BC2380]
//              call 0x004B8A20 Script_GetArg(frame, 0)
//              dword [0x00BC235C + 0x564 * count] = eax
std::uint32_t* GOInMouInv(const igi::retail::config::QvmFrameView* frame)
{
    using namespace igi::retail::config;
    const std::uint32_t count = Config_ProfileCount;
    InputRemap_ActionTable[(count * action_table_stride) + record_mouse_invert_offset] =
        resolved_argument(frame, 0U);
    return value_block();
}

// Native: GO_InMouSens; retail VA 0x004038E0.
// 0x004038E0:  eax = [0x00BC2380]
//              call 0x004B8A50 Profile_ValueSet(frame, 0) -> st0
//              fstp float [0x00BC2360 + 0x564 * count]
// Profile_ValueSet is the retail value reader: it takes the argument from the
// frame's constant table when the frame has no runtime stack, otherwise from the
// stack, and returns the real in st0. The host frame carries the resolved value,
// so the transcription stores it directly.
std::uint32_t* GOInMouSens(const igi::retail::config::QvmFrameView* frame)
{
    using namespace igi::retail::config;
    const std::uint32_t count = Config_ProfileCount;
    const float sensitivity = resolved_real(frame, 0U);
    std::memcpy(&InputRemap_ActionTable[(count * action_table_stride)
                                        + record_mouse_sensitivity_offset],
                &sensitivity, sizeof(sensitivity));
    return value_block();
}

// Native: GO_ContentControlPW; retail VA 0x00403C10.
// 0x00403C10:  call 0x004B8A80 Sys_ResolveSymbol_23(frame, 0, &0x00BC79C8, 0x20)
std::uint32_t* GOContentControlPW(const igi::retail::config::QvmFrameView* frame,
                                  const char* password)
{
    copy_resolved_text(Config_ContentControlPassword,
                       igi::retail::config::content_control_password_capacity, password);
    (void)frame;
    return value_block();
}

// Native: Config_GetActivePlayerProfileRecord; retail VA 0x00406220.
// 0x00406220:  eax = [0x00BC2384]
//              lea eax, [eax*4 + 0x00BC2388]; shl/sub/lea chain for the 0x564 stride
// The accessor returns &0x00BC2388[active * 345], which is 12 dwords past the
// active record's block base at 0x00BC2358.
std::uint32_t* Config_GetActivePlayerProfileRecord()
{
    using namespace igi::retail::config;
    return InputRemap_ActionTable + active_record_accessor_offset +
           (Config_ActiveProfileIndex * action_table_stride);
}

// Native: Config_GetActivePlayerProfileIndex; retail VA 0x00405010.
// 0x00405010:  block.integer = [0x00BC2384]; block.real = double([0x00BC2384]); type 0x00567C74
// (the retail fills the 24-byte block on the stack and copies it to the context)
void Config_GetActivePlayerProfileIndex(void* context)
{
    using namespace igi::retail::config;
    ScriptValueBlock block{};
    block.integer = static_cast<std::int32_t>(Config_ActiveProfileIndex);
    block.real = static_cast<double>(Config_ActiveProfileIndex);
    block.type_descriptor = script_real_type_descriptor;
    std::memcpy(context, &block, sizeof(block));
}

// Native: Config_SetActivePlayerProfileIndex; retail VA 0x00405050.
// 0x00405050:  call 0x004B8A20 Script_GetArg(frame, 0); [0x00BC2384] = eax
//              block {1, 0, 1.0, 0x00567C74}
void Config_SetActivePlayerProfileIndex(void* context, std::int32_t index)
{
    using namespace igi::retail::config;
    Config_ActiveProfileIndex = static_cast<std::uint32_t>(index);
    ScriptValueBlock block{};
    block.integer = 1;
    block.real = 1.0;
    block.type_descriptor = script_real_type_descriptor;
    std::memcpy(context, &block, sizeof(block));
}

} // extern "C"
