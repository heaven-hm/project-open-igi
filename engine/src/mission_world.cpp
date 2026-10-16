#include "igi/logger.hpp"
#include "igi/mission_world.hpp"

#include "common.h"
#include "iff_parser.h"
#include "mef_native.h"
#include "mtp_parser.h"
#include "qvm_decompiler.h"
#include "qvm_parser.h"
#include "res_parser.h"
#include "terrain_files.h"
#include "tex_parser.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <fstream>
#include <limits>
#include <sstream>

namespace igi::gameplay {

namespace {

[[nodiscard]] std::string to_lower_ascii(std::string value) {
    for (char& ch : value) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return value;
}

[[nodiscard]] std::string strip_extension_and_lower(std::string value) {
    const auto slash_pos = value.find_last_of("/\\");
    if (slash_pos != std::string::npos) {
        value = value.substr(slash_pos + 1);
    }
    const auto dot_pos = value.find_last_of('.');
    if (dot_pos != std::string::npos) {
        value = value.substr(0, dot_pos);
    }
    return to_lower_ascii(std::move(value));
}

[[nodiscard]] bool read_binary_file(
    const std::filesystem::path& path,
    std::vector<std::uint8_t>& out_bytes) {
    std::ifstream stream(path, std::ios::binary | std::ios::ate);
    if (!stream) {
        return false;
    }
    const auto file_size = stream.tellg();
    if (file_size <= 0) {
        return false;
    }
    out_bytes.resize(static_cast<std::size_t>(file_size));
    stream.seekg(0, std::ios::beg);
    return static_cast<bool>(stream.read(
        reinterpret_cast<char*>(out_bytes.data()),
        static_cast<std::streamsize>(out_bytes.size())));
}

[[nodiscard]] TextureImage decode_tex_image(
    const std::string& name,
    const TEXImage& raw_img) {
    TextureImage result;
    result.name = name;
    result.width = raw_img.width;
    result.height = raw_img.height;
    const std::size_t pixel_count =
        static_cast<std::size_t>(raw_img.width) * static_cast<std::size_t>(raw_img.height);
    if (pixel_count == 0) {
        return result;
    }
    result.argb_pixels.resize(pixel_count, 0xFF808080u);

    if (raw_img.mode == 2 && raw_img.pixels.size() >= pixel_count * 2) {
        for (std::size_t i = 0; i < pixel_count; ++i) {
            const std::uint16_t packed = static_cast<std::uint16_t>(
                raw_img.pixels[i * 2] | (static_cast<std::uint16_t>(raw_img.pixels[i * 2 + 1]) << 8));
            const std::uint32_t r5 = (packed >> 10) & 0x1Fu;
            const std::uint32_t g5 = (packed >> 5) & 0x1Fu;
            const std::uint32_t b5 = packed & 0x1Fu;
            const std::uint32_t r8 = (r5 << 3) | (r5 >> 2);
            const std::uint32_t g8 = (g5 << 3) | (g5 >> 2);
            const std::uint32_t b8 = (b5 << 3) | (b5 >> 2);
            result.argb_pixels[i] = 0xFF000000u | (r8 << 16) | (g8 << 8) | b8;
        }
    } else if ((raw_img.mode == 3 || raw_img.mode == 67) &&
               raw_img.pixels.size() >= pixel_count * 4) {
        for (std::size_t i = 0; i < pixel_count; ++i) {
            const std::uint32_t b = raw_img.pixels[i * 4 + 0];
            const std::uint32_t g = raw_img.pixels[i * 4 + 1];
            const std::uint32_t r = raw_img.pixels[i * 4 + 2];
            const std::uint32_t a = raw_img.pixels[i * 4 + 3];
            if (a < 250) {
                result.has_alpha = true;
            }
            result.argb_pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }
    return result;
}

struct QscCallNode {
    std::vector<std::string> scalar_args;
    std::vector<QscCallNode> child_calls;
};

void skip_whitespace(const std::string& text, std::size_t& pos) {
    while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
        ++pos;
    }
}

bool parse_qsc_call(const std::string& text, std::size_t& pos, QscCallNode& out_node) {
    skip_whitespace(text, pos);
    constexpr const char* kPrefix = "Task_New(";
    constexpr std::size_t kPrefixLen = 9;
    if (pos + kPrefixLen > text.size() ||
        text.compare(pos, kPrefixLen, kPrefix) != 0) {
        return false;
    }
    pos += kPrefixLen;

    while (pos < text.size()) {
        skip_whitespace(text, pos);
        if (pos >= text.size()) {
            break;
        }
        if (text[pos] == ')') {
            ++pos;
            return true;
        }
        if (text[pos] == ',') {
            ++pos;
            continue;
        }

        if (pos + kPrefixLen <= text.size() &&
            text.compare(pos, kPrefixLen, kPrefix) == 0) {
            QscCallNode child;
            if (parse_qsc_call(text, pos, child)) {
                out_node.child_calls.push_back(std::move(child));
            }
            continue;
        }

        if (text[pos] == '"') {
            ++pos;
            std::string literal;
            while (pos < text.size() && text[pos] != '"') {
                if (text[pos] == '\\' && pos + 1 < text.size()) {
                    ++pos;
                }
                literal.push_back(text[pos]);
                ++pos;
            }
            if (pos < text.size() && text[pos] == '"') {
                ++pos;
            }
            out_node.scalar_args.push_back(std::move(literal));
            continue;
        }

        std::string token;
        int paren_depth = 0;
        while (pos < text.size()) {
            const char c = text[pos];
            if (c == '(') {
                ++paren_depth;
            } else if (c == ')') {
                if (paren_depth == 0) {
                    break;
                }
                --paren_depth;
            } else if (c == ',' && paren_depth == 0) {
                break;
            }
            token.push_back(c);
            ++pos;
        }
        // Trim token
        while (!token.empty() && std::isspace(static_cast<unsigned char>(token.front()))) {
            token.erase(token.begin());
        }
        while (!token.empty() && std::isspace(static_cast<unsigned char>(token.back()))) {
            token.pop_back();
        }
        if (!token.empty()) {
            out_node.scalar_args.push_back(std::move(token));
        }
    }
    return false;
}

[[nodiscard]] double parse_double_safe(const std::string& str, double fallback = 0.0) {
    char* end_ptr = nullptr;
    const double val = std::strtod(str.c_str(), &end_ptr);
    if (end_ptr == str.c_str() || !std::isfinite(val)) {
        return fallback;
    }
    return val;
}

[[nodiscard]] int parse_int_safe(const std::string& str, int fallback = 0) {
    char* end_ptr = nullptr;
    const long val = std::strtol(str.c_str(), &end_ptr, 10);
    if (end_ptr == str.c_str()) {
        return fallback;
    }
    return static_cast<int>(val);
}

[[nodiscard]] bool looks_like_model_id(const std::string& str) {
    if (str.size() < 7 || str.size() > 12) {
        return false;
    }
    return std::isdigit(static_cast<unsigned char>(str[0])) &&
           std::isdigit(static_cast<unsigned char>(str[1])) &&
           std::isdigit(static_cast<unsigned char>(str[2])) &&
           str[3] == '_';
}

// Retail TerrainOrientation table (8 square symmetries)
constexpr std::uint8_t kTerrainSlotTable[64] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    2, 0, 3, 1, 6, 4, 7, 5,
    3, 2, 1, 0, 7, 6, 5, 4,
    1, 3, 0, 2, 5, 7, 4, 6,
    1, 0, 3, 2, 5, 4, 7, 6,
    3, 1, 2, 0, 7, 5, 6, 4,
    2, 3, 0, 1, 6, 7, 4, 5,
    0, 2, 1, 3, 4, 6, 5, 7,
};

[[nodiscard]] std::uint8_t compose_terrain_orientation(
    std::uint8_t parent_orientation,
    std::uint8_t child_orientation) noexcept {
    if ((parent_orientation & 4u) == 0u) {
        return static_cast<std::uint8_t>(
            (child_orientation & 4u) |
            ((parent_orientation + child_orientation) & 3u));
    }

    return static_cast<std::uint8_t>(
        ((child_orientation & 4u) ^ 4u) |
        ((parent_orientation - child_orientation) & 3u));
}

void transform_terrain_local_xy(
    std::uint8_t orientation,
    double& io_x,
    double& io_y) noexcept {
    const double mirrored_x = (orientation & 4u) != 0u ? -io_x : io_x;
    if ((orientation & 1u) != 0u) {
        if ((orientation & 2u) != 0u) {
            io_x = io_y;
            io_y = -mirrored_x;
        } else {
            io_x = -io_y;
            io_y = mirrored_x;
        }
    } else if ((orientation & 2u) != 0u) {
        io_x = -mirrored_x;
        io_y = -io_y;
    } else {
        io_x = mirrored_x;
    }
}

void build_collision_triangles(
    const std::vector<XtvcVertex>& verts,
    const std::vector<EcfcFace>& faces,
    const std::vector<TamcRecord>& materials,
    std::vector<CollisionTriangle>& out_triangles) {
    out_triangles.clear();
    out_triangles.reserve(faces.size());
    for (const EcfcFace& face : faces) {
        if (face.a >= verts.size() || face.b >= verts.size() || face.c >= verts.size()) {
            continue;
        }
        CollisionTriangle tri{};
        tri.v0 = {verts[face.a].px, verts[face.a].py, verts[face.a].pz};
        tri.v1 = {verts[face.b].px, verts[face.b].py, verts[face.b].pz};
        tri.v2 = {verts[face.c].px, verts[face.c].py, verts[face.c].pz};

        if (face.mat < materials.size()) {
            tri.material_id = materials[face.mat].matId;
            tri.is_portal = (materials[face.mat].portal != 0xFFFFu);
        }

        const float abx = tri.v1.x - tri.v0.x;
        const float aby = tri.v1.y - tri.v0.y;
        const float abz = tri.v1.z - tri.v0.z;
        const float acx = tri.v2.x - tri.v0.x;
        const float acy = tri.v2.y - tri.v0.y;
        const float acz = tri.v2.z - tri.v0.z;
        const float nx = aby * acz - abz * acy;
        const float ny = abz * acx - abx * acz;
        const float nz = abx * acy - aby * acx;
        const float len_sq = nx * nx + ny * ny + nz * nz;
        if (len_sq <= 1.0e-6f) {
            continue;
        }
        const float inv_len = 1.0f / std::sqrt(len_sq);
        tri.unit_normal = {nx * inv_len, ny * inv_len, nz * inv_len};
        out_triangles.push_back(tri);
    }
}

} // namespace

