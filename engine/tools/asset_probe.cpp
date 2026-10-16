#include "igi/game_assets.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: igi_asset_probe <game-folder> [mission-number]\n";
        return 2;
    }

    int mission = 1;
    if (argc == 3) {
        char* end = nullptr;
        const long parsed = std::strtol(argv[2], &end, 10);
        if (!end || *end != '\0' || parsed < 1 || parsed > 99) {
            std::cerr << "Mission number must be between 1 and 99.\n";
            return 2;
        }
        mission = static_cast<int>(parsed);
    }

    const auto summary = igi::assets::GameAssetCatalog::scan(argv[1], mission);
    if (!summary.error.empty()) {
        std::cerr << summary.error << '\n';
        return 1;
    }

    std::cout << "Game root: " << summary.game_root << '\n'
              << "Mission root: " << summary.level_root << '\n'
              << "Archives parsed: " << summary.archives_scanned << '\n'
              << "Archive resources: " << summary.archived_resources << '\n'
              << "QVM files: " << summary.qvm_files << '\n'
              << "Graph files parsed: " << summary.graph_files << '\n'
              << "Standalone texture files: " << summary.texture_files << '\n'
              << "Model resources: " << summary.model_resources << '\n'
              << "Navigation graphs loaded: " << summary.navigation_graphs.size() << '\n';

    std::vector<int> graph_ids;
    graph_ids.reserve(summary.navigation_graphs.size());
    for (const auto& [graph_id, graph] : summary.navigation_graphs) {
        (void)graph;
        graph_ids.push_back(graph_id);
    }
    std::sort(graph_ids.begin(), graph_ids.end());
    for (const int graph_id : graph_ids) {
        const auto& graph = summary.navigation_graphs.at(graph_id);
        std::cout << "  Graph " << graph.id << ": " << graph.nodes.size()
                  << " nodes, " << graph.edges.size() << " edges ("
                  << graph.source_file.filename().string() << ")\n";
    }

    std::vector<std::pair<std::string, std::size_t>> types(
        summary.resources_by_extension.begin(), summary.resources_by_extension.end());
    std::sort(types.begin(), types.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.first < rhs.first;
    });
    for (const auto& [extension, count] : types)
        std::cout << "  " << extension << ": " << count << '\n';
    return 0;
}
