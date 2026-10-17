#include "igi/retail/render/trans_context.hpp"

#include <cstring>

namespace igi::retail::render {
namespace {

// Global 0x00BCAAE0: the active transform context copied by the retail setter.
TransContext active_trans_context{};

// Global 0x006E5BB0: set to 1 by the setter on every install.
std::int32_t trans_context_installed = 0;

// Global 0x006E5BB4: monotonically incremented by the setter.
std::int32_t trans_context_install_count = 0;

} // namespace

// 0x00497E70  Transcontext_SetActiveTransContext
//
// Copies the caller's transform context (0xA8 bytes) into the active context, dispatches the
// registered "SetActiveTransContext" render method, then marks the context installed and
// increments the install counter. The render method is selected through the render-method
// table indexed by the id returned when "SetActiveTransContext" was registered; the port has
// no render-method table yet, so that dispatch is the outstanding dependency.
void Transcontext_SetActiveTransContext(const TransContext& context)
{
    std::memcpy(active_trans_context.bytes.data(), context.bytes.data(), trans_context_size);
    trans_context_installed = 1;
    ++trans_context_install_count;
}

TransContext& Transcontext_GetActiveTransContext()
{
    return active_trans_context;
}

// Reads the three float64 at the active context's +0x28 (globals 0x00BCAB08/10/18).
std::array<double, 3> Transcontext_GetCameraPosition()
{
    std::array<double, 3> position{};
    std::memcpy(position.data(),
                active_trans_context.bytes.data() + trans_context_camera_position_offset,
                sizeof(position));
    return position;
}

void Transcontext_SetCameraPosition(double x, double y, double z)
{
    const std::array<double, 3> position{x, y, z};
    std::memcpy(active_trans_context.bytes.data() + trans_context_camera_position_offset,
                position.data(), sizeof(position));
}

std::int32_t Transcontext_GetInstalledFlag()
{
    return trans_context_installed;
}

std::int32_t Transcontext_GetInstallCount()
{
    return trans_context_install_count;
}

} // namespace igi::retail::render