std::uint32_t TextureImage::sample_nearest(float u, float v) const noexcept {
    if (width == 0 || height == 0 || argb_pixels.empty()) {
        return 0xFF909890u;
    }
    float uf = u - std::floor(u);
    float vf = v - std::floor(v);
    if (!std::isfinite(uf)) uf = 0.0f;
    if (!std::isfinite(vf)) vf = 0.0f;
    const std::uint32_t x = std::min(
        static_cast<std::uint32_t>(uf * static_cast<float>(width)),
        width - 1u);
    const std::uint32_t y = std::min(
        static_cast<std::uint32_t>(vf * static_cast<float>(height)),
        height - 1u);
    return argb_pixels[static_cast<std::size_t>(y) * width + x];
}

std::uint32_t TextureImage::sample_bilinear(float u, float v) const noexcept {
    return sample_nearest(u, v);
}

Basis3x3 MissionWorld::basis_from_euler(float alpha, float beta, float gamma) noexcept {
    // Matches retail Matrix3f::FromEngineEulerAngles (0x4B38E0):
    // R = RotationZ(gamma) * RotationY(beta) * RotationX(alpha)
    const float ca = std::cos(alpha);
    const float sa = std::sin(alpha);
    const float cb = std::cos(beta);
    const float sb = std::sin(beta);
    const float cg = std::cos(gamma);
    const float sg = std::sin(gamma);

    // Ry * Rx:
    const float yx11 = cb;
    const float yx12 = sb * sa;
    const float yx13 = sb * ca;
    const float yx21 = 0.0f;
    const float yx22 = ca;
    const float yx23 = -sa;
    const float yx31 = -sb;
    const float yx32 = cb * sa;
    const float yx33 = cb * ca;

    // Rz * (Ry * Rx):
    Basis3x3 r{};
    r.m11 = cg * yx11 - sg * yx21;
    r.m12 = cg * yx12 - sg * yx22;
    r.m13 = cg * yx13 - sg * yx23;
    r.m21 = sg * yx11 + cg * yx21;
    r.m22 = sg * yx12 + cg * yx22;
    r.m23 = sg * yx13 + cg * yx23;
    r.m31 = yx31;
    r.m32 = yx32;
    r.m33 = yx33;
    return r;
}

Vector3 MissionWorld::transform_local_to_world_offset(
    Vector3 local_point,
    const Basis3x3& basis) noexcept {
    // Retail OpenIGI Orientation.Transposed().Transform(localPoint) == R * localPoint
    return {
        basis.m11 * local_point.x + basis.m12 * local_point.y + basis.m13 * local_point.z,
        basis.m21 * local_point.x + basis.m22 * local_point.y + basis.m23 * local_point.z,
        basis.m31 * local_point.x + basis.m32 * local_point.y + basis.m33 * local_point.z,
    };
}

Vector3 MissionWorld::transform_world_to_local_offset(
    Vector3 world_offset,
    const Basis3x3& basis) noexcept {
    // Inverse of orthogonal R is R^T * world_offset
    return {
        basis.m11 * world_offset.x + basis.m21 * world_offset.y + basis.m31 * world_offset.z,
        basis.m12 * world_offset.x + basis.m22 * world_offset.y + basis.m32 * world_offset.z,
        basis.m13 * world_offset.x + basis.m23 * world_offset.y + basis.m33 * world_offset.z,
    };
}

