// IGI native provenance: Terrain_TraverseContourQuadtree - 0x004D14E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D14E0-Terrain_TraverseContourQuadtree.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D14E0-Terrain_TraverseContourQuadtree.hpp"
int Terrain_TraverseContourQuadtree(void *context)
{
    ContourQuadtree *tree = g_a44344;
    int visited = 0;
    Contour_VisitNode(tree, &visited);
    return visited;
}
