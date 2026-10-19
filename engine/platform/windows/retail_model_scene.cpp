#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include "retail_model_scene.hpp"

#include "igi/retail/diagnostics/log_record.hpp"
#include "igi/retail/config/config_graphic_options.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
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


#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cctype>
#include <cmath>
#include <cstdio>
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

#include "developer_controls.hpp"

namespace
{
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
    std::ofstream output("open-igi-model-scene.log", std::ios::app);
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
                current[row * 3U + column] = left[row * 3U] * right[column] +
                                             left[row * 3U + 1U] * right[3U + column] +
                                             left[row * 3U + 2U] * right[6U + column];
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
    HWND window = nullptr;
    bool first_frame = true;
    static constexpr float world_scale = 1.0F / 4096.0F;

    // Host free camera (enabled by the "devmode" command-line argument).
    bool windowed = true;
    bool exclusive = false;
    bool dev_mode = false;
    igi::developer::DeveloperControls developer_controls;
    float dev_yaw = 0.0F;   // radians, around Z
    float dev_pitch = 0.0F; // radians, up/down
    float dev_speed = 50.0F;
    bool dev_look_initialised = false;
    POINT dev_last_mouse{};

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
        const POINT cursor = [this]
        {
            POINT position{};
            GetCursorPos(&position);
            return position;
        }();

        if (!dev_look_initialised)
        {
            dev_last_mouse = cursor;
            dev_look_initialised = true;
        }
        const float delta_x = static_cast<float>(cursor.x - dev_last_mouse.x);
        const float delta_y = static_cast<float>(cursor.y - dev_last_mouse.y);
        dev_last_mouse = cursor;

        constexpr float look_sensitivity = 0.0035F;
        dev_yaw += delta_x * look_sensitivity;
        dev_pitch = std::clamp(dev_pitch - delta_y * look_sensitivity, -1.5533F, 1.5533F);

        // Must match dev_camera_reset(): yaw is measured from +Y towards +X, so
        // forward = (sin yaw, cos yaw, 0) and right = forward x up = (cos yaw, -sin yaw, 0).
        // The previous -cos / +sin pair made W drive backwards and turned the view the wrong way.
        const float cosine_pitch = std::cos(dev_pitch);
        const Vector3 forward{
            std::sin(dev_yaw) * cosine_pitch, std::cos(dev_yaw) * cosine_pitch, std::sin(dev_pitch)};
        const Vector3 right{std::cos(dev_yaw), -std::sin(dev_yaw), 0.0F};

        Vector3 move{0, 0, 0};
        const bool key_w = (GetAsyncKeyState('W') & 0x8000) != 0;
        const bool key_s = (GetAsyncKeyState('S') & 0x8000) != 0;
        const bool key_a = (GetAsyncKeyState('A') & 0x8000) != 0;
        const bool key_d = (GetAsyncKeyState('D') & 0x8000) != 0;
        const bool key_up = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
        const bool key_down = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

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

        const float speed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 ? dev_speed * 4.0F : dev_speed;
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
                player_origin_found = true;
                break;
            }
        }
        write_log(std::string("HumanPlayer spawn camera=") +
                  (player_origin_found ? "installed" : "fallback"));
        // Load retail level terrain if available (TERRAIN directory)
        const auto terrain_dir_lower = level / "terrain";
        const auto terrain_dir_upper = level / "TERRAIN";
        const auto terrain_dir = std::filesystem::exists(terrain_dir_lower) ? terrain_dir_lower : terrain_dir_upper;
        if (std::filesystem::exists(terrain_dir))
        {
            const bool terrain_loaded = terrain_data.Host_LoadLevelTerrain(terrain_dir, calls);
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
                    // Retail 0x0049B700 palette evaluation is ported (mef_skinner.cpp); the
                    // identity palette renders the authored bind pose until the animation
                    // slice supplies live bone palettes.
                    try
                    {
                        geometry = std::make_shared<model::MefRenderData>(
                            model::Display_DecodeMeshRenderData(*parsed_mef));
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
                        model::Display_DecodeMeshRenderData(*parsed_mef));
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
                                model::Display_DecodeMeshRenderData(*child_mef));
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
                  "; skinned models and remaining task types not yet rendered.");
        write_log("HumanPlayer input is unavailable until the retail GOInRemap and InputPort native chain is transcribed and verified.");
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
        render::Transcontext_BuildViewBlock(context.bytes.data(), &view,
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

LRESULT CALLBACK scene_window(HWND window, UINT message, WPARAM parameter, LPARAM detail)
{
    if (message == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }
    if (message == WM_ERASEBKGND)
    {
        return 1;
    }
    return DefWindowProcW(window, message, parameter, detail);
}
} // namespace