bool MissionWorld::load_from_retail_install(
    const std::filesystem::path& game_root,
    int mission_number) {
    try {
        is_loaded_ = false;
        game_root_ = game_root;

        const igi::assets::GameAssetSummary summary =
            igi::assets::GameAssetCatalog::scan(game_root, mission_number);
        if (!summary.error.empty()) {
            status_message_ = "Asset scan failed: " + summary.error;
            IGI_LOG_ERROR("World", status_message_);
            return false;
        }
        level_root_ = summary.level_root;

        // 1. Load objects.qsc or decompile OBJECTS.QVM
        std::string qsc_text;
        const std::filesystem::path qsc_path = level_root_ / "objects.qsc";
        if (std::filesystem::exists(qsc_path)) {
            std::ifstream qsc_stream(qsc_path);
            std::ostringstream buffer;
            buffer << qsc_stream.rdbuf();
            qsc_text = buffer.str();
        } else {
            const std::filesystem::path qvm_path = level_root_ / "OBJECTS.QVM";
            if (std::filesystem::exists(qvm_path)) {
                const QVMFile qvm = QVM_Parse(qvm_path.string());
                if (qvm.valid) {
                    qsc_text = QVM_DecompileToString(qvm);
                }
            }
        }

        if (qsc_text.empty()) {
            status_message_ = "Failed to load or decompile level objects script.";
            IGI_LOG_ERROR("World", status_message_);
            return false;
        }

        if (!parse_qsc_scene(qsc_text)) {
            status_message_ = "Failed to parse objects.qsc scene tasks.";
            IGI_LOG_ERROR("World", status_message_);
            return false;
        }

        // 2. Transform navigation graphs into world space using AiNavigationTransform
        world_navigation_graphs_.clear();
        for (const auto& [graph_id, local_graph] : summary.navigation_graphs) {
            igi::assets::NavigationGraph world_graph = local_graph;
            auto offset_it = ai_graph_offsets_.find(graph_id);
            if (offset_it != ai_graph_offsets_.end()) {
                NavigationGraphTransform transform{};
                transform.graph_task_offset = offset_it->second;
                transform.parent_orientation_is_active = false;
                transform.parent_world_position = {0.0, 0.0, 0.0};
                for (auto& node : world_graph.nodes) {
                    const NavigationPosition world_pos =
                        AiNavigationTransform::apply_graph_transform(
                            {node.x, node.y, node.z}, transform);
                    node.x = world_pos.x;
                    node.y = world_pos.y;
                    node.z = world_pos.z;
                }
            }
            world_navigation_graphs_.emplace(graph_id, std::move(world_graph));
        }

        // 3. Load MTP model-to-texture mappings
        load_mtp_packages();

        // 4. Load terrain octree, heightmaps, and extract nearby terrain triangles
        load_terrain_octree_and_heightmaps();
        extract_nearby_terrain_mesh(
            player_spawn_position_.x,
            player_spawn_position_.y,
            520000.0);

        // 5. Load referenced .mef models and .tex textures
        load_referenced_models_and_textures();

        // 6. Load skeletal animation banks (000.IFF, 001.IFF)
        load_animation_banks();

        is_loaded_ = true;
        std::ostringstream msg;
        msg << "Loaded Mission " << mission_number
            << " (" << placed_objects_.size() << " objects, "
            << models_.size() << " MEF models, "
            << textures_.size() << " TEX textures, "
            << soldier_spawns_.size() << " AI soldiers, "
            << world_navigation_graphs_.size() << " nav graphs, "
            << terrain_triangles_.size() << " terrain tris)";
        status_message_ = msg.str();
        IGI_LOG_INFO("World", status_message_);
        return true;
    } catch (const std::exception& e) {
        status_message_ = std::string("Exception during mission load: ") + e.what();
        IGI_LOG_ERROR("World", status_message_);
        is_loaded_ = false;
        return false;
    }
}

