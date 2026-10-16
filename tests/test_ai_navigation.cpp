#include "test_framework.hpp"

#include "igi/ai_system.hpp"
#include "igi/game_assets.hpp"

using namespace igi::test;
using igi::assets::NavigationEdge;
using igi::assets::NavigationGraph;
using igi::assets::NavigationNode;
using igi::gameplay::AiSystem;
using igi::gameplay::NavigationPosition;

namespace {

NavigationGraph make_chain_graph() {
    // 1 - 2 - 3 linear chain via per-node links.
    NavigationGraph graph;
    graph.id = 7;
    NavigationNode n1;
    n1.id = 1;
    n1.x = 0.0;
    n1.link1 = 2;
    NavigationNode n2;
    n2.id = 2;
    n2.x = 1000.0;
    n2.link1 = 1;
    n2.link2 = 3;
    NavigationNode n3;
    n3.id = 3;
    n3.x = 2000.0;
    n3.link1 = 2;
    graph.nodes = {n1, n2, n3};
    return graph;
}

NavigationGraph make_edgelist_graph() {
    // Nodes 10/20/30 connected only through the edge list (no per-node links).
    NavigationGraph graph;
    graph.id = 9;
    for (int id : {10, 20, 30}) {
        NavigationNode node;
        node.id = id;
        node.x = static_cast<double>(id) * 100.0;
        graph.nodes.push_back(node);
    }
    NavigationEdge e1;
    e1.source_node_id = 10;
    e1.destination_node_id = 20;
    NavigationEdge e2;
    e2.source_node_id = 20;
    e2.destination_node_id = 30;
    graph.edges = {e1, e2};
    return graph;
}

} // namespace

TEST_CASE(AiNavigation, ChainRouteFollowsIntermediateNodes) {
    const NavigationGraph graph = make_chain_graph();
    const std::vector<int> route = AiSystem::find_graph_route(graph, 1, 3);
    ASSERT_EQ(route.size(), 3u);
    ASSERT_EQ(route[0], 1);
    ASSERT_EQ(route[1], 2);
    ASSERT_EQ(route[2], 3);
}

TEST_CASE(AiNavigation, ReverseRouteIsSymmetric) {
    const NavigationGraph graph = make_chain_graph();
    const std::vector<int> route = AiSystem::find_graph_route(graph, 3, 1);
    ASSERT_EQ(route.size(), 3u);
    ASSERT_EQ(route[0], 3);
    ASSERT_EQ(route[1], 2);
    ASSERT_EQ(route[2], 1);
}

TEST_CASE(AiNavigation, EdgeListLinksAreTraversed) {
    const NavigationGraph graph = make_edgelist_graph();
    const std::vector<int> route = AiSystem::find_graph_route(graph, 10, 30);
    ASSERT_EQ(route.size(), 3u);
    ASSERT_EQ(route[0], 10);
    ASSERT_EQ(route[1], 20);
    ASSERT_EQ(route[2], 30);
}

TEST_CASE(AiNavigation, UnreachableTargetYieldsEmptyRoute) {
    NavigationGraph graph = make_chain_graph();
    NavigationNode island;
    island.id = 99;
    island.x = 99999.0;
    graph.nodes.push_back(island);
    ASSERT_TRUE(AiSystem::find_graph_route(graph, 1, 99).empty());
    ASSERT_TRUE(AiSystem::find_graph_route(graph, 1, 4242).empty());
}

TEST_CASE(AiNavigation, NearestNodeResolvesByPlanDistance) {
    const NavigationGraph graph = make_chain_graph();
    ASSERT_EQ(AiSystem::nearest_graph_node_id(graph, {100.0, 0.0, 0.0}), 1);
    ASSERT_EQ(AiSystem::nearest_graph_node_id(graph, {1900.0, 50.0, 0.0}), 3);
}

TEST_CASE(AiNavigation, EmptyGraphHasNoNearestNode) {
    const NavigationGraph graph;
    ASSERT_EQ(AiSystem::nearest_graph_node_id(graph, {0.0, 0.0, 0.0}), -1);
}
