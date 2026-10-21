#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include "retail_model_scene.hpp"

#include "igi/retail/core/frame_natives.hpp"
#include "igi/retail/core/window_procedure.hpp"
#include "igi/retail/core/game_flow.hpp"
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/diagnostics/log_record.hpp"
#include "igi/retail/config/config_graphic_options.hpp"
#include "igi/retail/config/retail_config.hpp"
#include "igi/retail/config/retail_input_bindings.hpp"
#include "igi/retail/input/input_port.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/model/mef_bone_rig.hpp"
#include "igi/retail/model/mef_render_data.hpp"
#include "igi/retail/model/model_texture_package.hpp"
#include "igi/retail/render/trans_context.hpp"
#include "igi/retail/render/view_block.hpp"
#include "igi/retail/human/human_view.hpp"
#include "igi/retail/resource/resource_archive.hpp"
#include "igi/retail/resource/resource_cache.hpp"
#include "igi/retail/script/qvm_loader.hpp"
#include "igi/retail/script/qvm_object_calls.hpp"
#include "igi/retail/terrain/terrain_loaders.hpp"
#include "igi/retail/texture/model_texture_pixels.hpp"

#include "player_controller.hpp"
#include "developer_controls.hpp"
#include "igi/retail/core/fpu_state.hpp"


#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <d3d.h>
#include <ddraw.h>
#include <fstream>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>


