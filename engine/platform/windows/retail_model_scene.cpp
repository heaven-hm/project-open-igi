#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include "retail_model_scene.hpp"

#include "igi/retail/diagnostics/log_record.hpp"
#include "igi/retail/model/mef_render_data.hpp"
#include "igi/retail/model/model_texture_package.hpp"
#include "igi/retail/render/trans_context.hpp"
#include "igi/retail/resource/resource_archive.hpp"
#include "igi/retail/resource/resource_cache.hpp"
#include "igi/retail/script/qvm_loader.hpp"
#include "igi/retail/script/qvm_object_calls.hpp"
#include "igi/retail/texture/model_texture_pixels.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cmath>
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
    std::array<double, 3> origin{};
    Vector3 camera_position{0, -100, 50};
    Vector3 camera_target{0, 0, 0};
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
    float dev_speed = 400.0F;
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
        resource::ResourceCache cache;
        const auto location = root / "missions" / "location0";
        const std::string level_name = "level" + std::to_string(level_number);
        const auto level = location / level_name;
        for (const auto& descriptor :
             {std::pair(location / "common", std::string("location0")), std::pair(level, level_name)})
        {
            auto package = std::make_unique<Package>();
            const auto bytes =
                resource::Resource_Load(cache, descriptor.first / (descriptor.second + ".mtp"));
            package->metadata = model::MtpPackage::DynObj_LoadModelTexturePackage(bytes->bytes);
            package->meshes =
                resource::Resource_Load(descriptor.first / "models" / (descriptor.second + ".res"));
            package->textures =
                resource::Resource_Load(descriptor.first / "textures" / (descriptor.second + ".res"));
            write_log("MTP/RES loaded: " + descriptor.second +
                      " models=" + std::to_string(package->meshes.entries.size()) +
                      " texture_names=" + std::to_string(package->metadata.texture_names.size()));
            packages.push_back(std::move(package));
        }
        const auto script = script::Qvm_Load(cache, level / "objects.qsc");
        const auto calls = script::Qvm_ReadObjectCalls(script.program());
        for (const auto& call : calls)
        {
            if (call->function_name == "Task_New" && call->arguments.size() > 8U &&
                text(call->arguments[1]) == "HumanPlayer")
            {
                for (std::size_t axis = 0; axis < 3U; ++axis)
                {
                    origin[axis] = number(call->arguments[3U + axis]);
                }
                break;
            }
        }
        std::map<std::string, std::shared_ptr<model::MefRenderData>> geometry_cache;
        std::size_t missing = 0;
        for (const auto& call : calls)
        {
            const auto& arguments = call->arguments;
            if (call->function_name != "Task_New" || arguments.size() < 10U)
            {
                continue;
            }
            const auto type = text(arguments[1]);
            const bool static_model_task = type == "EditRigidObj" || type == "Building";
            const bool initial_model_task = type == "ExplodeObject" || type == "Terminal" ||
                                            type == "SCameraControl" || type == "AlarmControl";
            const bool door_task = type == "Door";
            if (!static_model_task && !initial_model_task && !door_task)
            {
                continue;
            }
            // These parameter locations are declared in OBJECTS.QVM itself. This loads the initial
            // mesh pose only; task behavior, destruction and door interaction remain unported.
            const std::size_t model_parameter = door_task ? 12U : 9U;
            const std::size_t orientation_parameter = door_task ? 9U : 6U;
            if (arguments.size() <= model_parameter)
            {
                throw std::runtime_error("Truncated model task parameters.");
            }
            const std::string model_name = text(arguments[model_parameter]);
            PlacedModel object;
            for (std::size_t axis = 0; axis < 3U; ++axis)
            {
                object.position[axis] = number(arguments[3U + axis]);
            }
            object.rotation = orientation(number(arguments[orientation_parameter]),
                                          number(arguments[orientation_parameter + 1U]),
                                          number(arguments[orientation_parameter + 2U]));
            for (auto package_iterator = packages.rbegin(); package_iterator != packages.rend();
                 ++package_iterator)
            {
                for (const auto& instance : (*package_iterator)->metadata.instances)
                {
                    if (instance.instance_name == model_name)
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
                if (!model_name.empty())
                {
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
            if (!geometry)
            {
                const auto* entry = object.package->meshes.find("LOCAL:models/" + mesh_name + ".mef");
                if (entry == nullptr)
                {
                    throw std::runtime_error("MTP mesh resource missing: " + mesh_name);
                }
                geometry = std::make_shared<model::MefRenderData>(
                    model::Display_DecodeMeshRenderData(model::MefModel::DynObj_LoadModelMesh(entry->body)));
            }
            object.geometry = geometry;
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
        write_log("Loaded placed model poses=" + std::to_string(objects.size()) +
                  "; unresolved_source_models=" + std::to_string(missing) +
                  "; skinned models and remaining task types not yet rendered.");
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
        write_log("Direct3D7 HAL device ready; diagnostic presentation, retail lighting/lightmaps pending.");
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
                    const unsigned int shift = std::countr_zero(static_cast<std::uint32_t>(mask));
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
        D3DMATRIX projection{};
        projection._11 = 1.0F;
        projection._22 = 1.3333333F;
        projection._33 = 10000.0F / 9999.8F;
        projection._34 = 1;
        projection._43 = -0.2F * projection._33;
        check_result(device.pointer->SetTransform(D3DTRANSFORMSTATE_VIEW, &view), "Set view");
        check_result(device.pointer->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &projection),
                     "Set projection");
        check_result(device.pointer->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF607080U, 1, 0),
                     "Clear frame");
        check_result(device.pointer->BeginScene(), "BeginScene");
        std::size_t drawn = 0;
        try
        {
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
            write_log("First 3D frame presented: objects=" + std::to_string(objects.size()) +
                      " draw_batches=" + std::to_string(drawn) +
                      " uploaded_textures=" + std::to_string(uploaded_textures.size()));
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
        MSG message{};
        bool running = true;
        float dev_frame_seconds = 1.0F / 60.0F;
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
                        scene.camera_position = {0, -100, 50};
                        scene.camera_target = {0, 0, 0};
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
                const auto frame_start = std::chrono::steady_clock::now();
                if (scene.dev_mode && scene.developer_controls.free_camera_enabled)
                {
                    scene.update_dev_camera(dev_frame_seconds);
                }
                scene.draw();
                if (!capture_path.empty())
                {
                    scene.capture(capture_path);
                    DestroyWindow(window);
                    running = false;
                }
                const auto frame_end = std::chrono::steady_clock::now();
                dev_frame_seconds = std::chrono::duration<float>(frame_end - frame_start).count();
            }
            Sleep(16);
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
