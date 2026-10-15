#include "igi/game_assets.hpp"

#include "graph_parser.h"
#include "res_parser.h"
#include "qvm_parser.h"

#include <algorithm>
#include <cctype>
#include <system_error>

namespace igi::assets {
namespace {

std::string lower_extension(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
        [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return extension;
}

bool has_path_component(const std::filesystem::path& path, const std::string& expected) {
    for (const auto& component : path) {
        std::string value = component.string();
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
        if (value == expected) return true;
    }
    return false;
}

bool is_within(const std::filesystem::path& child, const std::filesystem::path& parent) {
    const auto relative = child.lexically_relative(parent);
    return !relative.empty() && *relative.begin() != "..";
}

} // namespace

GameAssetSummary GameAssetCatalog::scan(const std::filesystem::path& root, int mission) {
    GameAssetSummary summary;
    summary.game_root = root;
    if (!std::filesystem::is_directory(root)) {
        summary.error = "Game folder does not exist: " + root.string();
        return summary;
    }

    std::vector<std::filesystem::path> paths;
    std::error_code ec;
    for (std::filesystem::recursive_directory_iterator it(
             root, std::filesystem::directory_options::skip_permission_denied, ec), end;
         it != end; it.increment(ec)) {
        if (ec) { ec.clear(); continue; }
        if (it->is_regular_file(ec)) paths.push_back(it->path());
    }
    std::sort(paths.begin(), paths.end());

    const std::string level_name = "LEVEL" + std::to_string(mission);
    for (const auto& path : paths) {
        for (auto parent = path.parent_path(); parent != root && !parent.empty(); parent = parent.parent_path()) {
            std::string component = parent.filename().string();
            std::transform(component.begin(), component.end(), component.begin(),
                [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
            if (component == level_name) {
                summary.level_root = parent;
                break;
            }
        }
        if (!summary.level_root.empty()) break;
    }

    for (const auto& path : paths) {
        const auto extension = lower_extension(path);
        if (extension == ".qvm") ++summary.qvm_files;
        if (extension == ".tex" || extension == ".spr" || extension == ".pic") ++summary.texture_files;

        if (extension == ".dat" && has_path_component(path, "GRAPHS") &&
            !summary.level_root.empty() && is_within(path, summary.level_root)) {
            auto graph = GRAPH_Parse(path.string());
            if (!graph.valid) continue;
            ++summary.graph_files;
            if (summary.navigation_nodes.empty()) {
                summary.navigation_nodes.reserve(graph.nodes.size());
                for (const auto& node : graph.nodes)
                    summary.navigation_nodes.push_back({node.id, node.x, node.y, node.z, node.link1, node.link2});
            }
        }

        if (extension != ".res") continue;
        std::string error;
        const bool valid = RES_ForEachEntry(path.string(),
            [&](const std::string& name, const uint8_t*, size_t) {
                ++summary.archived_resources;
                const auto resource_ext = lower_extension(std::filesystem::path(name));
                ++summary.resources_by_extension[resource_ext.empty() ? "<none>" : resource_ext];
                if (resource_ext == ".mef") ++summary.model_resources;
            }, error);
        if (valid) ++summary.archives_scanned;
    }

    if (summary.level_root.empty())
        summary.error = "Mission folder not found: " + level_name;
    return summary;
}

} // namespace igi::assets