bool MissionWorld::parse_qsc_scene(const std::string& qsc_text) {
    placed_objects_.clear();
    soldier_spawns_.clear();
    ai_graph_offsets_.clear();
    height_maps_.clear();
    player_initial_weapons_.clear();

    std::vector<PatrolCommand> pending_patrol;
    std::size_t pos = 0;
    while (pos < qsc_text.size()) {
        const auto next_pos = qsc_text.find("Task_New(", pos);
        if (next_pos == std::string::npos) {
            break;
        }
        pos = next_pos + 9;
        std::size_t parse_position = next_pos;
        QscCallNode root_call;
        if (!parse_qsc_call(qsc_text, parse_position, root_call)) {
            continue;
        }
        if (root_call.scalar_args.size() < 2) {
            continue;
        }

        const int task_id = parse_int_safe(root_call.scalar_args[0], -1);
        const std::string& class_name = root_call.scalar_args[1];

        if (class_name == "HumanPlayer" && root_call.scalar_args.size() >= 8) {
            player_spawn_position_.x = parse_double_safe(root_call.scalar_args[3], 24578514.0);
            player_spawn_position_.y = parse_double_safe(root_call.scalar_args[4], -55852816.0);
            player_spawn_position_.z = parse_double_safe(root_call.scalar_args[5], 174421680.0);
            player_spawn_gamma_ = static_cast<float>(parse_double_safe(root_call.scalar_args[6], 3.095987));
            for (const QscCallNode& child : root_call.child_calls) {
                if (child.scalar_args.size() >= 4 && child.scalar_args[1] == "Gun") {
                    player_initial_weapons_.push_back(child.scalar_args[3]);
                }
            }
            continue;
        }

        if (class_name == "AIGraph" && root_call.scalar_args.size() >= 6) {
            NavigationPosition offset{};
            offset.x = parse_double_safe(root_call.scalar_args[3]);
            offset.y = parse_double_safe(root_call.scalar_args[4]);
            offset.z = parse_double_safe(root_call.scalar_args[5]);
            ai_graph_offsets_[task_id] = offset;
            continue;
        }

        if (class_name == "HeightMap" && root_call.scalar_args.size() >= 9) {
            HeightMapPatch patch{};
            patch.world_x = parse_double_safe(root_call.scalar_args[3]);
            patch.world_y = parse_double_safe(root_call.scalar_args[4]);
            patch.world_z = parse_double_safe(root_call.scalar_args[5]);
            patch.level = parse_int_safe(root_call.scalar_args[6], 11);
            patch.grid_size = parse_int_safe(root_call.scalar_args[7], 128);
            patch.bitmap_id = parse_int_safe(root_call.scalar_args[8], 0);
            height_maps_.push_back(patch);
            continue;
        }

        if (class_name == "PatrolPath") {
            // In objects.qsc a PatrolPath is emitted as a sibling immediately
            // before the soldier that owns it; attach it to the next soldier.
            pending_patrol.clear();
            for (const QscCallNode& cmd_node : root_call.child_calls) {
                if (cmd_node.scalar_args.size() >= 4 &&
                    cmd_node.scalar_args[1] == "PatrolPathCommand") {
                    PatrolCommand cmd{};
                    cmd.opcode = parse_int_safe(cmd_node.scalar_args[2], 0);
                    cmd.argument = parse_int_safe(cmd_node.scalar_args[3], 0);
                    pending_patrol.push_back(cmd);
                }
            }
            continue;
        }

        if (class_name == "HumanSoldier" && root_call.scalar_args.size() >= 8) {
            SoldierSpawn soldier{};
            soldier.task_id = task_id;
            soldier.name = root_call.scalar_args[2];
            soldier.position.x = parse_double_safe(root_call.scalar_args[3]);
            soldier.position.y = parse_double_safe(root_call.scalar_args[4]);
            soldier.position.z = parse_double_safe(root_call.scalar_args[5]);
            soldier.gamma = static_cast<float>(parse_double_safe(root_call.scalar_args[6]));
            soldier.model_id = root_call.scalar_args[7];
            if (root_call.scalar_args.size() >= 9) {
                soldier.team = parse_int_safe(root_call.scalar_args[8], 1);
            }
            if (root_call.scalar_args.size() >= 10) {
                soldier.bone_hierarchy = parse_int_safe(root_call.scalar_args[9], 0);
            }
            if (root_call.scalar_args.size() >= 11) {
                soldier.stand_animation = parse_int_safe(root_call.scalar_args[10], 0);
            }

            for (const QscCallNode& child : root_call.child_calls) {
                if (child.scalar_args.size() >= 4 && child.scalar_args[1] == "Gun") {
                    soldier.weapon_id = child.scalar_args[3];
                } else if (child.scalar_args.size() >= 5 && child.scalar_args[1] == "HumanAI") {
                    soldier.ai_type = child.scalar_args[3];
                    soldier.graph_id = parse_int_safe(child.scalar_args[4], 1);
                    for (const QscCallNode& grandchild : child.child_calls) {
                        if (grandchild.scalar_args.size() >= 2 &&
                            grandchild.scalar_args[1] == "PatrolPath") {
                            for (const QscCallNode& cmd_node : grandchild.child_calls) {
                                if (cmd_node.scalar_args.size() >= 4 &&
                                    cmd_node.scalar_args[1] == "PatrolPathCommand") {
                                    PatrolCommand cmd{};
                                    cmd.opcode = parse_int_safe(cmd_node.scalar_args[2], 0);
                                    cmd.argument = parse_int_safe(cmd_node.scalar_args[3], 0);
                                    soldier.patrol_commands.push_back(cmd);
                                }
                            }
                        }
                    }
                }
            }
            if (soldier.patrol_commands.empty() && !pending_patrol.empty()) {
                soldier.patrol_commands = std::move(pending_patrol);
            }
            pending_patrol.clear();
            soldier_spawns_.push_back(std::move(soldier));
            continue;
        }

        const bool is_spatial_class =
            class_name == "Building" ||
            class_name == "EditRigidObj" ||
            class_name == "Door" ||
            class_name == "Fence" ||
            class_name == "Switch" ||
            class_name == "Terminal" ||
            class_name == "SCamera" ||
            class_name == "ExplodeObject" ||
            class_name == "GunPickup" ||
            class_name == "AmmoPickup" ||
            class_name == "Car" ||
            class_name == "Heli" ||
            class_name == "Train" ||
            class_name == "SplineObj" ||
            class_name == "DynamicObject" ||
            class_name == "ClimbArea";

        if (!is_spatial_class || root_call.scalar_args.size() < 6) {
            continue;
        }

        PlacedObject obj{};
        obj.task_id = task_id;
        obj.class_name = class_name;
        obj.instance_name = root_call.scalar_args[2];
        obj.position.x = parse_double_safe(root_call.scalar_args[3]);
        obj.position.y = parse_double_safe(root_call.scalar_args[4]);
        obj.position.z = parse_double_safe(root_call.scalar_args[5]);
        obj.closed_position = obj.position;

        if (class_name == "ClimbArea") {
            obj.kind = PlacedObjectKind::ClimbLadder;
            obj.is_interactive = true;
            placed_objects_.push_back(std::move(obj));
            continue;
        }

        // Find model_id among scalar_args[6..] or child_calls
        std::size_t model_arg_index = 0;
        for (std::size_t i = 6; i < root_call.scalar_args.size(); ++i) {
            if (looks_like_model_id(root_call.scalar_args[i])) {
                obj.model_id = root_call.scalar_args[i];
                model_arg_index = i;
                break;
            }
        }

        if (model_arg_index == 9) {
            obj.alpha = static_cast<float>(parse_double_safe(root_call.scalar_args[6]));
            obj.beta = static_cast<float>(parse_double_safe(root_call.scalar_args[7]));
            obj.gamma = static_cast<float>(parse_double_safe(root_call.scalar_args[8]));
        } else if (model_arg_index >= 7) {
            obj.gamma = static_cast<float>(parse_double_safe(root_call.scalar_args[6]));
        } else if (root_call.scalar_args.size() >= 9) {
            obj.alpha = static_cast<float>(parse_double_safe(root_call.scalar_args[6]));
            obj.beta = static_cast<float>(parse_double_safe(root_call.scalar_args[7]));
            obj.gamma = static_cast<float>(parse_double_safe(root_call.scalar_args[8]));
        }

        if (obj.model_id.empty()) {
            for (const QscCallNode& child : root_call.child_calls) {
                for (std::size_t i = 2; i < child.scalar_args.size(); ++i) {
                    if (looks_like_model_id(child.scalar_args[i])) {
                        obj.model_id = child.scalar_args[i];
                        break;
                    }
                }
                if (!obj.model_id.empty()) {
                    break;
                }
            }
        }

        obj.basis = basis_from_euler(obj.alpha, obj.beta, obj.gamma);

        if (class_name == "Door") {
            obj.kind = PlacedObjectKind::Door;
            obj.is_interactive = true;
        } else if (class_name == "Fence") {
            obj.kind = PlacedObjectKind::Fence;
        } else if (class_name == "Switch") {
            obj.kind = PlacedObjectKind::Switch;
            obj.is_interactive = true;
        } else if (class_name == "Terminal") {
            obj.kind = PlacedObjectKind::Terminal;
            obj.is_interactive = true;
        } else if (class_name == "SCamera") {
            obj.kind = PlacedObjectKind::SecurityCamera;
            obj.is_destructible = true;
            obj.health = 35.0f;
        } else if (class_name == "ExplodeObject") {
            obj.kind = PlacedObjectKind::ExplosiveProp;
            obj.is_destructible = true;
            obj.health = 45.0f;
        } else if (class_name == "GunPickup") {
            obj.kind = PlacedObjectKind::GunPickup;
            obj.is_interactive = true;
            obj.pickup_weapon_id = (root_call.scalar_args.size() >= 11)
                ? root_call.scalar_args[10]
                : "WEAPON_ID_M16A2";
            obj.pickup_ammo_count = 30;
        } else if (class_name == "AmmoPickup") {
            obj.kind = PlacedObjectKind::AmmoPickup;
            obj.is_interactive = true;
            obj.pickup_ammo_count = 60;
        } else if (class_name == "Car" || class_name == "Heli" || class_name == "Train") {
            obj.kind = PlacedObjectKind::Vehicle;
        } else if (class_name == "EditRigidObj") {
            obj.kind = PlacedObjectKind::RigidProp;
        } else {
            obj.kind = PlacedObjectKind::StaticBuilding;
        }

        if (!obj.model_id.empty()) {
            placed_objects_.push_back(std::move(obj));
        }
    }

    return !placed_objects_.empty();
}

void MissionWorld::load_mtp_packages() {
    mtp_model_textures_.clear();
    const std::array<std::filesystem::path, 3> mtp_paths = {
        game_root_ / "COMMON" / "COMMON.MTP",
        level_root_.parent_path() / "COMMON" / "location0.mtp",
        level_root_ / (level_root_.filename().string() + ".MTP"),
    };

    for (const auto& mtp_path : mtp_paths) {
        if (!std::filesystem::exists(mtp_path)) {
            continue;
        }
        const MTPFile mtp = MTP_Parse(mtp_path.string());
        if (!mtp.valid) {
            continue;
        }
        for (const MTPModelTexture& mapping : mtp.mappings) {
            const std::string key = strip_extension_and_lower(mapping.modelName);
            std::vector<std::string> tex_list;
            tex_list.reserve(mapping.textureNames.size());
            for (const std::string& tex_name : mapping.textureNames) {
                tex_list.push_back(strip_extension_and_lower(tex_name));
            }
            mtp_model_textures_[key] = std::move(tex_list);
        }
    }
}