namespace
{


using namespace igi;
using namespace igi::retail;

template <class Interface> struct ComOwner
{
    Interface* pointer = nullptr;

    ~ComOwner()
    {
        if (pointer != nullptr)
        {
            pointer->Release();
        }
    }

    ComOwner() = default;
    ComOwner(const ComOwner&) = delete;
    ComOwner& operator=(const ComOwner&) = delete;
};

void check_result(HRESULT result, const char* operation)
{
    if (FAILED(result))
    {
        std::ostringstream message;
        message << operation << " failed HRESULT=0x" << std::hex << static_cast<unsigned long>(result);
        throw std::runtime_error(message.str());
    }
}

void write_log(std::string_view message, diagnostics::LogLevel level = diagnostics::LogLevel::info)
{
    const auto record = diagnostics::Log_FormatTimestampedRecord(
        level, std::chrono::system_clock::now(), "model-scene", message);
    std::ofstream output("igi.log", std::ios::app);
    output << record;
    OutputDebugStringA(record.c_str());
}

struct Vector3
{
    float x = 0;
    float y = 0;
    float z = 0;
};

Vector3 subtract(Vector3 left, Vector3 right)
{
    return {left.x - right.x, left.y - right.y, left.z - right.z};
}

float dot(Vector3 left, Vector3 right)
{
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

Vector3 cross(Vector3 left, Vector3 right)
{
    return {left.y * right.z - left.z * right.y,
            left.z * right.x - left.x * right.z,
            left.x * right.y - left.y * right.x};
}

Vector3 normalize(Vector3 value)
{
    const float length = std::sqrt(dot(value, value));
    if (length < 0.000001F)
    {
        throw std::runtime_error("Degenerate diagnostic camera axis.");
    }
    return {value.x / length, value.y / length, value.z / length};
}

D3DMATRIX identity()
{
    D3DMATRIX result{};
    result._11 = result._22 = result._33 = result._44 = 1;
    return result;
}

float number(const script::QvmObjectValue& value)
{
    if (const auto* real = std::get_if<float>(&value))
    {
        return *real;
    }
    if (const auto* integer = std::get_if<std::int32_t>(&value))
    {
        return static_cast<float>(*integer);
    }
    throw std::runtime_error("Object parameter is not numeric.");
}

std::string text(const script::QvmObjectValue& value)
{
    if (const auto* result = std::get_if<std::string>(&value))
    {
        return *result;
    }
    throw std::runtime_error("Object parameter is not text.");
}

float safe_number(const script::QvmObjectValue& value, float fallback = 0.0f)
{
    if (const auto* real = std::get_if<float>(&value))
    {
        return *real;
    }
    if (const auto* integer = std::get_if<std::int32_t>(&value))
    {
        return static_cast<float>(*integer);
    }
    return fallback;
}

std::string safe_text(const script::QvmObjectValue& value)
{
    if (const auto* result = std::get_if<std::string>(&value))
    {
        return *result;
    }
    return {};
}

bool is_model_identifier(std::string_view s)
{
    const auto p1 = s.find('_');
    if (p1 == 0 || p1 == std::string_view::npos)
    {
        return false;
    }
    const auto p2 = s.find('_', p1 + 1);
    if (p2 == std::string_view::npos || p2 == p1 + 1)
    {
        return false;
    }
    return true;
}

// Real32x9 parameter conversion: 0x004EFBA0 -> 0x004B38E0.
// Rotation constructors at 0x004B3BE0/0x004B3BA0/0x004B3B60, statically translated.
// Matrix layout conversion at 0x004950F0 transposes the retail 3x3 into D3D row-vector layout.
// Runtime transform comparison remains pending; no claim of full native parity.
std::array<float, 9> orientation(float alpha, float beta, float gamma)
{
    const float sine_alpha = std::sin(alpha), cosine_alpha = std::cos(alpha);
    const float sine_beta = std::sin(beta), cosine_beta = std::cos(beta);
    const float sine_gamma = std::sin(gamma), cosine_gamma = std::cos(gamma);
    std::array<float, 9> current{cosine_gamma, -sine_gamma, 0, sine_gamma, cosine_gamma, 0, 0, 0, 1};
    const std::array<float, 9> rotate_beta{cosine_beta, 0, sine_beta, 0, 1, 0, -sine_beta, 0, cosine_beta};
    const std::array<float, 9> rotate_alpha{
        1, 0, 0, 0, cosine_alpha, -sine_alpha, 0, sine_alpha, cosine_alpha};
    for (const auto& right : {rotate_beta, rotate_alpha})
    {
        auto left = current;
        for (std::size_t row = 0; row < 3U; ++row)
            for (std::size_t column = 0; column < 3U; ++column)
            {
                // Literal 0x004B3904..0x004B391C x87 sequence, repeated for
                // each element by retail 0x004B38E0. C++ float temporaries
                // introduce additional rounding under the retail control word.
#if defined(__i386__) && defined(__GNUC__)
                __asm__ volatile(
                    "flds %1\n\tfmuls %2\n\tflds %3\n\tfmuls %4\n\tfaddp\n\t"
                    "flds %5\n\tfmuls %6\n\tfaddp\n\tfstps %0"
                    : "=m"(current[row * 3U + column])
                    : "m"(right[column]), "m"(left[row * 3U]),
                      "m"(right[6U + column]), "m"(left[row * 3U + 2U]),
                      "m"(right[3U + column]), "m"(left[row * 3U + 1U])
                    : "st", "st(1)", "memory");
#else
                throw std::logic_error("Retail matrix arithmetic requires the Windows x86 GCC target.");
#endif
            }
    }
    return current;
}

std::array<float, 9> transpose(const std::array<float, 9>& matrix)
{
    std::array<float, 9> result{};
    for (std::size_t row = 0U; row < 3U; ++row)
        for (std::size_t column = 0U; column < 3U; ++column)
        {
            result[row * 3U + column] = matrix[column * 3U + row];
        }
    return result;
}

std::array<float, 9> multiply(const std::array<float, 9>& left, const std::array<float, 9>& right)
{
    std::array<float, 9> result{};
    for (std::size_t row = 0U; row < 3U; ++row)
        for (std::size_t column = 0U; column < 3U; ++column)
        {
            for (std::size_t inner = 0U; inner < 3U; ++inner)
            {
                result[row * 3U + column] += left[row * 3U + inner] * right[inner * 3U + column];
            }
        }
    return result;
}

std::string fold_ascii(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
    return value;
}

struct Package
{
    model::MtpPackage metadata;
    resource::ResourceArchive meshes;
    resource::ResourceArchive textures;
};

struct PlacedModel
{
    std::array<double, 3> position{};
    std::array<float, 9> rotation{};
    const model::MtpInstance* instance = nullptr;
    Package* package = nullptr;
    std::shared_ptr<model::MefRenderData> geometry;
    std::shared_ptr<model::MefModel> mef;
    std::vector<std::string> attachment_ancestry;
};

struct Scene
{
    ComOwner<IDirectDraw7> direct_draw;
    ComOwner<IDirectDrawSurface7> primary;
    ComOwner<IDirectDrawSurface7> back_buffer;
    ComOwner<IDirectDrawSurface7> depth_buffer;
    ComOwner<IDirectDrawClipper> clipper;
    ComOwner<IDirect3D7> direct_3d;
    ComOwner<IDirect3DDevice7> device;
    std::vector<std::unique_ptr<ComOwner<IDirectDrawSurface7>>> texture_surfaces;
    std::map<std::string, IDirectDrawSurface7*> uploaded_textures;
    std::vector<std::unique_ptr<Package>> packages;
    // The character skeletons, read from the retail animation files the model
    // packages name. A skinned MEF carries no skeleton of its own.
    model::IffSkeletonTable skeletons;
    std::vector<PlacedModel> objects;
    terrain::TerrainLevelData terrain_data;
    std::vector<std::unique_ptr<ComOwner<IDirectDrawSurface7>>> terrain_texture_surfaces;
    std::vector<IDirectDrawSurface7*> terrain_textures;
    std::array<double, 3> origin{};
    // The level origin is the HumanPlayer task's feet position.  These values are replaced
    // by load() once that task has been decoded; keeping the fallback at a standing eye avoids
    // the old elevated diagnostic camera if a malformed level has no player task.
    Vector3 camera_position{0, 0, 0.69F};
    Vector3 camera_target{0, 1, 0.69F};
    // The retail input port: the action bindings come from config.qvm, and the
    // player controller implements the retail human movement model.
    input::InputPort input_port;
    host::PlayerController player;
    host::TerrainCollisionGrid collision_grid;
    // The retail look rate is applied to the mouse counts the platform reports.
    double look_delta_x = 0.0;
    double look_delta_y = 0.0;
    bool player_ready = false;
    bool player_motion_available = true;
    bool raw_mouse_enabled = false;
    bool owns_mouse_clip = false;

    // Windows device adapter: relative counts continue when the cursor reaches an edge.
    void set_mouse_active(bool active)
    {
        if (active && raw_mouse_enabled && window != nullptr)
        {
            RECT rectangle{};
            if (GetClientRect(window, &rectangle))
            {
                POINT corners[2]{{rectangle.left, rectangle.top}, {rectangle.right, rectangle.bottom}};
                if (ClientToScreen(window, &corners[0]) && ClientToScreen(window, &corners[1]))
                {
                    rectangle = {corners[0].x, corners[0].y, corners[1].x, corners[1].y};
                    owns_mouse_clip = ClipCursor(&rectangle) != FALSE;
                }
            }
        }
        else if (owns_mouse_clip)
        {
            ClipCursor(nullptr);
            owns_mouse_clip = false;
        }
        if (!active)
        {
            for (std::uint32_t index = 0; index < static_cast<std::uint32_t>(input::Action::count); ++index)
                for (const auto& binding : input_port.Bindings(static_cast<input::Action>(index)))
                    if (binding.device == input::Device::keyboard)
                        input_port.SetKeyDown(binding.code, false);
                    else
                        input_port.SetMouseButtonDown(static_cast<int>(binding.code), false);
            input_port.Run();
        }
        cursor_valid = false;
        static_cast<void>(input_port.TakeMouseDelta());
    }
    LONG cursor_x = 0;
    LONG cursor_y = 0;
    bool cursor_valid = false;
    unsigned int key_messages = 0;
    unsigned int mouse_messages = 0;
    LONG last_mouse_x = 0;
    LONG last_mouse_y = 0;
    struct DemoEvent {
        unsigned int frame;
        unsigned int message;
        WPARAM wparam;
        LPARAM lparam;
    };
    std::array<float, 2> last_look_delta{0.0F, 0.0F};
    std::vector<DemoEvent> input_demo_events;
    unsigned int input_demo_frame = 0;
    unsigned int input_demo_next = 0;
    HWND window = nullptr;
    bool first_frame = true;
    bool windowed = true;
    bool exclusive = false;
    static constexpr float world_scale = 1.0F / 4096.0F;

    // Owner-requested inspection extension restored from a0ff6d94; not a retail native.
    igi::developer::DeveloperControls developer_controls;
    bool dev_next_level_requested = false;
    float dev_yaw = 0.0F;
    float dev_pitch = 0.0F;
    float dev_speed = 50.0F; // Existing inspection speed, not HumanPlayer movement data.
    Vector3 dev_saved_position{};
    Vector3 dev_saved_target{};

    void dev_camera_reset()
    {
        // World is X east, Y north, Z up. Yaw is measured from +Y towards +X, so
        // forward = (sin yaw, cos yaw, 0) and yaw = atan2(forward.x, forward.y).
        const Vector3 forward = normalize(subtract(camera_target, camera_position));
        dev_yaw = std::atan2(forward.x, forward.y);
        dev_pitch = std::asin(std::clamp(forward.z, -1.0F, 1.0F));
    }

    void update_dev_camera(float delta_seconds)
    {
        input_port.Run();
        const auto look = input_port.TakeMouseDelta();
        dev_yaw += look[0] * host::PlayerController::look_radians_per_count;
        dev_pitch = std::clamp(dev_pitch - look[1] * host::PlayerController::look_radians_per_count,
                               -1.5533F, 1.5533F);

        // Must match dev_camera_reset(): yaw is measured from +Y towards +X, so
        // forward = (sin yaw, cos yaw, 0) and right = forward x up = (cos yaw, -sin yaw, 0).
        // The previous -cos / +sin pair made W drive backwards and turned the view the wrong way.
        const float cosine_pitch = std::cos(dev_pitch);
        const Vector3 forward{
            std::sin(dev_yaw) * cosine_pitch, std::cos(dev_yaw) * cosine_pitch, std::sin(dev_pitch)};
        const Vector3 right{std::cos(dev_yaw), -std::sin(dev_yaw), 0.0F};

        Vector3 move{0, 0, 0};
        const bool key_w = input_port.ActionDown(input::Action::move_up);
        const bool key_s = input_port.ActionDown(input::Action::move_down);
        const bool key_a = input_port.ActionDown(input::Action::move_left);
        const bool key_d = input_port.ActionDown(input::Action::move_right);
        const bool key_up = input_port.ActionDown(input::Action::jump);
        const bool key_down = input_port.ActionDown(input::Action::crouch);

        if (key_w)
        {
            move.x += forward.x;
            move.y += forward.y;
            move.z += forward.z;
        }
        if (key_s)
        {
            move.x -= forward.x;
            move.y -= forward.y;
            move.z -= forward.z;
        }
        if (key_a)
        {
            move.x -= right.x;
            move.y -= right.y;
            move.z -= right.z;
        }
        if (key_d)
        {
            move.x += right.x;
            move.y += right.y;
            move.z += right.z;
        }
        if (key_up)
        {
            move.z += 1.0F;
        }
        if (key_down)
        {
            move.z -= 1.0F;
        }

        const float speed = dev_speed;
        camera_position.x += move.x * speed * delta_seconds;
        camera_position.y += move.y * speed * delta_seconds;
        camera_position.z += move.z * speed * delta_seconds;

        // Install the new eye position through the retail transform context
        // (Transcontext_SetActiveTransContext, VA 0x00497E70; camera position is +0x28,
        // globals 0x00BCAB08/10/18). The renderer then reads the position back from the
        // active context, so the retail global stays authoritative.
        render::Transcontext_SetCameraPosition(camera_position.x, camera_position.y, camera_position.z);
        render::Transcontext_SetActiveTransContext(render::Transcontext_GetActiveTransContext());
        const std::array<double, 3> installed = render::Transcontext_GetCameraPosition();
        camera_position = {static_cast<float>(installed[0]),
                           static_cast<float>(installed[1]),
                           static_cast<float>(installed[2])};

        camera_target = {
            camera_position.x + forward.x, camera_position.y + forward.y, camera_position.z + forward.z};
    }

    // Retail main (0x0048F544) establishes config.qvm before the Game class
    // registers Level_Start (0x00415530) as phase 0. The port keeps that split:
    // the input bindings, sensitivity and inversion are applied once at startup,
    // not on every level load.
    void load_startup_config(const std::filesystem::path& root)
    {
        config::ScriptProgram config_program;
        config::InputConfig input_config;
        std::string input_config_error;
        if (!config::read_script_file(root / "config.qvm", config_program, input_config_error)
            || !config::decode_input_config(config_program, input_config, input_config_error)
            || !config::apply_input_config(input_config, input_port, input_config_error)) {
            throw std::runtime_error("config.qvm: " + input_config_error);
        }
        write_log("Flow stage config.qvm: applied " +
                  std::to_string(input_config.bindings.size()) + " action bindings");
    }

    void load(const std::filesystem::path& root, unsigned int level_number)
    {
        if (mem::g_9422fc == 0U)
        {
            mem::Sys_CloseFileStream_2();
        }
        resource::ResourceCache cache;

        const auto location = root / "missions" / "location0";
        const std::string level_name = "level" + std::to_string(level_number);
        const auto level = location / level_name;

        write_log("Flow stage Level_Start 0x00415B30: level " + level_name);

        // Retail Level_Start (0x00415B30) loads terrain before the resource
        // archives and before objects.qsc: Terrain_LoadTexturesAndLightmaps
        // (0x004C5F00) at 0x00415C31, then Terrain_Load (0x004C6240) at
        // 0x00415C44. The QSC task stage follows objects.qsc below.
        const auto terrain_dir_lower = level / "terrain";
        const auto terrain_dir_upper = level / "TERRAIN";
        const auto terrain_dir = std::filesystem::exists(terrain_dir_lower) ? terrain_dir_lower : terrain_dir_upper;
        bool terrain_files_loaded = false;
        if (std::filesystem::exists(terrain_dir))
        {
            const std::string terrain_base = (terrain_dir / "terrain").string();
            const std::int32_t lightmaps =
                terrain::Terrain_LoadTexturesAndLightmaps(&terrain_data, terrain_base.c_str());
            terrain_files_loaded =
                terrain::Terrain_Load(&terrain_data, terrain_base.c_str()) != nullptr;
            write_log("Level terrain files: lightmaps=" + std::to_string(lightmaps) +
                      " loaded=" + std::string(terrain_files_loaded ? "true" : "false"));
            write_log("Flow stage terrain+TEX 0x004C5F00/0x004C6240: "
                      "lightmaps=" + std::to_string(lightmaps));
        }

        for (const auto& descriptor :
             {std::pair(location / "common", std::string("location0")), std::pair(level, level_name)})
        {
            auto package = std::make_unique<Package>();
            const auto bytes =
                resource::Host_LoadResourceFile(cache, descriptor.first / (descriptor.second + ".mtp"));
            package->metadata = model::MtpPackage::Host_ParseModelTexturePackage(bytes->bytes);
            package->meshes =
                resource::Host_LoadResourceArchive(descriptor.first / "models" / (descriptor.second + ".res"));
            package->textures =
                resource::Host_LoadResourceArchive(descriptor.first / "textures" / (descriptor.second + ".res"));
            write_log("MTP/RES loaded: " + descriptor.second +
                      " models=" + std::to_string(package->meshes.entries.size()) +
                      " texture_names=" + std::to_string(package->metadata.texture_names.size()));
            packages.push_back(std::move(package));
        }
        write_log("Flow stage RES/MTP: packages=" + std::to_string(packages.size()));
        // The retail animation files hold the character skeletons. Each model
        // package names its animation files in its 'BANM' chunk, and the retail
        // loads them as "<prefix>anims/<name>.iff".
        {
            std::vector<model::IffSkeleton> loaded_skeletons;
            std::vector<std::string> animation_names;
            for (const auto& package : packages)
            {
                for (const std::string& name : package->metadata.animation_names)
                {
                    if (std::find(animation_names.begin(), animation_names.end(), name) ==
                        animation_names.end())
                    {
                        animation_names.push_back(name);
                    }
                }
            }
            for (const std::string& name : animation_names)
            {
                std::optional<model::IffSkeleton> skeleton;
                for (const std::string& folder : {"common", "COMMON"})
                {
                    for (const std::string& extension : {".iff", ".IFF"})
                    {
                        const auto path = root / folder / "anims" / (name + extension);
                        skeleton = model::IffSkeleton_Read(path);
                        if (skeleton.has_value())
                        {
                            break;
                        }
                    }
                    if (skeleton.has_value())
                    {
                        break;
                    }
                }
                if (skeleton.has_value())
                {
                    loaded_skeletons.push_back(*skeleton);
                }
                else
                {
                    write_log("Animation skeleton not found: " + name, diagnostics::LogLevel::warning);
                }
            }
            // The retail keeps the skeletons in a table indexed by the object id
            // the BOSH chunk holds, and a character uses the entry for its type.
            skeletons = model::IffSkeletonTable_Build(loaded_skeletons);
            write_log("Character skeletons loaded: " + std::to_string(loaded_skeletons.size()) +
                      " of " + std::to_string(animation_names.size()) + " animation files; " +
                      "object ids " + std::to_string(skeletons.by_object_id.size()));
            write_log("Flow stage MEF+IFF skeleton: skeletons=" +
                      std::to_string(loaded_skeletons.size()));
        }

        // Retail loads humanplayer.qvm before objects.qsc creates any HumanPlayer
        // instance: the file defines the general movement parameters the class
        // applies. Keep it ahead of the QVM object pass so the controller never
        // runs with unset parameters.
        config::ScriptProgram humanplayer_program;
        config::HumanPlayerGeneral general;
        std::string humanplayer_error;
        if (!config::read_script_file(root / "humanplayer" / "humanplayer.qvm",
                                      humanplayer_program, humanplayer_error)
            || !config::decode_human_player_general(humanplayer_program, general,
                                                    humanplayer_error)) {
            throw std::runtime_error("humanplayer.qvm: " + humanplayer_error);
        }
        const auto root_motion_scale = general.root_motion_scale();
        const auto jump_horizontal = general.jump_horizontal_kmh();
        const auto jump_vertical = general.jump_vertical_kmh();
        const auto air_control = general.air_control_kmh();
        if (!root_motion_scale || !jump_horizontal || !jump_vertical || !air_control) {
            throw std::runtime_error(
                "humanplayer.qvm: DefineHumanPlayerGeneral is missing a movement argument");
        }
        player.set_parameters({*root_motion_scale, *jump_horizontal, *jump_vertical,
                               *air_control});
        write_log("Flow stage humanplayer.qvm: root-motion scale " +
                  std::to_string(*root_motion_scale) + ", jump " +
                  std::to_string(*jump_horizontal) + "/" +
                  std::to_string(*jump_vertical) + " km/h, air control " +
                  std::to_string(*air_control) + " km/h");

        write_log("Flow stage QVM objects: objects.qsc");
        const auto script = script::Host_LoadQvmScript(cache, level / "objects.qsc");
        const auto calls = script::Qvm_ReadObjectCalls(script.program());
        const auto magic_script = script::Host_LoadQvmScript(cache, root / "MAGICOBJ" / "MAGICOBJ.QVM");
        std::map<std::string, std::string> magic_object_models;
        for (const auto& call : script::Qvm_ReadObjectCalls(magic_script.program()))
        {
            if (call->function_name == "DefineMagicObj" && call->arguments.size() >= 2U)
            {
                magic_object_models[fold_ascii(text(call->arguments[0]))] = text(call->arguments[1]);
            }
        }
        write_log("Loaded MAGICOBJ.QVM definitions=" + std::to_string(magic_object_models.size()));
        bool player_origin_found = false;
        for (const auto& call : calls)
        {
            if (call->function_name == "Task_New" && call->arguments.size() > 8U &&
                text(call->arguments[1]) == "HumanPlayer")
            {
                for (std::size_t axis = 0; axis < 3U; ++axis)
                {
                    origin[axis] = number(call->arguments[3U + axis]);
                }
                // Human_CameraEyePosition uses the standing 0.69 m eye offset.  The host
                // renderer uses metres, and the level origin is now the local player feet.
                constexpr float standing_eye_metres = 0.69F;
                camera_position = {0.0F, 0.0F, standing_eye_metres};
                camera_target = {0.0F, 1.0F, standing_eye_metres};
                // The human starts on the ground at the task's feet position.
                // The retail world is in units (4096 to the metre) and the
                // level origin is the player's own feet, so both are the same
                // value in the same units.
                // Human_Init binds its scalar heading after the position; Human_OnEvent
                // copies that heading to +0x508 and builds Mat33_RotateZ. Its forward
                // column is (-sin heading, cos heading, 0), opposite the host yaw sign.
                const double spawn_heading = number(call->arguments[6]);
                player.reset({origin[0], origin[1], origin[2]}, -spawn_heading, 0.0,
                             {origin[0], origin[1], origin[2]},
                             1.0 / host::PlayerController::units_per_metre);
                camera_target = {camera_position.x + static_cast<float>(std::sin(player.yaw())),
                                 camera_position.y + static_cast<float>(std::cos(player.yaw())),
                                 camera_position.z};
                write_log("HumanPlayer authored heading=" + std::to_string(spawn_heading) +
                          " host yaw=" + std::to_string(player.yaw()));
                player_ready = true;
                player_motion_available = true;
                player_origin_found = true;
                break;
            }
        }
        if (!player_origin_found) throw std::runtime_error("Level has no HumanPlayer spawn; camera is unavailable.");
        write_log("HumanPlayer spawn camera=installed");
        write_log("Flow stage QVM objects: player task bound, models follow");

        // The player's skinned 000_01_1 model selects the 000 skeleton (rig object id 0,
        // listed in the common MTP BANM). Its animation IFF carries the clips the retail
        // human state table at 0x00538870 indexes. The clip reader walks the IFF by tag
        // because the FORM sizes in these files are not trustworthy.
        {
            bool clips_loaded = false;
            for (const char* folder : {"COMMON", "common"})
            {
                for (const char* anims : {"ANIMS", "anims"})
                {
                    for (const char* file : {"000.IFF", "000.iff"})
                    {
                        const auto path = root / folder / anims / file;
                        if (!std::filesystem::exists(path))
                        {
                            continue;
                        }
                        try
                        {
                            auto clips = host::read_animation_clips(path);
                            const auto forward = std::find_if(
                                clips.begin(), clips.end(),
                                [](const host::AnimationClip& clip) { return clip.id == 4U; });
                            if (forward == clips.end() || forward->root_keys.empty())
                            {
                                throw std::runtime_error(
                                    "clip 4 (forward) has no root translation track");
                            }
                            write_log("Animation clips: " + path.string() + " clips=" +
                                      std::to_string(clips.size()) + " forward=" +
                                      std::to_string(forward->duration_ms) + " ms");
                            player.set_locomotion_clips(std::move(clips));
                            clips_loaded = true;
                        }
                        catch (const std::exception& exception)
                        {
                            write_log(std::string("Animation clips: ") + exception.what(),
                                      diagnostics::LogLevel::warning);
                        }
                        break;
                    }
                    if (clips_loaded)
                    {
                        break;
                    }
                }
                if (clips_loaded)
                {
                    break;
                }
            }
            if (!clips_loaded)
            {
                write_log("Animation clips: player clips not loaded; ground movement will fail",
                          diagnostics::LogLevel::warning);
            }
        }

        // Apply the terrain QSC tasks after objects.qsc. Retail's Terrain_Load
        // registers the CreateTerrainMaterial/CreateTerrainTileMap classes and
        // runs TERRAIN.QVM; the level script then supplies TextureModifier,
        // HeightMap and TerrainLightMap tasks.
        if (terrain_files_loaded)
        {
            const bool terrain_loaded = terrain_data.Host_LoadLevelTerrain(terrain_dir, calls, true);
            write_log("Level terrain loaded=" + std::string(terrain_loaded ? "true" : "false") +
                      " nodes=" + std::to_string(terrain_data.contour_nodes.size()) +
                      " layers=" + std::to_string(terrain_data.texture_layers.size()));
            if (direct_draw.pointer != nullptr)
            {
                upload_terrain_textures();
            }
        }
        std::map<std::string, std::shared_ptr<model::MefRenderData>> geometry_cache;
        std::map<std::string, std::shared_ptr<model::MefModel>> mef_cache;
        std::size_t missing = 0;
        for (const auto& call : calls)
        {
            const auto& arguments = call->arguments;
            if (call->function_name != "Task_New" || arguments.size() < 8U)
            {
                continue;
            }
            const auto type = safe_text(arguments[1]);
            if (type.empty())
            {
                continue;
            }

            const bool fence_task = (type == "Fence" || type == "Cabinet");
            const bool wire_task = (type == "Wire");
            const bool door_task = (type == "Door");
            const bool switch_task = (type == "Switch");
            const bool scamera_task = (type == "SCamera");
            const bool vehicle_task = (type == "Car" || type == "Heli");
            // Every positioned Task_New participates in the generic model resolver.  The
            // specialised branches below preserve retail argument layouts where known;
            // unknown task types use the standard layout and then scan their arguments for a
            // real MTP/MEF identifier.  This keeps objects visible without inventing AI or
            // interaction behaviour for task types whose runtime semantics remain unverified.

            PlacedModel object;
            std::string model_name;

            if (fence_task) // Fence / Cabinet: pos@3..5, heading@6, model@7
            {
                if (arguments.size() < 8U) continue;
                for (std::size_t axis = 0U; axis < 3U; ++axis)
                {
                    object.position[axis] = safe_number(arguments[3U + axis]);
                }
                const float heading = safe_number(arguments[6]);
                object.rotation = orientation(0.0f, 0.0f, heading);
                model_name = safe_text(arguments[7]);
            }
            else if (wire_task) // Wire: start pos@3..5, stop pos@6..8, model@9
            {
                if (arguments.size() < 10U) continue;
                for (std::size_t axis = 0U; axis < 3U; ++axis)
                {
                    object.position[axis] = safe_number(arguments[3U + axis]);
                }
                const float dx = safe_number(arguments[6]) - safe_number(arguments[3]);
                const float dy = safe_number(arguments[7]) - safe_number(arguments[4]);
                const float yaw = std::atan2(dy, dx);
                object.rotation = orientation(0.0f, 0.0f, yaw);
                model_name = safe_text(arguments[9]);
            }
            else if (door_task) // Door: pos@3..5, ori@9..11, model@12
            {
                if (arguments.size() < 13U) continue;
                for (std::size_t axis = 0U; axis < 3U; ++axis)
                {
                    object.position[axis] = safe_number(arguments[3U + axis]);
                }
                object.rotation = orientation(safe_number(arguments[9]),
                                              safe_number(arguments[10]),
                                              safe_number(arguments[11]));
                model_name = safe_text(arguments[12]);
            }
            else if (switch_task) // Switch: pos@3..5, ori@6..8, model@11
            {
                if (arguments.size() < 12U) continue;
                for (std::size_t axis = 0U; axis < 3U; ++axis)
                {
                    object.position[axis] = safe_number(arguments[3U + axis]);
                }
                object.rotation = orientation(safe_number(arguments[6]),
                                              safe_number(arguments[7]),
                                              safe_number(arguments[8]));
                model_name = safe_text(arguments[11]);
            }
            else if (scamera_task) // SCamera: pos@3..5, ori@6..8, model@10
            {
                if (arguments.size() < 11U) continue;
                for (std::size_t axis = 0U; axis < 3U; ++axis)
                {
                    object.position[axis] = safe_number(arguments[3U + axis]);
                }
                object.rotation = orientation(safe_number(arguments[6]),
                                              safe_number(arguments[7]),
                                              safe_number(arguments[8]));
                model_name = safe_text(arguments[10]);
            }
            else if (vehicle_task) // Car / Heli: pos@3..5, heading@8, model@13
            {
                if (arguments.size() < 14U) continue;
                for (std::size_t axis = 0U; axis < 3U; ++axis)
                {
                    object.position[axis] = safe_number(arguments[3U + axis]);
                }
                const float heading = safe_number(arguments[8]);
                object.rotation = orientation(0.0f, 0.0f, heading);
                model_name = safe_text(arguments[13]);
            }
            else // Standard positioned objects: pos@3..5, ori@6..8, model@9
            {
                if (arguments.size() < 10U) continue;
                for (std::size_t axis = 0U; axis < 3U; ++axis)
                {
                    object.position[axis] = safe_number(arguments[3U + axis]);
                }
                object.rotation = orientation(safe_number(arguments[6]),
                                              safe_number(arguments[7]),
                                              safe_number(arguments[8]));
                model_name = safe_text(arguments[9]);
            }

            // Fallback model identifier search across arguments if empty or non-matching
            if (model_name.empty() || !is_model_identifier(model_name))
            {
                for (std::size_t ai = 6U; ai < arguments.size(); ++ai)
                {
                    const auto candidate = safe_text(arguments[ai]);
                    if (is_model_identifier(candidate))
                    {
                        model_name = candidate;
                        break;
                    }
                }
            }

            if (model_name.empty())
            {
                continue;
            }

            const std::string folded_target = fold_ascii(model_name);
            for (auto package_iterator = packages.rbegin(); package_iterator != packages.rend();
                 ++package_iterator)
            {
                for (const auto& instance : (*package_iterator)->metadata.instances)
                {
                    if (fold_ascii(instance.instance_name) == folded_target)
                    {
                        object.instance = &instance;
                        object.package = package_iterator->get();
                        break;
                    }
                    if (instance.model_index < (*package_iterator)->metadata.model_names.size() &&
                        fold_ascii((*package_iterator)->metadata.model_names[instance.model_index]) == folded_target)
                    {
                        object.instance = &instance;
                        object.package = package_iterator->get();
                        break;
                    }
                }
                if (object.instance != nullptr)
                {
                    break;
                }
            }
            if (object.instance == nullptr)
            {
                // DynObj_Missing - 0x004CEC10: empty names return null; unknown nonempty names try "missing".
                // radare2 and CLEAN agree on this branch; full fallback-registry runtime parity is pending.
                for (auto package_iterator = packages.rbegin(); package_iterator != packages.rend();
                     ++package_iterator)
                {
                    for (const auto& instance : (*package_iterator)->metadata.instances)
                    {
                        if (instance.instance_name == "missing")
                        {
                            object.instance = &instance;
                            object.package = package_iterator->get();
                            break;
                        }
                    }
                    if (object.instance != nullptr)
                    {
                        break;
                    }
                }
                if (object.instance == nullptr)
                {
                    ++missing;
                    write_log("Unrendered source model: '" + model_name + "'; no resolved retail fallback",
                              diagnostics::LogLevel::warning);
                    continue;
                }
            }
            const auto& mesh_name = object.package->metadata.model_names.at(object.instance->model_index);
            const auto key =
                std::to_string(reinterpret_cast<std::uintptr_t>(object.package)) + ":" + mesh_name;
            auto& geometry = geometry_cache[key];
            auto& parsed_mef = mef_cache[key];
            if (!geometry)
            {
                const auto* entry = object.package->meshes.find("LOCAL:models/" + mesh_name + ".mef");
                if (entry == nullptr)
                {
                    throw std::runtime_error("MTP mesh resource missing: " + mesh_name);
                }
                parsed_mef = std::make_shared<model::MefModel>(
                    model::MefModel::DynObj_LoadModelMesh(entry->body));
                write_log("Loaded MEF model=" + mesh_name +
                          " ATTA records=" + std::to_string(parsed_mef->attachment_records.size()));
                if (parsed_mef->render_format == model::MefRenderFormat::skinned)
                {
                    // Retail 0x0049B700 palette evaluation is ported (mef_skinner.cpp). The
                    // palette comes from the retail animation IFF the model package names,
                    // because the MEF itself carries no skeleton.
                    try
                    {
                        model::MefBoneRig rig{};
                        geometry = std::make_shared<model::MefRenderData>(
                            model::Display_DecodeMeshRenderData(*parsed_mef, skeletons, &rig));
                        const model::IffSkeleton* skeleton =
                            skeletons.Find(rig.object_id);
                        write_log("Skinned MEF model=" + mesh_name +
                                  " rig_object_id=" + std::to_string(rig.object_id) +
                                  " bones=" + std::to_string(skeleton != nullptr
                                                                 ? skeleton->BoneCount()
                                                                 : 0U) +
                                  " influence_agreement=" + std::to_string(rig.disagreement) +
                                  " units");
                    }
                    catch (const std::exception& exception)
                    {
                        ++missing;
                        write_log("Skinned MEF model=" + mesh_name + " not rendered: " +
                                      exception.what(),
                                  diagnostics::LogLevel::warning);
                        continue;
                    }
                }
                else
                {
                    geometry = std::make_shared<model::MefRenderData>(
                        model::Display_DecodeMeshRenderData(*parsed_mef, skeletons));
                }
            }
            object.geometry = geometry;
            object.mef = parsed_mef;
            object.attachment_ancestry.push_back(fold_ascii(mesh_name));
            std::ostringstream record;
            record << std::setprecision(17)
                   << "placement source=OBJECTS.QVM code_offset=" << call->instruction_offset
                   << " type=" << type << " model=" << model_name << " position=" << object.position[0] << ","
                   << object.position[1] << "," << object.position[2] << " orientation=";
            for (float component : object.rotation)
            {
                record << component << ",";
            }
            write_log(record.str());
            objects.push_back(std::move(object));
        }
        if (objects.empty())
        {
            throw std::runtime_error("Object scene is empty.");
        }

        // Layout and transform conventions are cross-checked against OpenIGI's
        // MefAttachment/WorldScene at revision dade2b1a9c2c446462adf7d13a225926eabc96f0
        // (GPL-3.0; see that repository's LICENSE). Retail accessor evidence independently
        // establishes the 0x44-byte ATTA record stride.
        std::size_t attached_models = 0U;
        constexpr std::size_t max_scene_models = 8192U;
        constexpr std::size_t max_attachment_depth = 32U;
        for (std::size_t parent_index = 0U;
             parent_index < objects.size() && objects.size() < max_scene_models;
             ++parent_index)
        {
            const PlacedModel parent = objects[parent_index];
            if (parent.mef == nullptr || parent.attachment_ancestry.size() >= max_attachment_depth)
            {
                continue;
            }

            for (const model::MefAttachmentRecord& attachment : parent.mef->attachment_records)
            {
                const auto definition = magic_object_models.find(fold_ascii(attachment.name));
                if (definition == magic_object_models.end())
                {
                    continue;
                }
                const std::string child_name = definition->second;
                const std::string folded_child_name = fold_ascii(child_name);
                if (std::find(parent.attachment_ancestry.begin(), parent.attachment_ancestry.end(),
                              folded_child_name) != parent.attachment_ancestry.end())
                {
                    continue;
                }

                Package* child_package = nullptr;
                const model::MtpInstance* child_instance = nullptr;
                for (auto package_iterator = packages.rbegin(); package_iterator != packages.rend();
                     ++package_iterator)
                {
                    for (const model::MtpInstance& candidate : (*package_iterator)->metadata.instances)
                    {
                        if (candidate.model_index < (*package_iterator)->metadata.model_names.size() &&
                            fold_ascii((*package_iterator)->metadata.model_names[candidate.model_index]) ==
                                folded_child_name)
                        {
                            child_package = package_iterator->get();
                            child_instance = &candidate;
                            break;
                        }
                    }
                    if (child_instance != nullptr)
                    {
                        break;
                    }
                }
                if (child_package == nullptr || child_instance == nullptr)
                {
                    continue;
                }

                const auto* child_entry = child_package->meshes.find("LOCAL:models/" + child_name + ".mef");
                if (child_entry == nullptr)
                {
                    continue;
                }
                const auto child_key =
                    std::to_string(reinterpret_cast<std::uintptr_t>(child_package)) + ":" + child_name;
                auto& child_geometry = geometry_cache[child_key];
                auto& child_mef = mef_cache[child_key];
                if (!child_geometry)
                {
                    try
                    {
                        child_mef = std::make_shared<model::MefModel>(
                            model::MefModel::DynObj_LoadModelMesh(child_entry->body));
                        try
                        {
                            child_geometry = std::make_shared<model::MefRenderData>(
                                model::Display_DecodeMeshRenderData(*child_mef, skeletons));
                        }
                        catch (const std::exception& exception)
                        {
                            write_log("Skinned ATTA MEF model=" + child_name + " not rendered: " +
                                          exception.what(),
                                      diagnostics::LogLevel::warning);
                            continue;
                        }
                    }
                    catch (const std::exception& exception)
                    {
                        write_log("Skipping ATTA model=" + child_name + " reason=" + exception.what(),
                                  diagnostics::LogLevel::warning);
                        continue;
                    }
                }

                PlacedModel child{};
                child.instance = child_instance;
                child.package = child_package;
                child.geometry = child_geometry;
                child.mef = child_mef;
                child.rotation = multiply(parent.rotation, transpose(attachment.orientation));
                for (std::size_t row = 0U; row < 3U; ++row)
                {
                    const float offset = parent.rotation[row * 3U] * attachment.position[0] +
                                         parent.rotation[row * 3U + 1U] * attachment.position[1] +
                                         parent.rotation[row * 3U + 2U] * attachment.position[2];
                    child.position[row] = parent.position[row] + offset;
                }
                child.attachment_ancestry = parent.attachment_ancestry;
                child.attachment_ancestry.push_back(folded_child_name);
                objects.push_back(std::move(child));
                ++attached_models;
                if (objects.size() >= max_scene_models)
                {
                    break;
                }
            }
        }
        write_log("Loaded placed model poses=" + std::to_string(objects.size()) +
                  "; attached_models=" + std::to_string(attached_models) +
                  "; unresolved_source_models=" + std::to_string(missing) +
                  "; the skinned models use the retail animation skeleton; the remaining task types are not yet rendered.");
    }

    std::uint32_t render_width = 1024U;
    std::uint32_t render_height = 768U;

    void release_render_target()
    {
        // The device holds a reference to the render target, so release it first.
        if (device.pointer != nullptr)
        {
            device.pointer->Release();
            device.pointer = nullptr;
        }
        if (depth_buffer.pointer != nullptr)
        {
            depth_buffer.pointer->Release();
            depth_buffer.pointer = nullptr;
        }
        if (back_buffer.pointer != nullptr)
        {
            back_buffer.pointer->Release();
            back_buffer.pointer = nullptr;
        }
    }

    void apply_device_state()
    {
        check_result(device.pointer->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE), "Enable depth");
        check_result(device.pointer->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE),
                     "Disable fixed-function lighting for diagnostic scene");
        check_result(device.pointer->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE),
                     "Set diagnostic culling");
        check_result(device.pointer->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE),
                     "Enable cutout alpha");
        check_result(device.pointer->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATER),
                     "Alpha function");
        check_result(device.pointer->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0), "Alpha threshold");
        check_result(device.pointer->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1),
                     "Texture color operation");
        check_result(device.pointer->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE),
                     "Texture color argument");
        check_result(device.pointer->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1),
                     "Texture alpha operation");
        check_result(device.pointer->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE),
                     "Texture alpha argument");
        check_result(device.pointer->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR),
                     "Texture min filter");
        check_result(device.pointer->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR),
                     "Texture mag filter");
    }

    // Creates the render target, Z-buffer and Direct3D device at the requested client size.
    // The back buffer must match the blit destination, otherwise a maximized/fullscreen window
    // presents black; the retail game likewise recreates these on a mode change.
    void create_render_target(std::uint32_t width, std::uint32_t height, bool log_ready)
    {
        DDSURFACEDESC2 description{};
        description.dwSize = sizeof(description);
        description.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
        description.dwWidth = width;
        description.dwHeight = height;
        description.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
        check_result(direct_draw.pointer->CreateSurface(&description, &back_buffer.pointer, nullptr),
                     "Create render target");

        if (direct_3d.pointer == nullptr)
        {
            check_result(direct_draw.pointer->QueryInterface(IID_IDirect3D7,
                                                             reinterpret_cast<void**>(&direct_3d.pointer)),
                         "Query Direct3D7");
        }

        description = {};
        description.dwSize = sizeof(description);
        description.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
        description.dwWidth = width;
        description.dwHeight = height;
        description.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
        description.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
        description.ddpfPixelFormat.dwFlags = DDPF_ZBUFFER;
        description.ddpfPixelFormat.dwZBufferBitDepth = 16;
        check_result(direct_draw.pointer->CreateSurface(&description, &depth_buffer.pointer, nullptr),
                     "Create Z buffer");
        check_result(back_buffer.pointer->AddAttachedSurface(depth_buffer.pointer), "Attach Z buffer");
        check_result(
            direct_3d.pointer->CreateDevice(IID_IDirect3DHALDevice, back_buffer.pointer, &device.pointer),
            "Create Direct3D7 HAL");

        D3DVIEWPORT7 viewport{0, 0, width, height, 0, 1};
        check_result(device.pointer->SetViewport(&viewport), "SetViewport");
        apply_device_state();

        render_width = width;
        render_height = height;

        if (log_ready)
        {
            write_log(
                "Direct3D7 HAL device ready; diagnostic presentation, retail lighting/lightmaps pending.");
        }
    }

    // Recreates the render target when the client area changes (maximize, fullscreen, resize).
    void resize(std::uint32_t width, std::uint32_t height)
    {
        if (width == 0U || height == 0U || direct_draw.pointer == nullptr)
        {
            return;
        }
        if (device.pointer != nullptr && width == render_width && height == render_height)
        {
            return;
        }

        release_render_target();
        create_render_target(width, height, false);
        first_frame = true;
        write_log("Render target resized: " + std::to_string(width) + "x" + std::to_string(height));
        write_log("Present target: client=" + std::to_string(width) + "x" + std::to_string(height));
    }

    void initialize(HWND window_handle, bool windowed_mode)
    {
        window = window_handle;
        windowed = windowed_mode;
        check_result(DirectDrawCreateEx(
                         nullptr, reinterpret_cast<void**>(&direct_draw.pointer), IID_IDirectDraw7, nullptr),
                     "DirectDrawCreateEx");
        if (windowed)
        {
            check_result(direct_draw.pointer->SetCooperativeLevel(window, DDSCL_NORMAL),
                         "SetCooperativeLevel");
            write_log("Display mode: windowed (DDSCL_NORMAL).");
        }
        else
        {
            // Exclusive fullscreen is only valid together with a successful display-mode change.
            // Taking DDSCL_EXCLUSIVE while SetDisplayMode fails presents a black screen under
            // Wine/CrossOver, so that combination is rejected and we fall back to a borderless
            // WS_POPUP window at desktop size with DDSCL_NORMAL.
            const HRESULT exclusive_result =
                direct_draw.pointer->SetCooperativeLevel(window, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
            if (SUCCEEDED(exclusive_result) &&
                SUCCEEDED(direct_draw.pointer->SetDisplayMode(1024, 768, 32, 0, 0)))
            {
                exclusive = true;
                write_log("Display mode: exclusive fullscreen 1024x768x32.");
            }
            else
            {
                if (SUCCEEDED(exclusive_result))
                {
                    direct_draw.pointer->SetCooperativeLevel(window, DDSCL_NORMAL);
                }
                exclusive = false;
                write_log("Display mode: borderless fullscreen (exclusive unavailable).",
                          diagnostics::LogLevel::warning);
            }
        }
        DDSURFACEDESC2 description{};
        description.dwSize = sizeof(description);
        description.dwFlags = DDSD_CAPS;
        description.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
        check_result(direct_draw.pointer->CreateSurface(&description, &primary.pointer, nullptr),
                     "Create primary");
        if (!exclusive)
        {
            check_result(direct_draw.pointer->CreateClipper(0, &clipper.pointer, nullptr), "CreateClipper");
            check_result(clipper.pointer->SetHWnd(0, window), "Clipper SetHWnd");
            check_result(primary.pointer->SetClipper(clipper.pointer), "SetClipper");
        }
        RECT client_rect{};
        GetClientRect(window, &client_rect);
        std::uint32_t initial_width = static_cast<std::uint32_t>(client_rect.right - client_rect.left);
        std::uint32_t initial_height = static_cast<std::uint32_t>(client_rect.bottom - client_rect.top);
        if (initial_width == 0U || initial_height == 0U)
        {
            initial_width = static_cast<std::uint32_t>(GetSystemMetrics(SM_CXSCREEN));
            initial_height = static_cast<std::uint32_t>(GetSystemMetrics(SM_CYSCREEN));
        }
        create_render_target(initial_width, initial_height, true);
        upload_terrain_textures();
        write_log("Direct3D7 HAL device ready; diagnostic presentation, retail lighting/lightmaps pending.");
    }

    // HOST HELPER: upload terrain texture layers to DirectDraw surfaces.
    // Artifact: igi.exe (retail 1.00); SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
    // Tools/evidence: scripts/GameNatives/terrain/004C5F00-DynObj_ParseConfigFile_7.cpp.
    // Behavior/CFG: Creates 32-bit ARGB DirectDraw texture surfaces in system memory for each TERRAIN.TEX layer.
    // Status/limits: Host DirectDraw texture surface creator for Level terrain layers.
    void upload_terrain_textures()
    {
        terrain_texture_surfaces.clear();
        terrain_textures.clear();
        for (const auto& layer : terrain_data.texture_layers)
        {
            if (layer.width == 0U || layer.height == 0U || layer.argb_pixels.empty())
            {
                terrain_textures.push_back(nullptr);
                continue;
            }
            auto surface = std::make_unique<ComOwner<IDirectDrawSurface7>>();
            DDSURFACEDESC2 description{};
            description.dwSize = sizeof(description);
            description.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
            description.dwWidth = layer.width;
            description.dwHeight = layer.height;
            description.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
            description.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
            description.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
            description.ddpfPixelFormat.dwRGBBitCount = 32;
            description.ddpfPixelFormat.dwRBitMask = 0x00FF0000U;
            description.ddpfPixelFormat.dwGBitMask = 0x0000FF00U;
            description.ddpfPixelFormat.dwBBitMask = 0x000000FFU;
            description.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000U;
            check_result(direct_draw.pointer->CreateSurface(&description, &surface->pointer, nullptr),
                         "Create terrain texture");
            DDSURFACEDESC2 locked{};
            locked.dwSize = sizeof(locked);
            check_result(surface->pointer->Lock(nullptr, &locked, DDLOCK_WAIT | DDLOCK_WRITEONLY, nullptr),
                         "Lock terrain texture");
            for (std::size_t row = 0; row < layer.height; ++row)
            {
                std::memcpy(static_cast<std::byte*>(locked.lpSurface) +
                                row * static_cast<std::size_t>(locked.lPitch),
                            layer.argb_pixels.data() + row * layer.width,
                            layer.width * 4U);
            }
            check_result(surface->pointer->Unlock(nullptr), "Unlock terrain texture");
            terrain_textures.push_back(surface->pointer);
            terrain_texture_surfaces.push_back(std::move(surface));
        }
        if (!terrain_textures.empty())
        {
            write_log("Direct3D7 terrain textures uploaded: count=" + std::to_string(terrain_textures.size()));
        }
    }

    IDirectDrawSurface7* texture_for(const PlacedModel& object, std::uint16_t material)
    {
        if (material == 0xFFFFU)
        {
            return nullptr;
        }
        const auto reference = object.instance->material_reference_indices.at(material);
        const auto texture_index =
            object.package->metadata.material_references.at(reference).first_native_index;
        Package* owner = object.package;
        std::uint32_t index = texture_index;
        if (index >= 0x2000U)
        {
            owner = packages.front().get();
            index -= 0x2000U;
        }
        const auto name = owner->metadata.texture_names.at(index);
        const auto key = std::to_string(reinterpret_cast<std::uintptr_t>(owner)) + ":" + name;
        if (const auto existing = uploaded_textures.find(key); existing != uploaded_textures.end())
        {
            return existing->second;
        }
        const auto* resource = owner->textures.find("LOCAL:textures/" + name + ".tex");
        if (resource == nullptr)
        {
            throw std::runtime_error("Texture resource missing: " + name);
        }
        const auto pixels = texture::Texture_DecodeModelPixels(resource->body);
        auto surface = std::make_unique<ComOwner<IDirectDrawSurface7>>();
        DDSURFACEDESC2 description{};
        description.dwSize = sizeof(description);
        description.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
        description.dwWidth = pixels.width;
        description.dwHeight = pixels.height;
        description.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
        description.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
        description.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
        description.ddpfPixelFormat.dwRGBBitCount = 32;
        description.ddpfPixelFormat.dwRBitMask = 0x00FF0000U;
        description.ddpfPixelFormat.dwGBitMask = 0x0000FF00U;
        description.ddpfPixelFormat.dwBBitMask = 0x000000FFU;
        description.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000U;
        check_result(direct_draw.pointer->CreateSurface(&description, &surface->pointer, nullptr),
                     "Create texture");
        DDSURFACEDESC2 locked{};
        locked.dwSize = sizeof(locked);
        check_result(surface->pointer->Lock(nullptr, &locked, DDLOCK_WAIT | DDLOCK_WRITEONLY, nullptr),
                     "Lock texture");
        for (std::size_t row = 0; row < pixels.height; ++row)
        {
            std::memcpy(static_cast<std::byte*>(locked.lpSurface) +
                            row * static_cast<std::size_t>(locked.lPitch),
                        pixels.argb_pixels.data() + row * pixels.width,
                        pixels.width * 4U);
        }
        check_result(surface->pointer->Unlock(nullptr), "Unlock texture");
        auto* pointer = surface->pointer;
        uploaded_textures.emplace(key, pointer);
        texture_surfaces.push_back(std::move(surface));
        write_log("Texture uploaded: " + name + " size=" + std::to_string(pixels.width) + "x" +
                  std::to_string(pixels.height));
        return pointer;
    }

    void capture(const std::filesystem::path& output_path)
    {
        DDSURFACEDESC2 description{};
        description.dwSize = sizeof(description);
        check_result(back_buffer.pointer->Lock(nullptr, &description, DDLOCK_WAIT | DDLOCK_READONLY, nullptr),
                     "Lock frame capture");
        const std::size_t pixel_count = static_cast<std::size_t>(description.dwWidth) * description.dwHeight;
        std::vector<std::uint32_t> pixels(pixel_count);
        for (std::size_t row = 0; row < description.dwHeight; ++row)
        {
            const auto* source = static_cast<const std::byte*>(description.lpSurface) +
                                 row * static_cast<std::size_t>(description.lPitch);
            for (std::size_t column = 0; column < description.dwWidth; ++column)
            {
                std::uint32_t value = 0;
                const auto byte_count = description.ddpfPixelFormat.dwRGBBitCount / 8U;
                std::memcpy(&value, source + column * byte_count, byte_count);
                const auto expand = [value](DWORD mask) -> std::uint32_t
                {
                    if (mask == 0U)
                    {
                        return 0U;
                    }
                    const int shift = std::countr_zero(static_cast<std::uint32_t>(mask));
                    const std::uint32_t maximum = mask >> shift;
                    return ((value & mask) >> shift) * 255U / maximum;

                };
                pixels[row * description.dwWidth + column] =
                    0xFF000000U | (expand(description.ddpfPixelFormat.dwRBitMask) << 16U) |
                    (expand(description.ddpfPixelFormat.dwGBitMask) << 8U) |
                    expand(description.ddpfPixelFormat.dwBBitMask);
            }
        }
        check_result(back_buffer.pointer->Unlock(nullptr), "Unlock frame capture");
        BITMAPFILEHEADER file_header{};
        BITMAPINFOHEADER image_header{};
        file_header.bfType = 0x4D42;
        file_header.bfOffBits = sizeof(file_header) + sizeof(image_header);
        file_header.bfSize = file_header.bfOffBits + static_cast<DWORD>(pixels.size() * 4U);
        image_header.biSize = sizeof(image_header);
        image_header.biWidth = static_cast<LONG>(description.dwWidth);
        image_header.biHeight = -static_cast<LONG>(description.dwHeight);
        image_header.biPlanes = 1;
        image_header.biBitCount = 32;
        std::ofstream output(output_path, std::ios::binary);
        output.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
        output.write(reinterpret_cast<const char*>(&image_header), sizeof(image_header));
        output.write(reinterpret_cast<const char*>(pixels.data()),
                     static_cast<std::streamsize>(pixels.size() * 4U));
        if (!output)
        {
            throw std::runtime_error("Failed to save Direct3D frame capture.");
        }
        write_log("Direct3D frame saved: " + output_path.string());
    }

    // HOST HELPER: advances the retail input port and the human controller, then
    // places the camera on the human's eye.
    // The scene the phase 1 handler drives, and the frame time it uses.
    static Scene* frame_scene;
    static double frame_delta;

    // The phase 1 handler the retail's handler table holds. It runs the frame.
    static void OnGamePerFrame(void*)
    {
        if (frame_scene != nullptr)
        {
            frame_scene->run_frame(frame_delta);
        }
    }

    // The retail per-frame body, driven by the phase 1 handler.
    void run_frame(double delta_seconds)
    {
        if (developer_controls.free_camera_enabled)
            update_dev_camera(static_cast<float>(delta_seconds));
        else
            update_player(delta_seconds);
        draw();
    }

    void update_player(double delta_seconds)
    {
        if (!player_ready)
        {
            return;
        }
        input_port.Run();

        host::PlayerInputFrame frame{};
        frame.move_forward = input_port.ActionDown(input::Action::move_up);
        frame.move_backward = input_port.ActionDown(input::Action::move_down);
        frame.strafe_left = input_port.ActionDown(input::Action::move_left);
        frame.strafe_right = input_port.ActionDown(input::Action::move_right);
        frame.jump = input_port.ActionDown(input::Action::jump);
        frame.crouch = input_port.ActionDown(input::Action::crouch);
        frame.walk_toggle = input_port.ActionDown(input::Action::toggle_walk_run);
        // The look delta is the mouse counts the device collected since the
        // previous frame, already scaled by the retail mouse sensitivity.
        const auto look = input_port.TakeMouseDelta();
        frame.look_delta_x = look[0];
        frame.look_delta_y = look[1];
        last_look_delta = {look[0], look[1]};

        player.update_view(frame);
        const auto before_motion = player;
        try
        {
            if (player_motion_available)
            {
                frame.look_delta_x = frame.look_delta_y = 0.0F;
                player.update(frame, delta_seconds, collision_grid);
            }
        }
        catch (const std::exception& exception)
        {
            // The retail ground displacement comes from the animation
            // root-motion, which the port does not play yet. The controller
            // reports it, so the scene keeps the human where it is instead of
            // ending the frame.
            write_log(std::string("Player update: ") + exception.what(),
                      diagnostics::LogLevel::warning);
            player = before_motion;
            player_motion_available = false;
        }

        // The camera sits on the human's eye and looks along the human's
        // facing direction, which the controller keeps as yaw and pitch.
        const auto eye = player.eye_position();
        const double yaw = player.yaw();
        const double pitch = player.pitch();
        const double scale = 1.0 / host::PlayerController::units_per_metre;
        camera_position = Vector3{static_cast<float>((eye[0] - origin[0]) * scale),
                                 static_cast<float>((eye[1] - origin[1]) * scale),
                                 static_cast<float>((eye[2] - origin[2]) * scale)};
        const float cos_pitch = static_cast<float>(std::cos(pitch));
        const Vector3 direction{static_cast<float>(std::sin(yaw) * cos_pitch),
                                static_cast<float>(std::cos(yaw) * cos_pitch),
                                static_cast<float>(std::sin(pitch))};
        camera_target = Vector3{camera_position.x + direction.x,
                                camera_position.y + direction.y,
                                camera_position.z + direction.z};
    }

    // HOST HELPER: posts the retail input sequence to the scene window, so the
    // input port, the human controller and the camera can be exercised without
    // a physical keyboard. The messages are the same ones a real device sends.
    //
    // The sequence is driven a frame at a time: a key held for a single frame
    // is released again before the controller samples it, so the press edge
    // would never be seen.
    void build_input_demo()
    {
        std::vector<DemoEvent> events;
        unsigned int frame = 2;
        const auto schedule = [&](input::Action action, unsigned int held_frames)
        {
            const auto& bindings = input_port.Bindings(action);
            const auto found = std::find_if(bindings.begin(), bindings.end(), [](const auto& binding) {
                return binding.device == input::Device::keyboard || binding.code < 3U;
            });
            if (found == bindings.end()) throw std::runtime_error("Input demo action has no supported configured binding.");
            UINT down = WM_KEYDOWN, up = WM_KEYUP;
            WPARAM code = found->code;
            if (found->device == input::Device::mouse)
            {
                const UINT downs[]{WM_LBUTTONDOWN, WM_RBUTTONDOWN, WM_MBUTTONDOWN};
                const UINT ups[]{WM_LBUTTONUP, WM_RBUTTONUP, WM_MBUTTONUP};
                down = downs[found->code]; up = ups[found->code]; code = 0;
            }
            events.push_back({frame++, down, code, 0});
            events.push_back({frame + held_frames - 1, up, code, 0});
            frame += held_frames;
        };
        schedule(input::Action::move_up, 10);
        for (int index = 0; index < 10; ++index)
            events.push_back({frame++, WM_MOUSEMOVE, 0,
                              MAKELPARAM(400 + index * 12, 300 + index * 4)});
        schedule(input::Action::jump, 4);
        schedule(input::Action::crouch, 4);
        schedule(input::Action::toggle_walk_run, 4);
        input_demo_events = std::move(events);
        input_demo_frame = 0;
        input_demo_next = 0;
        write_log("Input demo: scheduled configured movement, jump, crouch and walk-toggle bindings with mouse moves.");
    }

    // HOST HELPER: posts the scheduled demo messages for this frame.
    void advance_input_demo()
    {
        if (input_demo_events.empty())
        {
            return;
        }
        while (input_demo_next < input_demo_events.size() &&
               input_demo_events[input_demo_next].frame <= input_demo_frame)
        {
            const DemoEvent& event = input_demo_events[input_demo_next];
            PostMessageW(window, event.message, event.wparam, event.lparam);
            ++input_demo_next;
        }
        ++input_demo_frame;
    }

    // HOST HELPER: reports the human state so the runtime behaviour is visible
    // in the scene log.
    void log_player_state()
    {
        if (!player_ready)
        {
            return;
        }
        const auto feet = player.feet_position();
        std::ostringstream state;
        state << std::fixed << std::setprecision(3)
              << "HumanPlayer feet=" << feet[0] << '/' << feet[1] << '/' << feet[2]
              << " eye=" << camera_position.x << '/' << camera_position.y << '/'
              << camera_position.z
              << " yaw=" << player.yaw() << " pitch=" << player.pitch()
              << " on_ground=" << (player.on_ground() ? 1 : 0)
              << " crouched=" << (player.crouched() ? 1 : 0)
              << " sliding=" << (player.sliding() ? 1 : 0)
              << " ground=" << player.ground_height()
              << " key_msgs=" << key_messages
              << " mouse_msgs=" << mouse_messages
              << " look_dx=" << last_look_delta[0]
              << " look_dy=" << last_look_delta[1]
              << " mouse_xy=" << last_mouse_x << '/' << last_mouse_y;
        write_log(state.str());
    }

    void draw()
    {
        const Vector3 forward = normalize(subtract(camera_target, camera_position));
        // Right-handed world (X east, Y north, Z up). The view basis must be right = forward x up,
        // up = right x forward; the previous right = up x forward produced (-1,0,0) for a camera
        // looking north, which mirrored the whole scene left-to-right.
        const Vector3 right = normalize(cross(forward, {0, 0, 1}));
        const Vector3 up = cross(right, forward);
        D3DMATRIX view = identity();
        view._11 = right.x;
        view._21 = right.y;
        view._31 = right.z;
        view._12 = up.x;
        view._22 = up.y;
        view._32 = up.z;
        view._13 = forward.x;
        view._23 = forward.y;
        view._33 = forward.z;
        view._41 = -dot(right, camera_position);
        view._42 = -dot(up, camera_position);
        view._43 = -dot(forward, camera_position);

        // Build the active transform context through the retail default-view path instead of
        // using a fixed 4:3 projection.  Human_TaskViewReset supplies the normal player's
        // half-FOV tangent; Transcontext_BuildViewBlock derives the matching vertical tangent
        // from the actual render dimensions and screen block.
        std::array<std::uint8_t, 0x1F0> human_view_state{};
        human::Human_TaskViewReset(human_view_state.data());
        float default_fov_x = 0.0F;
        std::memcpy(&default_fov_x,
                    human_view_state.data() + human::human_view_tangent_x_offset,
                    sizeof(default_fov_x));
        render::Display_HostSetDimensions(static_cast<std::int32_t>(render_width),
                                          static_cast<std::int32_t>(render_height));
        render::ScreenBlock_HostInstallRect(0.0F, 0.0F,
                                            static_cast<float>(render_width),
                                            static_cast<float>(render_height));
        render::TransContext context{};
        const std::array<double, 3> camera_context_position{
            static_cast<double>(camera_position.x),
            static_cast<double>(camera_position.y),
            static_cast<double>(camera_position.z)};
        // Retail consumes nine contiguous floats plus metadata, not a D3DMATRIX
        // whose rows have four floats. Native camera axes are right/forward/up.
        const std::array<float, 10> retail_camera_basis{
            right.x, forward.x, up.x, right.y, forward.y, up.y,
            right.z, forward.z, up.z, 0.0F};
        render::Transcontext_BuildViewBlock(context.bytes.data(), retail_camera_basis.data(),
                                             camera_context_position.data(), default_fov_x);
        render::Transcontext_SetActiveTransContext(context);
        const auto fov_tangents = render::Transcontext_GetFovTangents();
        if (!(fov_tangents[0] > 0.0F) || !(fov_tangents[1] > 0.0F))
        {
            throw std::runtime_error("Retail transform context produced an invalid FOV tangent.");
        }
        D3DMATRIX projection{};
        projection._11 = 1.0F / fov_tangents[0];
        projection._22 = 1.0F / fov_tangents[1];
        projection._33 = 10000.0F / 9999.8F;
        projection._34 = 1;
        projection._43 = -0.2F * projection._33;
        check_result(device.pointer->SetTransform(D3DTRANSFORMSTATE_VIEW, &view), "Set view");
        check_result(device.pointer->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &projection),
                     "Set projection");
        check_result(device.pointer->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF607080U, 1, 0),
                     "Clear frame");
        check_result(device.pointer->BeginScene(), "BeginScene");
        std::size_t terrain_batches_drawn = 0;
        std::size_t drawn = 0;
        try
        {
            if (terrain_data.loaded && !terrain_textures.empty())
            {
                const float cam_p[3] = {camera_position.x, camera_position.y, camera_position.z};
                const float cam_t[3] = {camera_target.x, camera_target.y, camera_target.z};
                const auto terrain_mesh = terrain_data.Host_BuildRenderMesh(
                    origin, world_scale, cam_p, cam_t, fov_tangents[0], fov_tangents[1], 4000.0f);

                // The ground query the human stands on is indexed from the
                // terrain that is actually drawn this frame, so the human
                // collides with the surface the player can see.
                if (!terrain_mesh.vertices.empty())
                {
                    collision_grid.rebuild(terrain_mesh, camera_position.x, camera_position.y);
                }

                if (!terrain_mesh.vertices.empty())
                {
                    D3DMATRIX terrain_world = identity();
                    check_result(device.pointer->SetTransform(D3DTRANSFORMSTATE_WORLD, &terrain_world),
                                 "Set terrain world transform");

                    check_result(device.pointer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE),
                                 "Disable alpha blend for terrain");
                    check_result(device.pointer->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE),
                                 "Terrain cull mode");
                    check_result(device.pointer->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE),
                                 "Terrain Z enable");
                    check_result(device.pointer->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE),
                                 "Terrain Z write");
                    check_result(device.pointer->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1),
                                 "Terrain COLOROP");
                    check_result(device.pointer->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE),
                                 "Terrain COLORARG1");
                    check_result(device.pointer->SetTextureStageState(0, D3DTSS_ADDRESS, D3DTADDRESS_WRAP),
                                 "Terrain texture wrap");
                    check_result(device.pointer->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP),
                                 "Terrain texture wrap U");
                    check_result(device.pointer->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP),
                                 "Terrain texture wrap V");

                    struct TerrainD3DVertex
                    {
                        float position[3];
                        DWORD color;
                        float coordinates[2];
                    };

                    for (const auto& batch : terrain_mesh.batches)
                    {
                        if (batch.indices.empty())
                        {
                            continue;
                        }
                        IDirectDrawSurface7* texture = nullptr;
                        if (batch.texture_index < terrain_textures.size())
                        {
                            texture = terrain_textures[batch.texture_index];
                        }
                        check_result(device.pointer->SetTexture(0, texture), "Set terrain texture");

                        std::vector<TerrainD3DVertex> batch_vertices;
                        std::vector<WORD> batch_indices;
                        std::map<std::uint32_t, WORD> index_map;
                        batch_vertices.reserve(std::min<std::size_t>(batch.indices.size(), 4096U));
                        batch_indices.reserve(batch.indices.size());

                        for (std::uint32_t global_idx : batch.indices)
                        {
                            if (global_idx >= terrain_mesh.vertices.size())
                            {
                                continue;
                            }
                            auto it = index_map.find(global_idx);
                            if (it == index_map.end())
                            {
                                if (batch_vertices.size() >= 65530U)
                                {
                                    check_result(
                                        device.pointer->DrawIndexedPrimitive(
                                            D3DPT_TRIANGLELIST,
                                            D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
                                            batch_vertices.data(),
                                            static_cast<DWORD>(batch_vertices.size()),
                                            batch_indices.data(),
                                            static_cast<DWORD>(batch_indices.size()),
                                            0),
                                        "Draw terrain sub-batch");
                                    batch_vertices.clear();
                                    batch_indices.clear();
                                    index_map.clear();
                                    ++terrain_batches_drawn;
                                }
                                const WORD local_idx = static_cast<WORD>(batch_vertices.size());
                                index_map[global_idx] = local_idx;
                                const auto& v = terrain_mesh.vertices[global_idx];
                                const auto a = static_cast<std::uint8_t>(std::clamp(v.alpha * 255.0f, 0.0f, 255.0f));
                                const DWORD color = (static_cast<DWORD>(a) << 24U) | 0x00FFFFFFU;
                                batch_vertices.push_back({{v.x, v.y, v.z}, color, {v.u, v.v}});
                                batch_indices.push_back(local_idx);
                            }
                            else
                            {
                                batch_indices.push_back(it->second);
                            }
                        }

                        if (!batch_indices.empty())
                        {
                            check_result(
                                device.pointer->DrawIndexedPrimitive(
                                    D3DPT_TRIANGLELIST,
                                    D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
                                    batch_vertices.data(),
                                    static_cast<DWORD>(batch_vertices.size()),
                                    batch_indices.data(),
                                    static_cast<DWORD>(batch_indices.size()),
                                    0),
                                "Draw terrain batch");
                            ++terrain_batches_drawn;
                        }
                    }
                }
            }

            for (const auto& object : objects)
            {
                D3DMATRIX world = identity();
                auto* values = reinterpret_cast<float*>(&world);
                for (std::size_t row = 0; row < 3U; ++row)
                    for (std::size_t column = 0; column < 3U; ++column)
                    {
                        values[row * 4U + column] = object.rotation[column * 3U + row] * world_scale;
                    }
                world._41 = static_cast<float>(object.position[0] - origin[0]) * world_scale;
                world._42 = static_cast<float>(object.position[1] - origin[1]) * world_scale;
                world._43 = static_cast<float>(object.position[2] - origin[2]) * world_scale;
                check_result(device.pointer->SetTransform(D3DTRANSFORMSTATE_WORLD, &world),
                             "Set object transform");

                struct Vertex
                {
                    float position[3];
                    float normal[3];
                    float coordinates[2];
                };

                std::vector<Vertex> vertices;
                vertices.reserve(object.geometry->vertices.size());
                for (const auto& source : object.geometry->vertices)
                {
                    vertices.push_back({{source.position[0], source.position[1], source.position[2]},
                                        {source.normal[0], source.normal[1], source.normal[2]},
                                        {source.texture_coordinates[0], source.texture_coordinates[1]}});
                }
                for (const auto& batch : object.geometry->batches)
                {
                    auto* texture = texture_for(object, batch.material_index);
                    check_result(device.pointer->SetTexture(0, texture), "Set object texture");
                    check_result(device.pointer->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,
                                                                batch.translucent ? TRUE : FALSE),
                                 "Set alpha blend");
                    check_result(device.pointer->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA),
                                 "Set source blend");
                    check_result(
                        device.pointer->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA),
                        "Set destination blend");
                    check_result(device.pointer->DrawIndexedPrimitive(
                                     static_cast<D3DPRIMITIVETYPE>(object.geometry->primitive_type),
                                     D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,
                                     vertices.data() + batch.first_vertex,
                                     batch.vertex_count,
                                     const_cast<WORD*>(batch.indices.data()),
                                     static_cast<DWORD>(batch.indices.size()),
                                     0),
                                 "Draw model batch");
                    ++drawn;
                }
            }
        }
        catch (...)
        {
            device.pointer->EndScene();
            throw;
        }
        check_result(device.pointer->EndScene(), "EndScene");
        RECT destination{};
        GetClientRect(window, &destination);
        POINT top_left{destination.left, destination.top},
            bottom_right{destination.right, destination.bottom};
        ClientToScreen(window, &top_left);
        ClientToScreen(window, &bottom_right);
        destination = {top_left.x, top_left.y, bottom_right.x, bottom_right.y};
        check_result(primary.pointer->Blt(&destination, back_buffer.pointer, nullptr, DDBLT_WAIT, nullptr),
                     "Present frame");
        if (first_frame)
        {
            write_log("First 3D frame presented: terrain_batches=" + std::to_string(terrain_batches_drawn) +
                      " objects=" + std::to_string(objects.size()) +
                      " model_draw_batches=" + std::to_string(drawn) +
                      " uploaded_textures=" + std::to_string(uploaded_textures.size() + terrain_textures.size()));
            first_frame = false;
        }
    }
};

