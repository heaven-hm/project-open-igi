#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace igi::assets {

struct NavigationNode {
    int id{};
    double x{}, y{}, z{};
    int link1{-1}, link2{-1};
    float gamma{};
    float radius{};
    int material{};
    std::string criteria;
};

struct NavigationEdge {
    int source_node_id{-1};
    int destination_node_id{-1};
    int link_type{};
};

struct NavigationGraph {
    int id{};
    std::filesystem::path source_file;
    std::vector<NavigationNode> nodes;
    std::vector<NavigationEdge> edges;
};

struct GameAssetSummary {
    std::filesystem::path game_root;
    std::filesystem::path level_root;
    std::size_t archives_scanned{};
    std::size_t archived_resources{};
    std::size_t qvm_files{};
    std::size_t graph_files{};
    std::size_t texture_files{};
    std::size_t model_resources{};
    std::unordered_map<std::string, std::size_t> resources_by_extension;
    std::unordered_map<int, NavigationGraph> navigation_graphs;
    std::string error;
};

// Indexes original retail files without copying the game data into the repo.
// Archive entries are counted as the RES reader streams them, so large level
// archives are not held in memory.
class GameAssetCatalog final {
public:
    [[nodiscard]] static GameAssetSummary scan(
        const std::filesystem::path& game_root, int mission = 1);
};

} // namespace igi::assets