void MissionWorld::load_referenced_models_and_textures() {
    models_.clear();
    textures_.clear();

    std::unordered_map<std::string, bool> needed_models;
    for (const PlacedObject& obj : placed_objects_) {
        if (!obj.model_id.empty()) {
            needed_models[strip_extension_and_lower(obj.model_id)] = true;
        }
    }
    for (const SoldierSpawn& soldier : soldier_spawns_) {
        if (!soldier.model_id.empty()) {
            needed_models[strip_extension_and_lower(soldier.model_id)] = true;
        }
    }
    needed_models["000_01_1"] = true;
    needed_models["003_01_1"] = true;
    needed_models["013_01_1"] = true;
    needed_models["015_01_1"] = true;
    needed_models["019_01_1"] = true;

    const std::filesystem::path unpacked_models_dir = level_root_ / "MODELS" / "models";
    const std::filesystem::path unpacked_textures_dir = level_root_ / "TEXTURES" / "textures";

    // Index unpacked files case-insensitively
    std::unordered_map<std::string, std::filesystem::path> model_file_index;
    if (std::filesystem::exists(unpacked_models_dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(unpacked_models_dir)) {
            if (entry.is_regular_file()) {
                model_file_index[strip_extension_and_lower(entry.path().filename().string())] =
                    entry.path();
            }
        }
    }

    std::unordered_map<std::string, std::filesystem::path> texture_file_index;
    if (std::filesystem::exists(unpacked_textures_dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(unpacked_textures_dir)) {
            if (entry.is_regular_file()) {
                texture_file_index[strip_extension_and_lower(entry.path().filename().string())] =
                    entry.path();
            }
        }
    }

    std::unordered_map<std::string, bool> needed_textures;

    for (const auto& [model_key, _] : needed_models) {
        auto file_it = model_file_index.find(model_key);
        if (file_it == model_file_index.end()) {
            continue;
        }

        try {
            const ParsedGeometry parsed = ParseMefFile(file_it->second.string());
            ModelMesh mesh{};
            mesh.model_id = model_key;

            // Convert vertices to raw world units (4096.0 = 1m)
            mesh.vertices.reserve(parsed.vertices.size());
            mesh.min_bounds = {1.0e9f, 1.0e9f, 1.0e9f};
            mesh.max_bounds = {-1.0e9f, -1.0e9f, -1.0e9f};
            float max_r_sq = 0.0f;

            for (const RenderVertex& rv : parsed.vertices) {
                MeshVertex mv{};
                // Note: ParseMefFile divides rv.pos by 40.96f (with bone offsets baked in)
                mv.position = {
                    rv.pos.x * 40.96f,
                    rv.pos.y * 40.96f,
                    rv.pos.z * 40.96f,
                };
                mv.normal = {rv.normal.x, rv.normal.y, rv.normal.z};
                mv.u = rv.uv.x;
                mv.v = rv.uv.y;
                mv.bone_index = rv.boneIndex;
                mesh.vertices.push_back(mv);

                mesh.min_bounds.x = std::min(mesh.min_bounds.x, mv.position.x);
                mesh.min_bounds.y = std::min(mesh.min_bounds.y, mv.position.y);
                mesh.min_bounds.z = std::min(mesh.min_bounds.z, mv.position.z);
                mesh.max_bounds.x = std::max(mesh.max_bounds.x, mv.position.x);
                mesh.max_bounds.y = std::max(mesh.max_bounds.y, mv.position.y);
                mesh.max_bounds.z = std::max(mesh.max_bounds.z, mv.position.z);

                const float r_sq =
                    mv.position.x * mv.position.x +
                    mv.position.y * mv.position.y +
                    mv.position.z * mv.position.z;
                if (r_sq > max_r_sq) {
                    max_r_sq = r_sq;
                }
            }

            mesh.triangles = parsed.triangles;
            for (const auto& rb : parsed.renderBlocks) {
                mesh.sub_blocks.push_back({
                    rb.triangleStart,
                    rb.triangleCount,
                    rb.materialSlot,
                    rb.opacity,
                });
            }

            build_collision_triangles(
                parsed.xtvcVerts,
                parsed.ecfcFaces,
                parsed.tamcRecords,
                mesh.collision_triangles_set0);
            build_collision_triangles(
                parsed.xtvcVerts1,
                parsed.ecfcFaces1,
                parsed.tamcRecords1,
                mesh.collision_triangles_set1);

            for (const CollisionTriangle& ctri : mesh.collision_triangles_set0) {
                for (const Vector3& pt : {ctri.v0, ctri.v1, ctri.v2}) {
                    const float r_sq = pt.x * pt.x + pt.y * pt.y + pt.z * pt.z;
                    if (r_sq > max_r_sq) {
                        max_r_sq = r_sq;
                    }
                }
            }
            mesh.bounding_radius = std::sqrt(max_r_sq);

            const std::vector<glm::vec3> rest_positions =
                ComputeBoneWorldPositions(parsed.bones);
            mesh.bones.reserve(parsed.bones.size());
            for (std::size_t bi = 0; bi < parsed.bones.size(); ++bi) {
                MeshBone bone{};
                bone.name = parsed.bones[bi].name;
                bone.parent = parsed.bones[bi].parent;
                bone.pivot = {
                    parsed.bones[bi].pivot.x,
                    parsed.bones[bi].pivot.y,
                    parsed.bones[bi].pivot.z,
                };
                if (bi < rest_positions.size()) {
                    bone.rest_world_position = {
                        rest_positions[bi].x,
                        rest_positions[bi].y,
                        rest_positions[bi].z,
                    };
                }
                mesh.bones.push_back(std::move(bone));
            }

            auto mtp_it = mtp_model_textures_.find(model_key);
            if (mtp_it != mtp_model_textures_.end()) {
                mesh.slot_texture_names = mtp_it->second;
            } else {
                for (const std::string& ptex : parsed.pmtlTextures) {
                    mesh.slot_texture_names.push_back(strip_extension_and_lower(ptex));
                }
            }

            for (const std::string& tname : mesh.slot_texture_names) {
                if (!tname.empty()) {
                    needed_textures[tname] = true;
                }
            }

            models_.emplace(model_key, std::move(mesh));
        } catch (...) {
            // Skip malformed model file without failing level load
        }
    }

    // Load referenced .tex files from unpacked_textures_dir
    for (const auto& [tex_key, _] : needed_textures) {
        auto tex_it = texture_file_index.find(tex_key);
        if (tex_it == texture_file_index.end()) {
            continue;
        }
        const TEXFile parsed_tex = TEX_Parse(tex_it->second.string());
        if (parsed_tex.valid && !parsed_tex.images.empty()) {
            textures_.emplace(tex_key, decode_tex_image(tex_key, parsed_tex.images.front()));
        }
    }

    // Also extract any still-missing needed textures from location0.res / COMMON.RES
    const std::array<std::filesystem::path, 3> res_archives = {
        level_root_ / "TEXTURES" / (level_root_.filename().string() + ".RES"),
        level_root_.parent_path() / "COMMON" / "TEXTURES" / "location0.res",
        game_root_ / "COMMON" / "TEXTURES" / "TEXTURES.RES",
    };
    const std::filesystem::path temp_tex_path =
        std::filesystem::temp_directory_path() / "igi_temp_extract.tex";
    for (const auto& res_path : res_archives) {
        if (!std::filesystem::exists(res_path)) {
            continue;
        }
        std::string res_err;
        (void)RES_ForEachEntry(
            res_path.string(),
            [&](const std::string& entry_name, const std::uint8_t* data, std::size_t size) {
                const std::string key = strip_extension_and_lower(entry_name);
                if (needed_textures.find(key) == needed_textures.end() ||
                    textures_.find(key) != textures_.end()) {
                    return;
                }
                std::ofstream out(temp_tex_path, std::ios::binary);
                if (!out) {
                    return;
                }
                out.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
                out.close();
                const TEXFile parsed_tex = TEX_Parse(temp_tex_path.string());
                if (parsed_tex.valid && !parsed_tex.images.empty()) {
                    textures_.emplace(key, decode_tex_image(key, parsed_tex.images.front()));
                }
            },
            res_err);
    }
    std::error_code ec;
    std::filesystem::remove(temp_tex_path, ec);
}