// The scene the window procedure reports the input to.
Scene* g_active_scene = nullptr;

// Phase-0 (Level_Start, 0x00415B30) needs the level root selected by WinMain.
// Retail keeps these paths in the Game/mission context; the port keeps the
// equivalent values beside the active scene so the linked handler can run
// without a synthetic game object.
std::filesystem::path g_level_start_root;
unsigned int g_level_start_level = 1U;

void RunLinkedLevelStart(void*)
{
    if (g_active_scene != nullptr)
    {
        g_active_scene->load(g_level_start_root, g_level_start_level);
    }
}

// Scoped Windows adapter cleanup, including failures before the event loop starts.
struct SceneWindowLifetime
{
    Scene& scene;
    HWND owned_window = nullptr;
    ~SceneWindowLifetime()
    {
        scene.set_mouse_active(false);
        if (scene.raw_mouse_enabled)
        {
            const RAWINPUTDEVICE mouse{0x01, 0x02, RIDEV_REMOVE, nullptr};
            RegisterRawInputDevices(&mouse, 1, sizeof(mouse));
        }
        if (g_active_scene == &scene) g_active_scene = nullptr;
        if (owned_window != nullptr && IsWindow(owned_window)) DestroyWindow(owned_window);
    }
};

Scene* Scene::frame_scene = nullptr;
double Scene::frame_delta = 1.0 / 30.0;

