// IGI native provenance: Render_TransformVertices_12 - 0x004DA160 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DA160-Render_TransformVertices_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004DA160-Render_TransformVertices_12.hpp"
int Render_TransformVertices_12(int* context)

{
    DynObj_RegisterSymbol_9(context[0x1a], context);
    return QTask_FindType(context);
}
