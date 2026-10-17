#pragma once

#include "igi/retail/model/mef_model_loader.hpp"
#include <array>

namespace igi::retail::model
{

struct MefRenderVertex
{
    std::array<float, 3> position{};
    std::array<float, 3> normal{};
    std::array<float, 2> texture_coordinates{};
    std::array<float, 2> lightmap_coordinates{};
};

struct MefRenderBatch
{
    std::uint16_t material_index = 0;
    std::uint16_t first_vertex = 0;
    std::uint16_t vertex_count = 0;
    bool translucent = false;
    std::vector<std::uint16_t> indices;
};

struct MefRenderData
{
    std::uint32_t primitive_type = 0;
    std::vector<MefRenderVertex> vertices;
    std::vector<MefRenderBatch> batches;
};

// DisplayRender_DrawModel - 0x0049DF60 - static translation slice; runtime parity unverified.
// Source: IGI.EXE SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Binary Ninja export and radare2 assembly establish 28/32-byte batch headers and linked index lists.
// Vertex upload at 0x0049B4B0 copies 32-byte format-0 and 40-byte format-3 records.
// This helper decodes those fields; it is not a separate original game entry point.
[[nodiscard]] MefRenderData Display_DecodeMeshRenderData(const MefModel &model);

} // namespace igi::retail::model