int RunRetailModelScene(HINSTANCE instance,
                        int show_command,
                        const std::filesystem::path& game_directory,
                        unsigned int level_number,
                        const std::filesystem::path& capture_path,
                        bool dev_mode,
                        bool windowed)
{
    try
    {
        Scene scene;
        scene.load(game_directory, level_number);
        scene.dev_mode = dev_mode;
        scene.developer_controls.enabled = dev_mode;
        scene.developer_controls.free_camera_enabled = dev_mode;
        if (dev_mode)
        {
            scene.dev_camera_reset();
            write_log("Free camera enabled (devmode): W/A/S/D move, Space up, Shift down, mouse looks, "
                      "Ctrl boosts. F9 player cheat requests, F10 next level, F12 free-camera toggle.");
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
        if (window == nullptr)
        {
            throw std::runtime_error("Create model-scene window failed.");
        }
        scene.initialize(window, windowed);
        ShowWindow(window, show_command);
        UpdateWindow(window);

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
        auto previous_frame_start = next_frame_deadline;
        auto fps_window_start = next_frame_deadline;
        unsigned int fps_window_frames = 0;
        unsigned int fps_window_overruns = 0;
        float dev_frame_seconds = std::chrono::duration<float>(frame_period).count();
        while (running)
        {
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
            {
                if (message.message == WM_QUIT)
                {
                    running = false;
                    break;
                }
                if (message.hwnd == window &&
                    (message.message == WM_KEYDOWN || message.message == WM_SYSKEYDOWN))
                {
                    const auto action = scene.developer_controls.handle_key(
                        static_cast<unsigned int>(message.wParam),
                        (static_cast<unsigned long long>(message.lParam) & (1ULL << 30U)) != 0U);
                    if (action == igi::developer::DeveloperAction::toggle_free_camera)
                    {
                        scene.dev_look_initialised = false;
                        if (scene.developer_controls.free_camera_enabled)
                        {
                            scene.dev_camera_reset();
                        }
                        write_log(std::string("Developer free camera: ") +
                                  (scene.developer_controls.free_camera_enabled ? "enabled" : "disabled"));
                    }
                    else if (action == igi::developer::DeveloperAction::toggle_player_cheats)
                    {
                        write_log(
                            std::string("Developer player-only god/ammo/clips request: ") +
                            (scene.developer_controls.player_god_mode_requested ? "enabled" : "disabled") +
                            "; gameplay health/ammunition integration unavailable in model scene.");
                    }
                    else if (action == igi::developer::DeveloperAction::next_level)
                    {
                        level_number = level_number == 14U ? 1U : level_number + 1U;
                        write_log("Developer loading next model scene: level" + std::to_string(level_number));
                        scene.objects.clear();
                        scene.packages.clear();
                        scene.uploaded_textures.clear();
                        scene.texture_surfaces.clear();
                        scene.origin = {};
                        scene.camera_position = {0, 0, 0.69F};
                        scene.camera_target = {0, 1, 0.69F};
                        scene.load(game_directory, level_number);
                        scene.first_frame = true;
                        scene.dev_look_initialised = false;
                        scene.dev_camera_reset();
                    }
                    if (action != igi::developer::DeveloperAction::none)
                    {
                        continue;
                    }
                }
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            if (running && !IsIconic(window))
            {
                std::this_thread::sleep_until(next_frame_deadline);
                const auto frame_start = std::chrono::steady_clock::now();
                dev_frame_seconds = std::chrono::duration<float>(frame_start - previous_frame_start).count();
                previous_frame_start = frame_start;
                if (scene.dev_mode && scene.developer_controls.free_camera_enabled)
                {
                    scene.update_dev_camera(dev_frame_seconds);
                    scene.draw();
                }
                else
                {
                    scene.draw();
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
