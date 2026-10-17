#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include "retail_model_scene.hpp"
#include "igi/retail/model/mef_render_data.hpp"
#include "igi/retail/model/model_texture_package.hpp"
#include "igi/retail/resource/resource_archive.hpp"
#include "igi/retail/resource/resource_cache.hpp"
#include "igi/retail/script/qvm_loader.hpp"
#include "igi/retail/script/qvm_object_calls.hpp"
#include "igi/retail/texture/model_texture_pixels.hpp"
#include "igi/retail/diagnostics/log_record.hpp"
#include <ddraw.h>
#include <d3d.h>
#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cstring>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

namespace
{
using namespace igi::retail;
template <class Interface> struct ComOwner
{
    Interface *pointer = nullptr;
    ~ComOwner()
    {
        if (pointer != nullptr)
        {
            pointer->Release();
        }
    }
    ComOwner() = default;
    ComOwner(const ComOwner &) = delete;
    ComOwner &operator=(const ComOwner &) = delete;
};
void check_result(HRESULT result, const char *operation)
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
    const auto record = diagnostics::Log_FormatTimestampedRecord(level, std::chrono::system_clock::now(),
                                                                 "model-scene", message);
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
    return {left.y * right.z - left.z * right.y, left.z * right.x - left.x * right.z,
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
float number(const script::QvmObjectValue &value)
{
    if (const auto *real = std::get_if<float>(&value))
    {
        return *real;
    }
    if (const auto *integer = std::get_if<std::int32_t>(&value))
    {
        return static_cast<float>(*integer);
    }
    throw std::runtime_error("Object parameter is not numeric.");
}
std::string text(const script::QvmObjectValue &value)
{
    if (const auto *result = std::get_if<std::string>(&value))
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
    const std::array<float, 9> rotate_alpha{1,           0, 0,          0,           cosine_alpha,
                                            -sine_alpha, 0, sine_alpha, cosine_alpha};
    for (const auto &right : {rotate_beta, rotate_alpha})
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
    const model::MtpInstance *instance = nullptr;
    Package *package = nullptr;
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
    std::map<std::string, IDirectDrawSurface7 *> uploaded_textures;
    std::vector<std::unique_ptr<Package>> packages;
    std::vector<PlacedModel> objects;
    std::array<double, 3> origin{};
    Vector3 camera_position{0, -100, 50};
    Vector3 camera_target{0, 0, 0};
    HWND window = nullptr;
    bool first_frame = true;
    static constexpr float world_scale = 1.0F / 4096.0F;

    void load(const std::filesystem::path &root, unsigned int level_number)
    {
        resource::ResourceCache cache;
        const auto location = root / "missions" / "location0";
        const std::string level_name = "level" + std::to_string(level_number);
        const auto level = location / level_name;
        for (const auto &descriptor :
             {std::pair(location / "common", std::string("location0")), std::pair(level, level_name)})
        {
            auto package = std::make_unique<Package>();
            const auto bytes =
                resource::Resource_Load(cache, descriptor.first / (descriptor.second + ".mtp"));
            package->metadata = model::MtpPackage::DynObj_InsertHashItem_14(bytes->bytes);
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
        for (const auto &call : calls)
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
        for (const auto &call : calls)
        {
            const auto &arguments = call->arguments;
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
                for (const auto &instance : (*package_iterator)->metadata.instances)
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
                        for (const auto &instance : (*package_iterator)->metadata.instances)
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
            const auto &mesh_name = object.package->metadata.model_names.at(object.instance->model_index);
            const auto key =
                std::to_string(reinterpret_cast<std::uintptr_t>(object.package)) + ":" + mesh_name;
            auto &geometry = geometry_cache[key];
            if (!geometry)
            {
                const auto *entry = object.package->meshes.find("LOCAL:models/" + mesh_name + ".mef");
                if (entry == nullptr)
                {
                    throw std::runtime_error("MTP mesh resource missing: " + mesh_name);
                }
                geometry = std::make_shared<model::MefRenderData>(model::Display_DecodeMeshRenderData(
                    model::MefModel::DynObj_LocalModelsSMef(entry->body)));
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
    void initialize(HWND window_handle)
    {
        window = window_handle;
        check_result(DirectDrawCreateEx(nullptr, reinterpret_cast<void **>(&direct_draw.pointer),
                                        IID_IDirectDraw7, nullptr),
                     "DirectDrawCreateEx");
        check_result(direct_draw.pointer->SetCooperativeLevel(window, DDSCL_NORMAL), "SetCooperativeLevel");
        DDSURFACEDESC2 description{};
        description.dwSize = sizeof(description);
        description.dwFlags = DDSD_CAPS;
        description.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
        check_result(direct_draw.pointer->CreateSurface(&description, &primary.pointer, nullptr),
                     "Create primary");
        check_result(direct_draw.pointer->CreateClipper(0, &clipper.pointer, nullptr), "CreateClipper");
        check_result(clipper.pointer->SetHWnd(0, window), "Clipper SetHWnd");
        check_result(primary.pointer->SetClipper(clipper.pointer), "SetClipper");
        description = {};
        description.dwSize = sizeof(description);
        description.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
        description.dwWidth = 1024;
        description.dwHeight = 768;
        description.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
        check_result(direct_draw.pointer->CreateSurface(&description, &back_buffer.pointer, nullptr),
                     "Create render target");
        check_result(direct_draw.pointer->QueryInterface(IID_IDirect3D7,
                                                         reinterpret_cast<void **>(&direct_3d.pointer)),
                     "Query Direct3D7");
        description = {};
        description.dwSize = sizeof(description);
        description.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
        description.dwWidth = 1024;
        description.dwHeight = 768;
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
        D3DVIEWPORT7 viewport{0, 0, 1024, 768, 0, 1};
        check_result(device.pointer->SetViewport(&viewport), "SetViewport");
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
        write_log("Direct3D7 HAL device ready; diagnostic presentation, retail lighting/lightmaps pending.");
    }
    IDirectDrawSurface7 *texture_for(const PlacedModel &object, std::uint16_t material)
    {
        if (material == 0xFFFFU)
        {
            return nullptr;
        }
        const auto reference = object.instance->material_reference_indices.at(material);
        const auto texture_index =
            object.package->metadata.material_references.at(reference).first_native_index;
        Package *owner = object.package;
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
        const auto *resource = owner->textures.find("LOCAL:textures/" + name + ".tex");
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
            std::memcpy(static_cast<std::byte *>(locked.lpSurface) +
                            row * static_cast<std::size_t>(locked.lPitch),
                        pixels.argb_pixels.data() + row * pixels.width, pixels.width * 4U);
        }
        check_result(surface->pointer->Unlock(nullptr), "Unlock texture");
        auto *pointer = surface->pointer;
        uploaded_textures.emplace(key, pointer);
        texture_surfaces.push_back(std::move(surface));
        write_log("Texture uploaded: " + name + " size=" + std::to_string(pixels.width) + "x" +
                  std::to_string(pixels.height));
        return pointer;
    }
    void capture(const std::filesystem::path &output_path)
    {
        DDSURFACEDESC2 description{};
        description.dwSize = sizeof(description);
        check_result(back_buffer.pointer->Lock(nullptr, &description, DDLOCK_WAIT | DDLOCK_READONLY, nullptr),
                     "Lock frame capture");
        const std::size_t pixel_count = static_cast<std::size_t>(description.dwWidth) * description.dwHeight;
        std::vector<std::uint32_t> pixels(pixel_count);
        for (std::size_t row = 0; row < description.dwHeight; ++row)
        {
            const auto *source = static_cast<const std::byte *>(description.lpSurface) +
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
        output.write(reinterpret_cast<const char *>(&file_header), sizeof(file_header));
        output.write(reinterpret_cast<const char *>(&image_header), sizeof(image_header));
        output.write(reinterpret_cast<const char *>(pixels.data()),
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
        const Vector3 right = normalize(cross({0, 0, 1}, forward));
        const Vector3 up = cross(forward, right);
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
            for (const auto &object : objects)
            {
                D3DMATRIX world = identity();
                auto *values = reinterpret_cast<float *>(&world);
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
                for (const auto &source : object.geometry->vertices)
                {
                    vertices.push_back({{source.position[0], source.position[1], source.position[2]},
                                        {source.normal[0], source.normal[1], source.normal[2]},
                                        {source.texture_coordinates[0], source.texture_coordinates[1]}});
                }
                for (const auto &batch : object.geometry->batches)
                {
                    auto *texture = texture_for(object, batch.material_index);
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
                                     vertices.data() + batch.first_vertex, batch.vertex_count,
                                     const_cast<WORD *>(batch.indices.data()),
                                     static_cast<DWORD>(batch.indices.size()), 0),
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
int RunRetailModelScene(HINSTANCE instance, int show_command, const std::filesystem::path &game_directory,
                        unsigned int level_number, const std::filesystem::path &capture_path)
{
    try
    {
        Scene scene;
        scene.load(game_directory, level_number);
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
        AdjustWindowRect(&dimensions, WS_OVERLAPPEDWINDOW, FALSE);
        HWND window =
            CreateWindowExW(0, window_class.lpszClassName, L"IGI", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                            CW_USEDEFAULT, dimensions.right - dimensions.left,
                            dimensions.bottom - dimensions.top, nullptr, nullptr, instance, nullptr);
        if (window == nullptr)
        {
            throw std::runtime_error("Create model-scene window failed.");
        }
        scene.initialize(window);
        ShowWindow(window, show_command);
        UpdateWindow(window);
        MSG message{};
        bool running = true;
        while (running)
        {
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
            if (running && !IsIconic(window))
            {
                scene.draw();
                if (!capture_path.empty())
                {
                    scene.capture(capture_path);
                    DestroyWindow(window);
                    running = false;
                }
            }
            Sleep(16);
        }
        return 0;
    }
    catch (const std::exception &exception)
    {
        write_log(exception.what(), diagnostics::LogLevel::error);
        if (capture_path.empty())
        {
            MessageBoxA(nullptr, exception.what(), "IGI", MB_OK | MB_ICONERROR);
        }
        return 1;
    }
}