// The retail's window procedure at 0x0048F970 walks its handler chain at
// 0x005C8DF0 before the retail's own message handling. The port's input
// handling is one chain entry, so the messages reach it the way they reach
// the retail's own handlers.
std::intptr_t scene_message_handler(void* window, std::uint32_t message, std::uintptr_t parameter,
                                    std::intptr_t detail)
{
    HWND const native_window = static_cast<HWND>(native_window);
    if (message == WM_ACTIVATEAPP)
        core::FP_InitExtendedPrecision();
    // The input messages feed the retail input port, which the scene's player
    // update reads at the start of the frame.
    if (g_active_scene != nullptr)
    {
        switch (message)
        {
        case WM_SETFOCUS:
            g_active_scene->set_mouse_active(true);
            break;
        case WM_KILLFOCUS:
            g_active_scene->set_mouse_active(false);
            break;
        case WM_MOVE:
        case WM_SIZE:
            if (GetForegroundWindow() == native_window) g_active_scene->set_mouse_active(true);
            break;
        case WM_SETCURSOR:
            if (g_active_scene->raw_mouse_enabled && LOWORD(detail) == HTCLIENT &&
                GetForegroundWindow() == native_window)
            {
                SetCursor(nullptr);
                return 1;
            }
            break;
        case WM_INPUT:
        {
            RAWINPUT input{};
            UINT size = sizeof(input);
            if (g_active_scene->raw_mouse_enabled && GetForegroundWindow() == native_window &&
                GetRawInputData(reinterpret_cast<HRAWINPUT>(detail), RID_INPUT, &input, &size,
                                sizeof(RAWINPUTHEADER)) != static_cast<UINT>(-1) &&
                input.header.dwType == RIM_TYPEMOUSE &&
                (input.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == 0)
            {
                g_active_scene->input_port.AddMouseDelta(input.data.mouse.lLastX, input.data.mouse.lLastY);
                ++g_active_scene->mouse_messages;
            }
            return 1;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            const bool down = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
            auto& scene = *g_active_scene;
            if (scene.developer_controls.enabled &&
                (parameter == VK_F9 || parameter == VK_F10 || parameter == VK_F12))
            {
                if (down)
                {
                    const auto action = scene.developer_controls.handle_key(
                        static_cast<unsigned int>(parameter), (detail & (LPARAM{1} << 30)) != 0);
                    if (action == igi::developer::DeveloperAction::toggle_free_camera)
                    {
                        if (scene.developer_controls.free_camera_enabled)
                        {
                            scene.dev_saved_position = scene.camera_position;
                            scene.dev_saved_target = scene.camera_target;
                            scene.dev_camera_reset();
                        }
                        else
                        {
                            scene.camera_position = scene.dev_saved_position;
                            scene.camera_target = scene.dev_saved_target;
                        }
                        static_cast<void>(scene.input_port.TakeMouseDelta());
                        scene.cursor_valid = false;
                        write_log(std::string("Developer free camera: ") +
                                  (scene.developer_controls.free_camera_enabled ? "enabled" : "disabled"));
                    }
                    else if (action == igi::developer::DeveloperAction::next_level)
                        scene.dev_next_level_requested = true;
                    else if (action == igi::developer::DeveloperAction::toggle_player_cheats)
                        write_log(std::string("Developer F9 cheat request: ") +
                                  (scene.developer_controls.player_god_mode_requested ? "enabled" : "disabled") +
                                  "; health/ammunition native integration pending.");
                }
                return 0;
            }
            // WM_KEY messages collapse sided modifiers; config.qvm names them
            // separately. Translate the Windows message, without assigning actions.
            std::uint32_t key = static_cast<std::uint32_t>(parameter);
            if (key == VK_SHIFT)
                key = MapVirtualKeyW(static_cast<UINT>((detail >> 16) & 0xFF), MAPVK_VSC_TO_VK_EX);
            else if (key == VK_CONTROL)
                key = (detail & (LPARAM{1} << 24)) ? VK_RCONTROL : VK_LCONTROL;
            else if (key == VK_MENU)
                key = (detail & (LPARAM{1} << 24)) ? VK_RMENU : VK_LMENU;
            g_active_scene->input_port.SetKeyDown(key, down);
            ++g_active_scene->key_messages;
            return 1;
        }
        case WM_MOUSEMOVE:
        {
            if (g_active_scene->raw_mouse_enabled) return 0;
            // The mouse position arrives in lParam, so a posted message counts
            // the same way a real one does.
            const auto packed = static_cast<LPARAM>(detail);
            const LONG x = static_cast<LONG>(static_cast<short>(packed & 0xffff));
            const LONG y = static_cast<LONG>(static_cast<short>((packed >> 16) & 0xffff));
            if (g_active_scene->cursor_valid)
            {
                g_active_scene->input_port.AddMouseDelta(
                    static_cast<int>(x - g_active_scene->cursor_x),
                    static_cast<int>(y - g_active_scene->cursor_y));
            }
            g_active_scene->cursor_x = x;
            g_active_scene->cursor_y = y;
            g_active_scene->cursor_valid = true;
            g_active_scene->last_mouse_x = x;
            g_active_scene->last_mouse_y = y;
            ++g_active_scene->mouse_messages;
            return 1;
        }
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        {
            int button = 0;
            if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP)
            {
                button = 1;
            }
            else if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP)
            {
                button = 2;
            }
            const bool down = (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN ||
                               message == WM_MBUTTONDOWN);
            g_active_scene->input_port.SetMouseButtonDown(button, down);
            return 1;
        }
        default:
            break;
        }
    }
    if (message == WM_DESTROY)
    {
        if (g_active_scene != nullptr) g_active_scene->set_mouse_active(false);
        PostQuitMessage(0);
        return 0;
    }
    if (message == WM_ERASEBKGND)
    {
        return 1;
    }
    return 1;
}

