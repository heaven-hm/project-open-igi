#include "igi/retail/render/backend.hpp"

namespace igi::retail::render {
namespace {

// Port-owned storage for global 0x00548208.
std::int32_t render_backend_id = -1;

} // namespace

// Native: Render_GetActiveBackendId; retail VA 0x004CEA80; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.
// Behavior/CFG: not restated here.
// Status/limits: STRUCTURALLY_VERIFIED; see verification record for scope and port limits.
// Previous catalog alias: DynObj_InsertHashItem_13.
std::int32_t Render_GetActiveBackendId()
{
    return render_backend_id;
}

void Host_SetRenderBackendId(std::int32_t backend_id)
{
    render_backend_id = backend_id;
}

} // namespace igi::retail::render