void MissionWorld::load_terrain_octree_and_heightmaps() {
    ctr_bytes_.clear();
    cmd_bytes_.clear();
    terrain_tiles_.clear();

    const std::filesystem::path terrain_dir = level_root_ / "TERRAIN";
    try {
        if (!read_binary_file(terrain_dir / "TERRAIN.CTR", ctr_bytes_)) {
            IGI_LOG_WARN("Asset", "Missing or empty TERRAIN.CTR at " << terrain_dir);
        } else {
            IGI_LOG_DEBUG("Asset", "Loaded TERRAIN.CTR (" << ctr_bytes_.size() << " bytes)");
        }
        if (!read_binary_file(terrain_dir / "TERRAIN.CMD", cmd_bytes_)) {
            IGI_LOG_WARN("Asset", "Missing or empty TERRAIN.CMD at " << terrain_dir);
        } else {
            IGI_LOG_DEBUG("Asset", "Loaded TERRAIN.CMD (" << cmd_bytes_.size() << " bytes)");
        }

        // Exact IGI 1 .hmp structure from project-igi-converter (converters/native/terrain_data.cpp):
        // 256 twelve-byte table descriptors (3072 bytes), followed by grid sample payloads.
        std::vector<std::uint8_t> hmp_bytes;
        if (read_binary_file(terrain_dir / "TERRAIN.HMP", hmp_bytes) && hmp_bytes.size() >= 256 * 12) {
            constexpr std::size_t kTableSize = 256 * 12;
            std::size_t position = kTableSize;
            std::size_t loaded_grids = 0;
            for (std::size_t index = 0; index < 256; ++index) {
                std::uint32_t size = 0;
                std::memcpy(&size, hmp_bytes.data() + index * 12 + 8, 4);
                if (size == 0) {
                    continue;
                }
                if (size > 4096) {
                    IGI_LOG_WARN("Asset", "Invalid HMP grid size " << size << " at index " << index);
                    break;
                }
                const std::size_t length = static_cast<std::size_t>(size + 1) * (size + 1);
                if (position + length > hmp_bytes.size()) {
                    IGI_LOG_WARN("Asset", "HMP payload truncated at grid " << index);
                    break;
                }
                for (HeightMapPatch& patch : height_maps_) {
                    if (patch.bitmap_id == static_cast<int>(index)) {
                        patch.grid_size = static_cast<int>(size);
                        patch.samples.resize(length);
                        std::memcpy(patch.samples.data(), hmp_bytes.data() + position, length);
                        ++loaded_grids;
                    }
                }
                position += length;
            }
            IGI_LOG_DEBUG("Asset", "Loaded " << loaded_grids << " HMP heightmap patches from TERRAIN.HMP");
        }

        const std::filesystem::path terrain_tex_path = terrain_dir / "TERRAIN.TEX";
        if (std::filesystem::exists(terrain_tex_path)) {
            const TEXFile tfile = TEX_Parse(terrain_tex_path.string());
            if (tfile.valid) {
                for (std::size_t i = 0; i < tfile.images.size(); ++i) {
                    terrain_tiles_.push_back(
                        decode_tex_image("terrain_" + std::to_string(i), tfile.images[i]));
                }
                IGI_LOG_DEBUG("Asset", "Loaded " << terrain_tiles_.size() << " terrain tiles from TERRAIN.TEX");
            } else {
                IGI_LOG_WARN("Asset", "Failed to parse TERRAIN.TEX: " << tfile.error);
            }
        }
    } catch (const std::exception& ex) {
        IGI_LOG_ERROR("Asset", "Exception in load_terrain_octree_and_heightmaps: " << ex.what());
    }
}

bool MissionWorld::sample_terrain_height(
    double world_x,
    double world_y,
    float& out_ground_z,
    Vector3& out_ground_normal) const noexcept {
    return sample_terrain_height(
        world_x,
        world_y,
        -std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity(),
        out_ground_z,
        out_ground_normal);
}