LRESULT CALLBACK scene_window(HWND window, UINT message, WPARAM parameter, LPARAM detail)
{
    // The retail's window procedure at 0x0048F970: it walks the handler chain
    // and then applies the retail's own message handling.
    return static_cast<LRESULT>(Retail_WindowProcedure(
        window, static_cast<std::uint32_t>(message), static_cast<std::uintptr_t>(parameter),
        static_cast<std::intptr_t>(detail)));
}
} // namespace

int RunRetailModelScene(HINSTANCE instance,
                        int show_command,
                        const std::filesystem::path& game_directory,
                        unsigned int level_number,
                        const std::filesystem::path& capture_path,
                        bool dev_mode,
                        bool windowed,
                        bool input_demo)
{
    try
    {
        // Retail CRT startup 0x004A7157 establishes precision before WinMain;
        // 0x0048F530 then establishes rounding before the game task loop.
        static_cast<void>(core::Camera_ApplyBandFilter_10());
        core::FP_InitExtendedPrecision();
        Scene scene;
        SceneWindowLifetime lifetime{scene};
        g_active_scene = &scene;
        // Retail does not load the level from WinMain. The Game class registers
        // Level_Start (0x00415B30) as phase 0 during Game_Init (0x00415530);
        // the phase-0 dispatch then runs the level load. The scene load is
        // therefore deferred into the phase-0 hook below.
        scene.developer_controls.enabled = dev_mode;
        if (dev_mode)
        {
            write_log("Developer controls enabled: F9 cheat request (gameplay integration pending), "
                      "F10 next level, F12 inspection camera; movement uses config.qvm bindings.");
        }
        WNDCLASSEXW window_class{};
        window_class.cbSize = sizeof(window_class);
        window_class.lpfnWndProc = scene_window;
        window_class.hInstance = instance;
        window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        window_class.lpszClassName = L"IGI.Retail.ModelScene";
        if (RegisterClassExW(&window_class) == 0)
        {
            throw std::runtime_error("Register model-scene class failed.");
        }
        RECT dimensions{0, 0, 1024, 768};
        DWORD window_style = WS_OVERLAPPEDWINDOW;
        int window_x = CW_USEDEFAULT;
        int window_y = CW_USEDEFAULT;
        int window_width = 0;
        int window_height = 0;
        if (windowed)
        {
            AdjustWindowRect(&dimensions, window_style, FALSE);
            window_width = dimensions.right - dimensions.left;
            window_height = dimensions.bottom - dimensions.top;
        }
        else
        {
            // Fullscreen is the default, matching the retail executable.
            window_style = WS_POPUP | WS_VISIBLE;
            window_x = 0;
            window_y = 0;
            window_width = GetSystemMetrics(SM_CXSCREEN);
            window_height = GetSystemMetrics(SM_CYSCREEN);
        }
        HWND window = CreateWindowExW(0,
                                      window_class.lpszClassName,
                                      L"IGI",
                                      window_style,
                                      window_x,
                                      window_y,
                                      window_width,
                                      window_height,
                                      nullptr,
                                      nullptr,
                                      instance,
                                      nullptr);
        lifetime.owned_window = window;
        if (window == nullptr)
        {
            throw std::runtime_error("Create model-scene window failed.");
        }
        scene.initialize(window, windowed);
        if (!input_demo)
        {
            const RAWINPUTDEVICE mouse{0x01, 0x02, 0, window};
            if (!RegisterRawInputDevices(&mouse, 1, sizeof(mouse)))
                throw std::runtime_error("Register relative mouse input failed.");
            scene.raw_mouse_enabled = true;
            write_log("Relative mouse input registered; config.qvm sensitivity and inversion remain active.");
        }
        ShowWindow(window, show_command);
        UpdateWindow(window);
        scene.set_mouse_active(GetForegroundWindow() == window);

        // Apply the highest retail graphics mode. Retail keeps seven performance levels in the
        // dword table at 0x00537B74 (verified by unicorn execution of 0x004055C0); level 6, the
        // last of the seven that Config_GraphicOptionsNumPerfLevels (0x00405620) reports, carries
        // flags 0x00000000 — no feature masks set, i.e. the highest quality the retail executable
        // offers. The ported config natives apply it through the same record the retail menu
        // writes, so every run starts at the highest available retail graphics mode.
        {
            igi::retail::config::ScriptValueBlock flags_block{};
            igi::retail::config::ScriptValueBlock return_block{};
            igi::retail::config::Config_GraphicOptionsGetPerfFlagsFromLevel(
                &flags_block, static_cast<std::int32_t>(
                                  igi::retail::config::performance_level_count - 1U));
            // Both config natives write the 24-byte script value block to their context
            // argument (rep movsd x6 in retail), so a real scratch buffer is required.
            igi::retail::config::Config_GraphicOptionsSetPerformanceFlags(&return_block, flags_block.integer);
            char graphics_log[128]{};
            std::snprintf(graphics_log, sizeof(graphics_log),
                          "Retail graphics mode: level %u of %u (highest), performance flags 0x%08X, "
                          "render %ux%u.",
                          static_cast<unsigned>(igi::retail::config::performance_level_count - 1U),
                          static_cast<unsigned>(igi::retail::config::performance_level_count),
                          static_cast<unsigned>(flags_block.integer),
                          static_cast<unsigned>(window_width > 0 ? window_width : 640),
                          static_cast<unsigned>(window_height > 0 ? window_height : 480));
            write_log(graphics_log);
        }

        MSG message{};
        bool running = true;
        // Retail's player simulation is 30 Hz. Pace the host presentation at that same
        // cadence instead of adding a fixed Sleep(16) after variable render work.
        constexpr auto frame_period = std::chrono::nanoseconds(1'000'000'000 / 30);
        auto next_frame_deadline = std::chrono::steady_clock::now();
        auto last_frame_time = std::chrono::steady_clock::time_point{};
        // The retail game object the per-frame dispatch reads: the phase word
        // at +0x1C selects the handler in the phase 1 table.
        static igi::retail::core::FrameDispatchRecordView game_object{};
        DAT_00567C8C = &game_object;

        // The retail Game class (0x00415530) links the three phase handlers
        // into the handler table, so the per-frame dispatch at 0x00401050
        // reaches the scene through the table, the way the retail does.
        Scene::frame_scene = &scene;
        // The port's input handling joins the retail's window message chain, so
        // the messages reach it the way they reach the retail's own handlers.
        static igi::retail::core::WindowHandlerEntry scene_handler{};
        scene_handler.next = nullptr;
        scene_handler.handler = &scene_message_handler;
        LinkWindowHandler(&scene_handler);
        // The retail startup sets the loop status byte (0x005C8DE8) to the
        // running value, so the per-frame check does not end the loop.
        Loop_SetStatusByte(1);
        // The app context becomes active with the level: 0x0048F140 writes the
        // isActive flag the main loop reads at 0x0048F170, and the retail only
        // dispatches a frame while the app context is active.
        AppContext_SetIsActive(1);
        // Phase 0 (Level_Start, 0x00415B30) owns the level load. The hook is
        // installed before Game_Init links the handler, so the phase-0
        // dispatch below reaches the scene through the retail handler table.
        g_level_start_root = game_directory;
        g_level_start_level = level_number;
        // Retail main establishes config.qvm before Game_Init registers the
        // phase-0 Level_Start handler. The scene's input port is therefore
        // ready before the first level load and is not re-read per level.
        scene.load_startup_config(game_directory);
        write_log("Flow stage main: config.qvm complete before Level_Start");
        igi::retail::core::SetGameLevelStartHook(&RunLinkedLevelStart);
        igi::retail::core::SetGamePerFrameHook(&Scene::OnGamePerFrame);
        igi::retail::core::SetGameShutdownHook([](void*) {});
        Game_Init();
        // The retail game object carries the class id in the dispatch word at
        // +0x1C, and the per-frame dispatch reads the phase 1 table at that
        // index, so the word is the class the game object belongs to.
        game_object.dispatch_table_index = DAT_0057B1A2;
        // Retail reaches 0x00415B30 through 0x004012A0 after Game_Init. The
        // port invokes the linked phase-0 handler directly; the handler is the
        // same function registered above, and it runs the level load before
        // the first 0x00401050 per-frame dispatch.
        Level_Start(&game_object);
        write_log("Flow stage Level_Start 0x00415B30 complete; "
                  "entering per-frame 0x00401050 -> phase-1 0x00416260");

        unsigned int player_report_frames = 0U;
        // The input demo posts the retail key sequence to the scene window.
        bool input_demo_pending = input_demo;
        auto fps_window_start = next_frame_deadline;
        unsigned int fps_window_frames = 0;
        unsigned int fps_window_overruns = 0;
        while (running)
        {
            // The retail per-frame order (0x0048F80D inside main at
            // 0x0048F530): set the x87 extended precision (0x00498460), read
            // the app context's isActive flag (0x0048F170) and either dispatch
            // the frame (0x00401050) or sleep 500 ms, then check the loop
            // status byte (0x005C8DE8) and pump the messages.
            core::FP_InitExtendedPrecision();
            if (AppContext_GetIsActive() != 0)
            {
                QTask_OpenEvent();
            }
            else
            {
                Sleep(500);
            }
            if (DAT_005C8DE8 == 0U)
            {
                DestroyWindow(window);
            }
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
            {
                if (message.message == WM_QUIT)
                {
                    running = false;
                    break;
                }
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            if (running && scene.dev_next_level_requested)
            {
                scene.dev_next_level_requested = false;
                level_number = level_number == 14U ? 1U : level_number + 1U;
                scene.objects.clear();
                scene.uploaded_textures.clear();
                scene.texture_surfaces.clear();
                scene.packages.clear();
                scene.terrain_textures.clear();
                scene.terrain_texture_surfaces.clear();
                scene.terrain_data = {};
                scene.collision_grid = {};
                scene.input_port = {};
                scene.player_ready = false;
                scene.cursor_valid = false;
                g_level_start_level = level_number;
                Level_Start(&game_object);
                scene.dev_saved_position = scene.camera_position;
                scene.dev_saved_target = scene.camera_target;
                scene.first_frame = true;
                scene.dev_camera_reset();
                write_log("Developer loaded next level: level" + std::to_string(level_number));
                last_frame_time = {};
            }
            if (running && !IsIconic(window))
            {
                std::this_thread::sleep_until(next_frame_deadline);
                const auto frame_start = std::chrono::steady_clock::now();
                const auto previous_frame = last_frame_time;
                last_frame_time = frame_start;
                const double delta_seconds = previous_frame == std::chrono::steady_clock::time_point{}
                                                 ? 1.0 / 30.0
                                                 : std::chrono::duration<double>(frame_start - previous_frame).count();
                Scene::frame_delta = delta_seconds;
                if (input_demo_pending)
                {
                    input_demo_pending = false;
                    scene.build_input_demo();
                }
                scene.advance_input_demo();
                // The frame itself is run by the phase 1 handler, which the
                // per-frame dispatch reaches through the handler table.
                if (player_report_frames++ % 30U == 0U)
                {
                    scene.log_player_state();
                }
                if (!capture_path.empty())
                {
                    scene.capture(capture_path);
                    DestroyWindow(window);
                    running = false;
                }
                const auto frame_end = std::chrono::steady_clock::now();
                next_frame_deadline = frame_start + frame_period;
                if (frame_end > next_frame_deadline)
                {
                    ++fps_window_overruns;
                    next_frame_deadline = frame_end;
                }
                ++fps_window_frames;
                const auto fps_window_duration = frame_end - fps_window_start;
                if (fps_window_duration >= std::chrono::seconds(2))
                {
                    const double observed_fps = static_cast<double>(fps_window_frames) /
                                                std::chrono::duration<double>(fps_window_duration).count();
                    std::ostringstream timing;
                    timing << "Frame pacing: " << std::fixed << std::setprecision(2) << observed_fps
                           << " FPS over " << std::chrono::duration<double>(fps_window_duration).count()
                           << " s; render overruns=" << fps_window_overruns << '/' << fps_window_frames
                           << " (30 FPS target).";
                    write_log(timing.str());
                    fps_window_start = frame_end;
                    fps_window_frames = 0;
                    fps_window_overruns = 0;
                }
            }
        }
        return 0;
    }
    catch (const std::exception& exception)
    {
        write_log(exception.what(), diagnostics::LogLevel::error);
        if (capture_path.empty())
        {
            MessageBoxA(nullptr, exception.what(), "IGI", MB_OK | MB_ICONERROR);
        }
        return 1;
    }
}
