#include "igi/retail/render/backend.hpp"

namespace igi::retail::render {
namespace {

// Port-owned storage for global 0x00548208.
std::int32_t render_backend_id = -1;

} // namespace

// Native: Render_GetActiveBackendId() - VA 0x004CEA80 - complete port.
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