bool MissionWorld::sample_terrain_height(
    double world_x,
    double world_y,
    double minimum_ground_z,
    double maximum_ground_z,
    float& out_ground_z,
    Vector3& out_ground_normal) const noexcept {
    // Retail TerrainPointDescent (0x4C9A30) + TerrainSurfaceTest (0x4CB050)
    if (minimum_ground_z > maximum_ground_z ||
        ctr_bytes_.size() < sizeof(ctr_item_s) * 2 ||
        cmd_bytes_.size() < sizeof(cmd_item_s)) {
        return false;
    }

    const auto* nodes = reinterpret_cast<const ctr_item_s*>(ctr_bytes_.data());
    const std::size_t node_count = ctr_bytes_.size() / sizeof(ctr_item_s);

    const std::uint32_t query_x =
        static_cast<std::uint32_t>(static_cast<std::int32_t>(std::floor(world_x))) ^
        0x40000000u;
    const std::uint32_t query_y =
        static_cast<std::uint32_t>(static_cast<std::int32_t>(std::floor(world_y))) ^
        0x40000000u;
    const double query_reference_z = std::isfinite(maximum_ground_z)
        ? maximum_ground_z
        : player_spawn_position_.z;
    const std::uint32_t query_z = static_cast<std::uint32_t>(
        static_cast<std::int32_t>(std::floor(query_reference_z))) ^ 0x40000000u;

    std::int32_t node_x = 0;
    std::int32_t node_y = 0;
    std::int32_t node_z = 0;
    std::uint8_t orientation = 0;
    std::uint32_t extent = 0x40000000u;
    int shift = 30;
    int node_index = 1;

    int best_surface_node = 0;
    std::int32_t best_node_x = 0;
    std::int32_t best_node_y = 0;
    std::int32_t best_node_z = 0;
    std::uint8_t best_orientation = 0;
    std::uint32_t best_extent = 0;

    for (int level = 1; level <= 16; ++level) {
        if (node_index <= 0 || static_cast<std::size_t>(node_index) >= node_count) {
            break;
        }
        const ctr_item_s& node = nodes[node_index];
        if (node.cmd_offset_ + sizeof(cmd_item_s) <= cmd_bytes_.size()) {
            cmd_item_s hdr{};
            std::memcpy(&hdr, cmd_bytes_.data() + node.cmd_offset_, sizeof(cmd_item_s));
            if (hdr.num_triangle_ > 0) {
                best_surface_node = node_index;
                best_node_x = node_x;
                best_node_y = node_y;
                best_node_z = node_z;
                best_orientation = orientation;
                best_extent = extent;
            }
        }

        if (level == 16) {
            break;
        }

        const int x_bit = static_cast<int>((query_x >> shift) & 1u);
        const int y_bit = static_cast<int>((query_y >> shift) & 1u);
        const int z_bit = static_cast<int>((query_z >> shift) & 1u);
        const int octant = (z_bit << 2) | (y_bit << 1) | x_bit;
        const std::uint8_t slot = kTerrainSlotTable[(orientation & 7u) * 8u + octant];
        const std::uint8_t child_mask_bit =
            static_cast<std::uint8_t>(1u << slot);
        if ((node.children_mask_ & child_mask_bit) == 0) {
            break;
        }

        const std::int16_t child = node.children_[slot];
        if (child <= 0) {
            break;
        }

        const std::uint32_t half_extent = extent >> 1;
        node_x += static_cast<std::int32_t>((x_bit != 0) ? half_extent : -half_extent);
        node_y += static_cast<std::int32_t>((y_bit != 0) ? half_extent : -half_extent);
        node_z += static_cast<std::int32_t>((z_bit != 0) ? half_extent : -half_extent);
        orientation = compose_terrain_orientation(
            orientation,
            static_cast<std::uint8_t>(node.cmd_transform_[slot] & 7));
        extent = half_extent;
        --shift;
        node_index = child;
    }

    if (best_surface_node <= 0) {
        return false;
    }

    const ctr_item_s& surf_node = nodes[best_surface_node];
    if (surf_node.cmd_offset_ + sizeof(cmd_item_s) > cmd_bytes_.size()) {
        return false;
    }

    cmd_item_s hdr{};
    std::memcpy(&hdr, cmd_bytes_.data() + surf_node.cmd_offset_, sizeof(cmd_item_s));
    const std::size_t tri_offset = surf_node.cmd_offset_ + sizeof(cmd_item_s);
    const std::size_t vert_offset = tri_offset + static_cast<std::size_t>(hdr.vertex_offset_);
    const std::size_t total_verts =
        static_cast<std::size_t>(hdr.num_parent_vertex_) +
        static_cast<std::size_t>(hdr.num_child_vertex_);

    if (vert_offset + total_verts * 4 > cmd_bytes_.size()) {
        return false;
    }

    const double unit_scale = static_cast<double>(best_extent) / 24.0;

    // Decode vertices in world space with HeightMap displacement
    std::array<Vector3, 256> world_verts{};
    const std::size_t clamped_verts = std::min<std::size_t>(total_verts, 256);
    for (std::size_t vi = 0; vi < clamped_verts; ++vi) {
        std::uint32_t packed_v = 0;
        std::memcpy(&packed_v, cmd_bytes_.data() + vert_offset + vi * 4, 4);
        const int vx = static_cast<int>((packed_v >> 26) & 0x3Fu) - 24;
        const int vy = static_cast<int>((packed_v >> 20) & 0x3Fu) - 24;
        const int vz = static_cast<int>((packed_v >> 14) & 0x3Fu) - 24;

        double lx = static_cast<double>(vx) * unit_scale;
        double ly = static_cast<double>(vy) * unit_scale;
        const double lz = static_cast<double>(vz) * unit_scale;
        transform_terrain_local_xy(best_orientation, lx, ly);

        const double wx = static_cast<double>(best_node_x) + lx;
        const double wy = static_cast<double>(best_node_y) + ly;
        double wz = static_cast<double>(best_node_z) + lz;

        // Apply HeightMap displacement if inside any HeightMap patch
        for (const HeightMapPatch& patch : height_maps_) {
            if (patch.samples.empty() || patch.grid_size <= 0) {
                continue;
            }
            const double cell_world_size = static_cast<double>(1u << (31 - patch.level));
            const double patch_span = cell_world_size * static_cast<double>(patch.grid_size);
            if (wx >= patch.world_x && wx <= patch.world_x + patch_span &&
                wy >= patch.world_y && wy <= patch.world_y + patch_span) {
                const double gx = (wx - patch.world_x) / cell_world_size;
                const double gy = (wy - patch.world_y) / cell_world_size;
                const int ix = std::clamp(static_cast<int>(gx), 0, patch.grid_size - 1);
                const int iy = std::clamp(static_cast<int>(gy), 0, patch.grid_size - 1);
                const double fx = std::clamp(gx - static_cast<double>(ix), 0.0, 1.0);
                const double fy = std::clamp(gy - static_cast<double>(iy), 0.0, 1.0);
                const int stride = patch.grid_size + 1;
                const double s00 = patch.samples[iy * stride + ix];
                const double s10 = patch.samples[iy * stride + (ix + 1)];
                const double s01 = patch.samples[(iy + 1) * stride + ix];
                const double s11 = patch.samples[(iy + 1) * stride + (ix + 1)];
                const double s0 = s00 + (s10 - s00) * fx;
                const double s1 = s01 + (s11 - s01) * fx;
                const double sample = s0 + (s1 - s0) * fy;
                wz += (sample - 64.0) * 256.0;
                break;
            }
        }

        world_verts[vi] = {
            static_cast<float>(wx - best_node_x),
            static_cast<float>(wy - best_node_y),
            static_cast<float>(wz),
        };
    }

    const float rel_qx = static_cast<float>(world_x - best_node_x);
    const float rel_qy = static_cast<float>(world_y - best_node_y);

    for (std::uint16_t ti = 0; ti < hdr.num_triangle_; ++ti) {
        std::uint32_t packed_t = 0;
        std::memcpy(&packed_t, cmd_bytes_.data() + tri_offset + ti * 4, 4);
        const std::uint8_t i0 = static_cast<std::uint8_t>(packed_t & 0xFFu);
        const std::uint8_t i1 = static_cast<std::uint8_t>((packed_t >> 8) & 0xFFu);
        const std::uint8_t i2 = static_cast<std::uint8_t>((packed_t >> 16) & 0xFFu);
        if (i0 >= clamped_verts || i1 >= clamped_verts || i2 >= clamped_verts) {
            continue;
        }

        const Vector3& a = world_verts[i0];
        const Vector3& b = world_verts[i1];
        const Vector3& c = world_verts[i2];

        // 2D barycentric test in XY
        const float v0x = b.x - a.x;
        const float v0y = b.y - a.y;
        const float v1x = c.x - a.x;
        const float v1y = c.y - a.y;
        const float v2x = rel_qx - a.x;
        const float v2y = rel_qy - a.y;
        const float denom = v0x * v1y - v1x * v0y;
        if (std::fabs(denom) <= 1.0e-5f) {
            continue;
        }
        const float inv_denom = 1.0f / denom;
        const float u = (v2x * v1y - v1x * v2y) * inv_denom;
        const float v = (v0x * v2y - v2x * v0y) * inv_denom;
        if (u >= -0.02f && v >= -0.02f && (u + v) <= 1.02f) {
            const float candidate_ground_z =
                a.z + u * (b.z - a.z) + v * (c.z - a.z);
            if (candidate_ground_z < minimum_ground_z ||
                candidate_ground_z > maximum_ground_z) {
                continue;
            }

            out_ground_z = candidate_ground_z;
            float nx = (b.y - a.y) * (c.z - a.z) - (b.z - a.z) * (c.y - a.y);
            float ny = (b.z - a.z) * (c.x - a.x) - (b.x - a.x) * (c.z - a.z);
            float nz = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
            if (nz < 0.0f) {
                nx = -nx;
                ny = -ny;
                nz = -nz;
            }
            const float nlen = std::sqrt(nx * nx + ny * ny + nz * nz);
            if (nlen > 1.0e-5f) {
                out_ground_normal = {nx / nlen, ny / nlen, nz / nlen};
            } else {
                out_ground_normal = {0.0f, 0.0f, 1.0f};
            }
            return true;
        }
    }

    return false;
}

