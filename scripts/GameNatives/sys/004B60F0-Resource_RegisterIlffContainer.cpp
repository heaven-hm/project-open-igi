// IGI native provenance: Resource_RegisterIlffContainer - 0x004B60F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B60F0-Resource_RegisterIlffContainer.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B60F0-Resource_RegisterIlffContainer.hpp"
void *Resource_RegisterIlffContainer(void *context, void *arg2)
{
    ResourceNode *node = Resource_CreateNode(context);
    node->container = arg2;
    return node;
}
