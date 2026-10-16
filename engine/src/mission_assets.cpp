#include "igi/mission_assets.hpp"

#include "igi/asset_io.hpp"
#include "igi/diagnostics.hpp"
#include "igi/qvm_initializer.hpp"

#include "mef_native.h"
#include "mtp_parser.h"
#include "olm_parser.h"
#include "res_parser.h"
#include "tex_parser.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <limits>
#include <glm/gtc/matrix_transform.hpp>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace igi::assets
{

gameplay::SceneTexture decode_original_texture(const TEXImage& image)
{
    const std::size_t pixel_count = static_cast<std::size_t>(image.width) * image.height;
    const std::size_t bytes_per_pixel = image.mode == 2 ? 2 : 4;
    if (pixel_count == 0 || pixel_count > 16777216 || image.pixels.size() < pixel_count * bytes_per_pixel)
    {
        throw std::runtime_error("Invalid decoded texture dimensions or pixel data");
    }
    gameplay::SceneTexture texture{image.width, image.height, {}, {}};
    texture.pixels.reserve(pixel_count);
    const BinaryView pixels(image.pixels);
    for (std::size_t pixel_index = 0; pixel_index < pixel_count; ++pixel_index)
    {
        if (image.mode == 2)
        {
            const auto packed = pixels.unsigned_short(pixel_index * 2);
            const auto red = ((packed >> 10) & 31) * 255 / 31;
            const auto green = ((packed >> 5) & 31) * 255 / 31;
            const auto blue = (packed & 31) * 255 / 31;
            const std::uint32_t alpha = (packed & 0x8000) != 0 ? 0xff000000u : 0;
            texture.pixels.push_back(alpha | (red << 16) | (green << 8) | blue);
        }
        else if (image.mode == 3 || image.mode == 67)
        {
            texture.pixels.push_back(pixels.unsigned_integer(pixel_index * 4));
        }
        else
        {
            throw std::runtime_error("Unsupported decoded texture mode");
        }
    }
    return texture;
}

namespace
{

void append_generated_mipmaps(gameplay::SceneTexture& texture)
{
    const std::vector<std::uint32_t>* source_pixels =
        texture.mipmaps.empty() ? &texture.pixels : &texture.mipmaps.back().pixels;
    std::uint32_t source_width = texture.mipmaps.empty() ? texture.width : texture.mipmaps.back().width;
    std::uint32_t source_height = texture.mipmaps.empty() ? texture.height : texture.mipmaps.back().height;
    while (source_width > 1 || source_height > 1)
    {
        const std::uint32_t destination_width = std::max(1u, source_width / 2);
        const std::uint32_t destination_height = std::max(1u, source_height / 2);
        gameplay::SceneTextureMip mipmap;
        mipmap.width = destination_width;
        mipmap.height = destination_height;
        mipmap.pixels.resize(static_cast<std::size_t>(destination_width) * destination_height);
        for (std::uint32_t row = 0; row < destination_height; ++row)
        {
            for (std::uint32_t column = 0; column < destination_width; ++column)
            {
                std::array<std::uint32_t, 4> samples{};
                std::size_t sample_index = 0;
                for (std::uint32_t vertical_offset = 0; vertical_offset < 2; ++vertical_offset)
                {
                    for (std::uint32_t horizontal_offset = 0; horizontal_offset < 2; ++horizontal_offset)
                    {
                        const auto source_column = std::min(source_width - 1, column * 2 + horizontal_offset);
                        const auto source_row = std::min(source_height - 1, row * 2 + vertical_offset);
                        samples[sample_index++] = (*source_pixels)[
                            static_cast<std::size_t>(source_row) * source_width + source_column];
                    }
                }
                std::uint32_t pixel = 0;
                std::uint32_t alpha_total = 0;
                for (const auto sample : samples)
                {
                    alpha_total += (sample >> 24) & 255u;
                }
                for (const std::uint32_t channel_shift : {0u, 8u, 16u})
                {
                    std::uint32_t premultiplied_channel_total = 0;
                    for (const auto sample : samples)
                    {
                        const auto alpha = (sample >> 24) & 255u;
                        const auto color = (sample >> channel_shift) & 255u;
                        premultiplied_channel_total += color * alpha;
                    }
                    const std::uint32_t channel = alpha_total == 0
                        ? 0
                        : premultiplied_channel_total / alpha_total;
                    pixel |= channel << channel_shift;
                }
                pixel |= (alpha_total / 4u) << 24;
                mipmap.pixels[static_cast<std::size_t>(row) * destination_width + column] = pixel;
            }
        }
        texture.mipmaps.push_back(std::move(mipmap));
        source_width = destination_width;
        source_height = destination_height;
        source_pixels = &texture.mipmaps.back().pixels;
    }
}

gameplay::SceneTexture decode_texture_with_mipmaps(const std::vector<TEXImage>& images,
                                                    std::size_t base_image_index,
                                                    bool images_are_mipmap_chain)
{
    if (base_image_index >= images.size())
    {
        throw std::runtime_error("Texture mipmap base image index is outside the parsed image list");
    }
    auto texture = decode_original_texture(images[base_image_index]);
    if (images_are_mipmap_chain)
    {
        for (std::size_t mipmap_index = base_image_index + 1; mipmap_index < images.size(); ++mipmap_index)
        {
            auto decoded_mipmap = decode_original_texture(images[mipmap_index]);
            if (decoded_mipmap.width != std::max(1u, texture.mipmaps.empty()
                                                           ? texture.width / 2
                                                           : texture.mipmaps.back().width / 2) ||
                decoded_mipmap.height != std::max(1u, texture.mipmaps.empty()
                                                            ? texture.height / 2
                                                            : texture.mipmaps.back().height / 2))
            {
                throw std::runtime_error("Authored TEX mipmap dimensions do not follow the expected chain");
            }
            texture.mipmaps.push_back({decoded_mipmap.width,
                                       decoded_mipmap.height,
                                       std::move(decoded_mipmap.pixels),
                                       true});
        }
    }
    append_generated_mipmaps(texture);
    return texture;
}

} // namespace

MissionAssets::MissionAssets(const std::filesystem::path& game_directory,
                             const std::filesystem::path& mission_directory,
                             const std::filesystem::path& cache_directory)
{
    textures_.push_back({2, 2, {0xffa5a59bu, 0xff919188u, 0xff919188u, 0xffa5a59bu}, {}});
    const auto magic_bytes = read_file(resolve_path(game_directory, "magicobj/magicobj.qvm"));
    const QvmInitializer magic_script{BinaryView(magic_bytes)};
    for (const auto& call : magic_script.calls())
    {
        if (call.function == "DefineMagicObj" && call.arguments.size() >= 3)
        {
            const auto attachment_name = normalize_name(call.arguments[0].text);
            const auto model_name = normalize_name(call.arguments[1].text);
            if (!attachment_name.empty() && !model_name.empty())
            {
                magic_attachment_models_.try_emplace(attachment_name, model_name);
            }
        }
        if (call.function == "DefineMagicObj" && call.arguments.size() >= 3 &&
            call.arguments[2].text == "TASKTYPE_GUNFLAME")
        {
            muzzle_flash_models_.insert(normalize_name(call.arguments[0].text));
            muzzle_flash_models_.insert(normalize_name(call.arguments[1].text));
        }
        if (call.function == "DefineMagicObj" && call.arguments.size() >= 3 &&
            call.arguments[2].text == "TASKTYPE_DEATHZONE")
        {
            invisible_attachment_models_.insert(normalize_name(call.arguments[0].text));
        }
    }
    const auto common_directory = resolve_path(game_directory, "missions/location0/common");
    for (const auto& base_directory :
         {resolve_path(game_directory, "common"), common_directory, mission_directory})
    {
        for (const auto& entry : std::filesystem::directory_iterator(base_directory))
        {
            if (entry.is_regular_file() && normalize_name(entry.path().extension().string()) == ".mtp")
            {
                load_mapping(entry.path());
            }
            if (!entry.is_directory())
            {
                continue;
            }
            const auto folder_name = normalize_name(entry.path().filename().string());
            if (folder_name != "models" && folder_name != "textures")
            {
                continue;
            }
            for (const auto& archive : std::filesystem::directory_iterator(entry.path()))
            {
                if (normalize_name(archive.path().extension().string()) == ".res")
                {
                    load_archive(archive.path(), cache_directory / base_directory.filename() / folder_name);
                }
            }
        }
    }
    index_lightmap_archive(mission_directory / "lightmaps/lightmaps.res");
    diagnostics::log(diagnostics::Severity::information,
                     "assets",
                     "Indexed " + std::to_string(files_.size()) + " original resources and " +
                         std::to_string(material_names_.size()) + " model material mappings and " +
                         std::to_string(lightmap_entries_.size()) + " object lightmaps");
}

void MissionAssets::index_lightmap_archive(const std::filesystem::path& path)
{
    std::error_code filesystem_error;
    if (!std::filesystem::is_regular_file(path, filesystem_error))
    {
        if (filesystem_error)
        {
            throw std::runtime_error("Cannot inspect object lightmap archive " + path.string() + ": " +
                                     filesystem_error.message());
        }
        diagnostics::log(diagnostics::Severity::warning,
                         "lightmap-assets",
                         "Object lightmap archive is missing: " + path.string());
        return;
    }
    if (std::filesystem::file_size(path) > 512 * 1024 * 1024)
    {
        throw std::runtime_error("Object lightmap archive exceeds size limit: " + path.string());
    }

    std::string error;
    const bool valid = RES_ForEachEntry(
        path.string(),
        [&](const std::string& resource_name, const std::uint8_t*, std::size_t)
        {
            std::string_view set_name;
            std::uint32_t model_index = 0;
            if (!parse_object_lightmap_entry_name(resource_name, set_name, model_index))
            {
                return;
            }
            const std::string key = normalize_name(resource_name.substr(
                resource_name.find_last_of("/\\:") == std::string::npos
                    ? 0
                    : resource_name.find_last_of("/\\:") + 1));
            const auto [entry, inserted] =
                lightmap_entries_.emplace(key, LightmapArchiveEntry{path, resource_name});
            if (!inserted && entry->second.resource_name != resource_name)
            {
                throw std::runtime_error("Duplicate object lightmap resource name: " + resource_name);
            }
        },
        error);
    if (!valid)
    {
        throw std::runtime_error("Cannot index object lightmap archive " + path.string() + ": " + error);
    }
    diagnostics::log(diagnostics::Severity::information,
                     "lightmap-assets",
                     "Indexed " + std::to_string(lightmap_entries_.size()) + " object lightmap resources from " +
                         path.string());
}

std::vector<ModelLightmapEntry> MissionAssets::model_lightmap_layout(const std::string& model_name)
{
    return model_lightmap_layout(std::vector<std::string>{model_name});
}

std::vector<ModelLightmapEntry>
MissionAssets::model_lightmap_layout(const std::vector<std::string>& model_names)
{
    struct LayoutModel final
    {
        std::string name;
        std::uint32_t model_type{};
        std::uint32_t lightmap_page_count{};
        std::size_t triangle_count{};
        bool is_sector{};
        std::vector<MefAttachment> attachments;
    };

    std::unordered_map<std::string, LayoutModel> parsed_models;
    const auto load_model = [&](const std::string& requested_name) -> const LayoutModel*
    {
        const std::string normalized = normalize_name(requested_name);
        if (const auto cached = parsed_models.find(normalized); cached != parsed_models.end())
        {
            return &cached->second;
        }
        const auto resource = files_.find(normalized + ".mef");
        if (resource == files_.end())
        {
            return nullptr;
        }

        const ParsedGeometry geometry = ParseMefFile(resource->second.string());
        LayoutModel model;
        model.name = normalized;
        model.model_type = geometry.modelType;
        model.triangle_count = geometry.triangles.size();
        if (geometry.modelType == 3)
        {
            for (const auto& render_block : geometry.renderBlocks)
            {
                model.lightmap_page_count = std::max(
                    model.lightmap_page_count, static_cast<std::uint32_t>(render_block.lightmapPage) + 1);
            }
        }
        model.is_sector = std::any_of(geometry.rawChunks.begin(),
                                      geometry.rawChunks.end(),
                                      [](const ParsedGeometry::RawChunk& chunk)
                                      { return chunk.isTag("TROP") && !chunk.data.empty(); });
        model.attachments = geometry.mefAttachments;
        return &parsed_models.emplace(normalized, std::move(model)).first->second;
    };

    const auto load_lod_chain = [&](const std::string& requested_name)
    {
        std::vector<const LayoutModel*> models;
        std::string candidate = normalize_name(requested_name);
        if (candidate.empty())
        {
            return models;
        }
        for (std::size_t lod_index = 0; lod_index < 5; ++lod_index)
        {
            const LayoutModel* model = load_model(candidate);
            if (model == nullptr)
            {
                break;
            }
            models.push_back(model);
            const auto final_character = static_cast<unsigned char>(candidate.back());
            if (final_character == std::numeric_limits<unsigned char>::max())
            {
                break;
            }
            candidate.back() = static_cast<char>(final_character + 1u);
        }
        return models;
    };

    std::vector<ModelLightmapEntry> entries;
    std::unordered_set<std::string> active_models;
    std::uint64_t next_lightmap_index = 0;
    std::function<void(const std::string&, std::size_t)> append_model_tree;
    append_model_tree = [&](const std::string& requested_name, std::size_t depth)
    {
        constexpr std::size_t maximum_attachment_depth = 16;
        if (depth > maximum_attachment_depth)
        {
            throw std::runtime_error("MEF lightmap attachment tree exceeds its depth limit: " +
                                     requested_name);
        }
        const auto lod_models = load_lod_chain(requested_name);
        if (lod_models.empty())
        {
            return;
        }

        const std::string& normalized_name = lod_models.front()->name;
        if (!active_models.insert(normalized_name).second)
        {
            throw std::runtime_error("MEF lightmap attachment cycle detected: " + normalized_name);
        }
        try
        {
            const bool lightmapped = lod_models.front()->model_type == 3;
            if (lightmapped && next_lightmap_index + lod_models.size() - 1 >
                                   static_cast<std::uint64_t>(std::numeric_limits<std::int32_t>::max()))
            {
                throw std::runtime_error("MEF lightmap index exceeds the retail signed index range");
            }
            const std::int32_t first_lightmap_index =
                lightmapped ? static_cast<std::int32_t>(next_lightmap_index) : -1;
            for (std::size_t lod_index = 0; lod_index < lod_models.size(); ++lod_index)
            {
                const auto& lod_model = *lod_models[lod_index];
                entries.push_back(
                    {lod_model.name,
                     lightmapped ? first_lightmap_index + static_cast<std::int32_t>(lod_index) : -1,
                     lightmapped && lod_model.model_type == 3,
                     lod_model.lightmap_page_count,
                     lod_model.triangle_count});
            }
            if (lightmapped)
            {
                next_lightmap_index += lod_models.size();
            }

            std::vector<std::string> ordinary_children;
            std::vector<std::string> sector_children;
            for (const MefAttachment& attachment : lod_models.front()->attachments)
            {
                const auto attachment_name_end =
                    std::find(std::begin(attachment.name), std::end(attachment.name), '\0');
                const std::string attachment_name(std::begin(attachment.name), attachment_name_end);
                const auto definition = magic_attachment_models_.find(normalize_name(attachment_name));
                if (attachment_name.empty() || definition == magic_attachment_models_.end())
                {
                    continue;
                }

                std::string child_model_name = definition->second;
                auto child_lod_models = load_lod_chain(child_model_name);
                if (child_lod_models.empty())
                {
                    child_model_name = "missing";
                    child_lod_models = load_lod_chain(child_model_name);
                }
                if (child_lod_models.empty())
                {
                    continue;
                }
                (child_lod_models.front()->is_sector ? sector_children : ordinary_children)
                    .push_back(std::move(child_model_name));
            }

            ordinary_children.insert(ordinary_children.end(), sector_children.begin(), sector_children.end());
            for (const std::string& child_model_name : ordinary_children)
            {
                append_model_tree(child_model_name, depth + 1);
            }
        }
        catch (...)
        {
            active_models.erase(normalized_name);
            throw;
        }
        active_models.erase(normalized_name);
    };

    for (const std::string& root_model_name : model_names)
    {
        append_model_tree(root_model_name, 0);
    }
    return entries;
}

std::vector<std::uint32_t> MissionAssets::lightmap_textures(std::string_view set_name,
                                                            std::uint32_t model_index)
{
    if (set_name.empty())
    {
        return {};
    }

    std::ostringstream entry_name_builder;
    entry_name_builder << normalize_name(set_name) << '_' << std::setw(5) << std::setfill('0') << model_index
                       << ".olm";
    const std::string entry_key = entry_name_builder.str();
    if (const auto cached = lightmap_texture_indices_.find(entry_key); cached != lightmap_texture_indices_.end())
    {
        return cached->second;
    }

    const auto resource = lightmap_entries_.find(entry_key);
    if (resource == lightmap_entries_.end())
    {
        diagnostics::log(diagnostics::Severity::warning,
                         "lightmap-assets",
                         "No object lightmap resource found for " + entry_key);
        return {};
    }

    const auto bytes = RES_Extract(resource->second.archive_path.string(), resource->second.resource_name);
    if (bytes.empty())
    {
        throw std::runtime_error("Indexed object lightmap resource could not be extracted: " +
                                 resource->second.resource_name);
    }
    const ObjectLightmap lightmap = parse_object_lightmap(bytes);
    std::vector<std::uint32_t> texture_indices;
    texture_indices.reserve(lightmap.images.size());
    for (const auto& image : lightmap.images)
    {
        const std::size_t pixel_count = static_cast<std::size_t>(image.width) * image.height;
        if (pixel_count == 0 || image.rgba_pixels.size() != pixel_count * 4)
        {
            throw std::runtime_error("Object lightmap image has inconsistent RGBA dimensions: " + entry_key);
        }

        gameplay::SceneTexture texture{image.width, image.height, {}, {}};
        texture.pixels.reserve(pixel_count);
        for (std::size_t pixel_index = 0; pixel_index < pixel_count; ++pixel_index)
        {
            const std::size_t byte_offset = pixel_index * 4;
            const std::uint32_t red = image.rgba_pixels[byte_offset];
            const std::uint32_t green = image.rgba_pixels[byte_offset + 1];
            const std::uint32_t blue = image.rgba_pixels[byte_offset + 2];
            const std::uint32_t alpha = image.rgba_pixels[byte_offset + 3];
            texture.pixels.push_back((alpha << 24) | (red << 16) | (green << 8) | blue);
        }
        texture_indices.push_back(static_cast<std::uint32_t>(textures_.size()));
        textures_.push_back(std::move(texture));
    }

    diagnostics::log(diagnostics::Severity::information,
                     "lightmap-assets",
                     "Loaded " + std::to_string(texture_indices.size()) + " RGBA page(s) for " + entry_key +
                         " (version " + std::to_string(lightmap.version) + ")");
    lightmap_texture_indices_.emplace(entry_key, texture_indices);
    return texture_indices;
}

void MissionAssets::load_archive(const std::filesystem::path& path,
                                 const std::filesystem::path& cache_directory)
{
    if (std::filesystem::file_size(path) > 512 * 1024 * 1024)
    {
        throw std::runtime_error("Resource archive exceeds size limit: " + path.string());
    }
    std::filesystem::create_directories(cache_directory);
    std::string error;
    const bool valid = RES_ForEachEntry(
        path.string(),
        [&](const std::string& name, const std::uint8_t* bytes, std::size_t length)
        {
            const auto normalized = normalize_name(name);
            const auto separator = normalized.find_last_of("/:");
            const auto filename = normalized.substr(separator == std::string::npos ? 0 : separator + 1);
            if (filename.empty() || filename == "." || filename == ".." || length > 64 * 1024 * 1024)
            {
                throw std::runtime_error("Invalid archive resource name or size in " + path.string());
            }
            const auto output_path = cache_directory / filename;
            std::ofstream output(output_path, std::ios::binary);
            if (!output.write(reinterpret_cast<const char*>(bytes), static_cast<std::streamsize>(length)))
            {
                throw std::runtime_error("Cannot cache original resource: " + output_path.string());
            }
            files_[filename] = output_path;
        },
        error);
    if (!valid)
    {
        throw std::runtime_error("Cannot parse resource archive " + path.string() + ": " + error);
    }
}

void MissionAssets::load_mapping(const std::filesystem::path& path)
{
    const auto mapping = MTP_Parse(path.string());
    if (!mapping.valid)
    {
        throw std::runtime_error("Cannot parse material mapping " + path.string() + ": " + mapping.error);
    }
    for (const auto& model_mapping : mapping.mappings)
    {
        material_names_[normalize_name(model_mapping.modelName)] = model_mapping.textureNames;
    }
}

std::uint32_t MissionAssets::texture(const std::string& name)
{
    const auto normalized = normalize_name(name);
    if (const auto found = texture_indices_.find(normalized); found != texture_indices_.end())
    {
        return found->second;
    }
    const auto resource = files_.find(normalized + ".tex");
    if (resource == files_.end())
    {
        diagnostics::log(diagnostics::Severity::warning, "assets", "Missing original texture: " + name);
        texture_indices_[normalized] = 0;
        return 0;
    }
    const auto parsed = TEX_Parse(resource->second.string());
    if (!parsed.valid || parsed.images.empty())
    {
        throw std::runtime_error("Cannot decode " + resource->second.string() + ": " + parsed.error);
    }
    const auto texture_index = static_cast<std::uint32_t>(textures_.size());
    textures_.push_back(decode_texture_with_mipmaps(parsed.images, 0, parsed.version == 11));
    texture_indices_[normalized] = texture_index;
    return texture_index;
}

std::vector<std::uint32_t> MissionAssets::terrain_textures(const std::filesystem::path& path)
{
    const auto parsed = TEX_Parse(path.string());
    if (!parsed.valid || parsed.images.empty())
    {
        throw std::runtime_error("Cannot decode terrain textures: " + parsed.error);
    }
    std::vector<std::uint32_t> indices;
    for (std::size_t image_index = 0; image_index < parsed.images.size(); ++image_index)
    {
        indices.push_back(static_cast<std::uint32_t>(textures_.size()));
        textures_.push_back(decode_texture_with_mipmaps(parsed.images, image_index, false));
    }
    return indices;
}

bool MissionAssets::has_model(const std::string& name) const
{
    const auto normalized = normalize_name(name);
    return meshes_.contains(normalized) || files_.contains(normalized + ".mef");
}

std::shared_ptr<gameplay::SceneMesh> MissionAssets::model(const std::string& name)
{
    const auto normalized = normalize_name(name);
    if (loading_models_.contains(normalized) || loading_models_.size() >= 32)
    {
        throw std::runtime_error("Recursive MEF attachment cycle or depth limit: " + normalized);
    }
    if (const auto found = meshes_.find(normalized); found != meshes_.end())
    {
        return found->second;
    }
    const auto resource = files_.find(normalized + ".mef");
    if (resource == files_.end())
    {
        throw std::runtime_error("Original model is missing: " + name);
    }
    const auto geometry = ParseMefFile(resource->second.string());
    auto mesh = std::make_shared<gameplay::SceneMesh>();
    const auto bone_positions = ComputeBoneWorldPositions(geometry.bones);
    for (const auto& position : bone_positions)
    {
        mesh->bone_rest_positions.push_back(
            {position.x / 4096.0f, position.y / 4096.0f, position.z / 4096.0f});
    }
    std::vector<std::uint32_t> materials;
    if (const auto mapping = material_names_.find(normalized); mapping != material_names_.end())
    {
        for (const auto& texture_name : mapping->second)
        {
            materials.push_back(texture(texture_name));
        }
    }
    std::vector<std::uint32_t> triangle_materials(geometry.triangles.size());
    std::vector<std::int32_t> triangle_lightmap_pages(geometry.triangles.size(), -1);
    for (const auto& block : geometry.renderBlocks)
    {
        if (block.triangleStart > triangle_materials.size() ||
            block.triangleCount > triangle_materials.size() - block.triangleStart)
        {
            throw std::runtime_error("Decoded MEF render block exceeds triangle array: " + name);
        }
        const auto material_index =
            block.materialSlot >= 0 && static_cast<std::size_t>(block.materialSlot) < materials.size()
                ? materials[block.materialSlot]
                : 0;
        std::fill_n(triangle_materials.begin() + static_cast<std::ptrdiff_t>(block.triangleStart),
                    block.triangleCount,
                    material_index);
        if (geometry.modelType == 3)
        {
            std::fill_n(triangle_lightmap_pages.begin() + static_cast<std::ptrdiff_t>(block.triangleStart),
                        block.triangleCount,
                        block.lightmapPage);
        }
    }
    for (std::size_t triangle_index = 0; triangle_index < geometry.triangles.size(); ++triangle_index)
    {
        gameplay::SceneTriangle triangle;
        triangle.texture_index = triangle_materials[triangle_index];
        triangle.lightmap_page_index = triangle_lightmap_pages[triangle_index];
        for (std::size_t corner_index = 0; corner_index < 3; ++corner_index)
        {
            const auto vertex_index = geometry.triangles[triangle_index][corner_index];
            if (vertex_index >= geometry.vertices.size())
            {
                throw std::runtime_error("Decoded MEF index exceeds vertex array: " + name);
            }
            const auto& vertex = geometry.vertices[vertex_index];
            const gameplay::Vector3 position{
                vertex.pos.x * 0.01f, vertex.pos.y * 0.01f, vertex.pos.z * 0.01f};
            if (!gameplay::finite(position) || !std::isfinite(vertex.uv.x) || !std::isfinite(vertex.uv.y) ||
                !std::isfinite(vertex.uv2.x) || !std::isfinite(vertex.uv2.y))
            {
                throw std::runtime_error("Decoded MEF vertex is not finite: " + name);
            }
            triangle.vertices[corner_index] = {position, vertex.uv.x, vertex.uv.y};
            triangle.vertices[corner_index].lightmap_u = vertex.uv2.x;
            triangle.vertices[corner_index].lightmap_v = vertex.uv2.y;
            if (vertex.boneIndex < mesh->bone_rest_positions.size())
            {
                triangle.vertices[corner_index].bone_index = vertex.boneIndex;
                triangle.vertices[corner_index].bone_local_position =
                    position - mesh->bone_rest_positions[vertex.boneIndex];
            }
        }
        mesh->triangles.push_back(triangle);
    }
    const auto decode_collision = [&](const auto& faces,
                                      const auto& vertices,
                                      const auto& materials,
                                      std::vector<gameplay::CollisionTriangle>& destination)
    {
        for (const auto& face : faces)
        {
            const std::array<std::uint16_t, 3> indices{face.a, face.b, face.c};
            gameplay::CollisionTriangle triangle;
            triangle.non_solid_portal = face.mat < materials.size() && materials[face.mat].portal != 0xffffu;
            for (std::size_t corner_index = 0; corner_index < 3; ++corner_index)
            {
                if (indices[corner_index] >= vertices.size())
                {
                    throw std::runtime_error("Decoded MEF collision index is invalid: " + name);
                }
                const auto& vertex = vertices[indices[corner_index]];
                triangle.positions[corner_index] = {
                    vertex.px / 4096.0f, vertex.py / 4096.0f, vertex.pz / 4096.0f};
            }
            destination.push_back(triangle);
        }
    };
    decode_collision(geometry.ecfcFaces, geometry.xtvcVerts, geometry.tamcRecords, mesh->collision);
    decode_collision(
        geometry.ecfcFaces1, geometry.xtvcVerts1, geometry.tamcRecords1, mesh->movement_collision);
    for (const auto& face : geometry.ecfcFaces)
    {
        const std::array<std::uint16_t, 3> vertex_indices{face.a, face.b, face.c};
        gameplay::SceneSkinTriangle triangle;
        for (std::size_t corner_index = 0; corner_index < vertex_indices.size(); ++corner_index)
        {
            if (vertex_indices[corner_index] >= geometry.xtvcVerts.size())
            {
                throw std::runtime_error("Decoded skinned collision index is invalid: " + name);
            }
            const auto& source_vertex = geometry.xtvcVerts[vertex_indices[corner_index]];
            const gameplay::Vector3 position{source_vertex.px / 4096.0f,
                                             source_vertex.py / 4096.0f,
                                             source_vertex.pz / 4096.0f};
            if (!gameplay::finite(position))
            {
                throw std::runtime_error("Decoded skinned collision vertex is not finite: " + name);
            }
            auto& vertex = triangle.vertices[corner_index];
            vertex.position = position;
            if (source_vertex.boneIndex < mesh->bone_rest_positions.size())
            {
                vertex.bone_index = source_vertex.boneIndex;
                vertex.bone_local_position = position - mesh->bone_rest_positions[source_vertex.boneIndex];
            }
        }
        mesh->skinned_collision.push_back(std::move(triangle));
    }
    loading_models_.insert(normalized);
    try
    {
        for (const auto& attachment : geometry.mefAttachments)
        {
            const auto name_end = std::find(std::begin(attachment.name), std::end(attachment.name), '\0');
            const std::string child_name(std::begin(attachment.name), name_end);
            const auto normalized_child = normalize_name(child_name);
            const auto magic_model = magic_attachment_models_.find(normalized_child);
            if (child_name.empty() || magic_model == magic_attachment_models_.end() ||
                invisible_attachment_models_.contains(normalized_child))
            {
                continue;
            }
            const auto& child_model_name = magic_model->second;
            if (!files_.contains(child_model_name + ".mef"))
            {
                diagnostics::log(diagnostics::Severity::debug,
                                 "attachments",
                                 "Magic-object attachment has no visual MEF model: " + child_name + " -> " +
                                     child_model_name);
                continue;
            }
            const auto child = model(child_model_name);
            const glm::mat4 rotation{attachment.r00,
                                     attachment.r01,
                                     attachment.r02,
                                     0,
                                     attachment.r03,
                                     attachment.r04,
                                     attachment.r05,
                                     0,
                                     attachment.r06,
                                     attachment.r07,
                                     attachment.r08,
                                     0,
                                     0,
                                     0,
                                     0,
                                     1};
            const glm::vec3 offset{attachment.px / 4096.0f, attachment.py / 4096.0f, attachment.pz / 4096.0f};
            const auto transform = [&](gameplay::Vector3 position)
            {
                const auto transformed =
                    glm::vec3(rotation * glm::vec4(position.x, position.y, position.z, 1)) + offset;
                const gameplay::Vector3 result{transformed.x, transformed.y, transformed.z};
                if (!gameplay::finite(result))
                {
                    throw std::runtime_error("MEF attachment transform is invalid: " + child_name);
                }
                return result;
            };
            for (auto triangle : child->triangles)
            {
                triangle.firing_only = triangle.firing_only ||
                                       muzzle_flash_models_.contains(normalized_child) ||
                                       muzzle_flash_models_.contains(child_model_name);
                for (auto& vertex : triangle.vertices)
                {
                    vertex.position = transform(vertex.position);
                    vertex.bone_index = static_cast<std::uint32_t>(-1);
                    if (attachment.boneId >= 0 &&
                        static_cast<std::size_t>(attachment.boneId) < mesh->bone_rest_positions.size())
                    {
                        vertex.bone_index = static_cast<std::uint32_t>(attachment.boneId);
                        vertex.bone_local_position =
                            vertex.position - mesh->bone_rest_positions[vertex.bone_index];
                    }
                }
                mesh->triangles.push_back(triangle);
            }
            for (auto triangle : child->collision)
            {
                if (muzzle_flash_models_.contains(normalized_child) ||
                    muzzle_flash_models_.contains(child_model_name))
                {
                    continue;
                }
                for (auto& position : triangle.positions)
                {
                    position = transform(position);
                }
                mesh->collision.push_back(triangle);
            }
            for (auto triangle : child->movement_collision)
            {
                if (muzzle_flash_models_.contains(normalized_child) ||
                    muzzle_flash_models_.contains(child_model_name))
                {
                    continue;
                }
                for (auto& position : triangle.positions)
                {
                    position = transform(position);
                }
                mesh->movement_collision.push_back(triangle);
            }
            mesh->attachment_count += 1 + child->attachment_count;
            mesh->maximum_attachment_depth =
                std::max(mesh->maximum_attachment_depth, 1 + child->maximum_attachment_depth);
        }
    }
    catch (...)
    {
        loading_models_.erase(normalized);
        throw;
    }
    loading_models_.erase(normalized);
    meshes_[normalized] = mesh;
    return mesh;
}

} // namespace igi::assets