void MissionWorld::extract_nearby_terrain_mesh(
    double center_x,
    double center_y,
    double radius) {
    terrain_triangles_.clear();
    const double step = 16384.0; // 4m grid cells matching depth-16 terrain leaf spacing
    const int half_steps = static_cast<int>(radius / step);
    const double base_x = std::floor(center_x / step) * step;
    const double base_y = std::floor(center_y / step) * step;

    const int grid_dim = half_steps * 2 + 1;
    std::vector<float> heights(static_cast<std::size_t>(grid_dim * grid_dim), 0.0f);
    std::vector<Vector3> normals(
        static_cast<std::size_t>(grid_dim * grid_dim),
        {0.0f, 0.0f, 1.0f});
    std::vector<bool> valid(static_cast<std::size_t>(grid_dim * grid_dim), false);

    for (int gy = 0; gy < grid_dim; ++gy) {
        for (int gx = 0; gx < grid_dim; ++gx) {
            const double wx = base_x + static_cast<double>(gx - half_steps) * step;
            const double wy = base_y + static_cast<double>(gy - half_steps) * step;
            float gz = 0.0f;
            Vector3 gn{0.0f, 0.0f, 1.0f};
            const std::size_t idx = static_cast<std::size_t>(gy * grid_dim + gx);
            if (sample_terrain_height(wx, wy, gz, gn)) {
                heights[idx] = gz;
                normals[idx] = gn;
                valid[idx] = true;
            }
        }
    }

    terrain_triangles_.reserve(static_cast<std::size_t>((grid_dim - 1) * (grid_dim - 1) * 2));
    const int tile_count = std::max(1, static_cast<int>(terrain_tiles_.size()));
    for (int gy = 0; gy < grid_dim - 1; ++gy) {
        for (int gx = 0; gx < grid_dim - 1; ++gx) {
            const std::size_t i00 = static_cast<std::size_t>(gy * grid_dim + gx);
            const std::size_t i10 = static_cast<std::size_t>(gy * grid_dim + (gx + 1));
            const std::size_t i01 = static_cast<std::size_t>((gy + 1) * grid_dim + gx);
            const std::size_t i11 = static_cast<std::size_t>((gy + 1) * grid_dim + (gx + 1));
            if (!valid[i00] || !valid[i10] || !valid[i01] || !valid[i11]) {
                continue;
            }

            const float x0 = static_cast<float>(base_x + static_cast<double>(gx - half_steps) * step);
            const float x1 = static_cast<float>(base_x + static_cast<double>(gx + 1 - half_steps) * step);
            const float y0 = static_cast<float>(base_y + static_cast<double>(gy - half_steps) * step);
            const float y1 = static_cast<float>(base_y + static_cast<double>(gy + 1 - half_steps) * step);

            const int tile_idx = ((gx + gy) & 0x7FFFFFFF) % tile_count;

            TerrainTriangle t0{};
            t0.v0 = {x0, y0, heights[i00]};
            t0.v1 = {x1, y0, heights[i10]};
            t0.v2 = {x1, y1, heights[i11]};
            t0.normal = normals[i00];
            t0.u0 = 0.0f; t0.v0_uv = 0.0f;
            t0.u1 = 1.0f; t0.v1_uv = 0.0f;
            t0.u2 = 1.0f; t0.v2_uv = 1.0f;
            t0.tile_index = tile_idx;
            terrain_triangles_.push_back(t0);

            TerrainTriangle t1{};
            t1.v0 = {x0, y0, heights[i00]};
            t1.v1 = {x1, y1, heights[i11]};
            t1.v2 = {x0, y1, heights[i01]};
            t1.normal = normals[i01];
            t1.u0 = 0.0f; t1.v0_uv = 0.0f;
            t1.u1 = 1.0f; t1.v1_uv = 1.0f;
            t1.u2 = 0.0f; t1.v2_uv = 1.0f;
            t1.tile_index = tile_idx;
            terrain_triangles_.push_back(t1);
        }
    }
}

void MissionWorld::load_animation_banks() {
    animation_banks_.clear();
    const std::filesystem::path anim_dir = game_root_ / "COMMON" / "ANIMS";
    for (int id : {0, 1, 2, 3}) {
        char filename[16];
        std::snprintf(filename, sizeof(filename), "%03d.IFF", id);
        const std::filesystem::path iff_path = anim_dir / filename;
        if (!std::filesystem::exists(iff_path)) {
            continue;
        }
        const IffFile iff = IFF_Parse(iff_path.string());
        if (!iff.valid) {
            continue;
        }
        SkeletalAnimationBank bank{};
        bank.hierarchy_id = id;
        bank.parents.assign(iff.skeleton.parents.begin(), iff.skeleton.parents.end());
        const std::size_t bone_count = iff.skeleton.translations.size() / 3;
        bank.bone_translations.reserve(bone_count);
        for (std::size_t b = 0; b < bone_count; ++b) {
            bank.bone_translations.push_back({
                iff.skeleton.translations[b * 3 + 0] * 40.96f,
                iff.skeleton.translations[b * 3 + 1] * 40.96f,
                iff.skeleton.translations[b * 3 + 2] * 40.96f,
            });
        }
        for (const IffClip& clip : iff.clips) {
            SkeletalClip sclip{};
            sclip.animation_id = static_cast<int>(clip.animation_id);
            sclip.duration = std::max(0.05f, clip.duration);
            sclip.bone_rotations.resize(clip.bone_rotations.size());
            for (std::size_t b = 0; b < clip.bone_rotations.size(); ++b) {
                for (const IffRotationKey& rk : clip.bone_rotations[b]) {
                    sclip.bone_rotations[b].push_back({
                        rk.rot[0], rk.rot[1], rk.rot[2], rk.rot[3], rk.time,
                    });
                }
            }
            bank.clips.push_back(std::move(sclip));
        }
        animation_banks_.emplace(id, std::move(bank));
    }
}

void MissionWorld::evaluate_skeletal_pose(
    int hierarchy_id,
    int clip_id,
    float time_seconds,
    const ModelMesh& mesh,
    std::vector<Vector3>& out_bone_world_offsets) const noexcept {
    out_bone_world_offsets.assign(mesh.bones.size(), {0.0f, 0.0f, 0.0f});
    if (mesh.bones.empty()) {
        return;
    }

    auto bank_it = animation_banks_.find(hierarchy_id);
    if (bank_it == animation_banks_.end() || bank_it->second.clips.empty()) {
        return;
    }
    const SkeletalAnimationBank& bank = bank_it->second;
    const SkeletalClip* chosen_clip = &bank.clips.front();
    for (const SkeletalClip& clip : bank.clips) {
        if (clip.animation_id == clip_id) {
            chosen_clip = &clip;
            break;
        }
    }

    const float wrapped_time = std::fmod(std::max(0.0f, time_seconds), chosen_clip->duration);
    const float phase = ( wrapped_time / chosen_clip->duration ) * 6.2831853f;

    // Evaluate hierarchical bone displacements relative to rest pose
    for (std::size_t bi = 0; bi < mesh.bones.size(); ++bi) {
        Vector3 delta{};
        if (bi < chosen_clip->bone_rotations.size() &&
            !chosen_clip->bone_rotations[bi].empty()) {
            const auto& keys = chosen_clip->bone_rotations[bi];
            const SkeletalKeyRotation& k = keys.front();
            // Apply subtle cyclic stride/breathing motion modulated by IFF key quaternion
            const float swing = std::sin(phase + static_cast<float>(bi) * 0.45f);
            delta.x = k.qx * 85.0f * swing;
            delta.y = k.qy * 85.0f * swing;
            delta.z = k.qz * 45.0f * std::cos(phase);
        }
        const int parent = mesh.bones[bi].parent;
        if (parent >= 0 && static_cast<std::size_t>(parent) < bi) {
            delta.x += out_bone_world_offsets[static_cast<std::size_t>(parent)].x * 0.35f;
            delta.y += out_bone_world_offsets[static_cast<std::size_t>(parent)].y * 0.35f;
            delta.z += out_bone_world_offsets[static_cast<std::size_t>(parent)].z * 0.35f;
        }
        out_bone_world_offsets[bi] = delta;
    }
}

const ModelMesh* MissionWorld::find_model(const std::string& model_id) const noexcept {
    const std::string key = strip_extension_and_lower(model_id);
    const auto it = models_.find(key);
    return (it != models_.end()) ? &it->second : nullptr;
}

const TextureImage* MissionWorld::find_texture(const std::string& texture_name) const noexcept {
    const std::string key = strip_extension_and_lower(texture_name);
    const auto it = textures_.find(key);
    return (it != textures_.end()) ? &it->second : nullptr;
}

const TextureImage* MissionWorld::terrain_tile_texture(int tile_index) const noexcept {
    if (terrain_tiles_.empty()) {
        return nullptr;
    }
    const std::size_t idx =
        static_cast<std::size_t>(tile_index >= 0 ? tile_index : -tile_index) %
        terrain_tiles_.size();
    return &terrain_tiles_[idx];
}

} // namespace igi::gameplay
