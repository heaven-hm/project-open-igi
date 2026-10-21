#pragma once

#include <cstdint>

namespace igi::retail::render {

// Native: Render_GetActiveBackendId() - VA 0x004CEA80 - complete port.
// Previous catalog alias: DynObj_InsertHashItem_13.
// Returns the active render-backend id stored in global 0x00548208. The retail registrar
// (Display_Transcontextprimitiv, 0x00497980) assigns this id through Sys_RendermodeFailedToCr_2;
// that registrar is not ported yet, so the id is settable by the host wiring only.
[[nodiscard]] std::int32_t Render_GetActiveBackendId();
inline std::int32_t DynObj_InsertHashItem_13() { return Render_GetActiveBackendId(); }

// Host wiring for the port-owned 0x00548208 storage. Not a retail entry point.
void Render_SetBackendId(std::int32_t backend_id);

} // namespace igi::retail::render
