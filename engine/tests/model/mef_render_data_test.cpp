#include "igi/retail/model/mef_render_data.hpp"
#include "igi/retail/resource/resource_archive.hpp"
#include "igi/retail/texture/model_texture_pixels.hpp"
#include "igi/retail/script/qvm_loader.hpp"
#include "igi/retail/script/qvm_object_calls.hpp"
#include <iostream>
#include <stdexcept>

namespace
{
void require(bool condition, const char *message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}
void write_short(std::vector<std::byte> &bytes, std::size_t offset, std::uint16_t value)
{
    bytes.at(offset) = static_cast<std::byte>(value & 255U);
    bytes.at(offset + 1U) = static_cast<std::byte>(value >> 8U);
}
igi::retail::model::MefModel fixture()
{
    using namespace igi::retail::model;
    MefModel model;
    model.render_header = MefRenderHeader{3U, 1U, 0U, 0U, 0U};
    IlffChunk header;
    header.tag = mef_tag::d3d_render_header;
    header.payload.resize(48U);
    header.payload[0] = std::byte{4};
    IlffChunk vertices;
    vertices.tag = mef_tag::vertices;
    vertices.payload.resize(96U);
    IlffChunk batch;
    batch.tag = mef_tag::draw_records;
    batch.payload.resize(34U);
    write_short(batch.payload, 12U, 3U);
    write_short(batch.payload, 14U, 0xFFFFU);
    write_short(batch.payload, 20U, 3U);
    write_short(batch.payload, 30U, 1U);
    write_short(batch.payload, 32U, 2U);
    model.container.chunks = {header, vertices, batch};
    return model;
}
void unit_tests()
{
    using namespace igi::retail::model;
    auto model = fixture();
    const auto data = Display_DecodeMeshRenderData(model);
    require(data.vertices.size() == 3U && data.batches.size() == 1U, "triangle decoded");
    require(data.batches[0].indices == std::vector<std::uint16_t>({0U, 1U, 2U}), "native indices preserved");
    write_short(model.container.chunks[2].payload, 32U, 3U);
    bool rejected = false;
    try
    {
        static_cast<void>(Display_DecodeMeshRenderData(model));
    }
    catch (const MefFormatError &)
    {
        rejected = true;
    }
    require(rejected, "out-of-range draw index rejected");
    model = fixture();
    model.container.chunks[1].payload.resize(4U);
    rejected = false;
    try
    {
        static_cast<void>(Display_DecodeMeshRenderData(model));
    }
    catch (const MefFormatError &)
    {
        rejected = true;
    }
    require(rejected, "truncated vertex stream rejected");
    model = fixture();
    model.container.chunks[2].payload.resize(32U);
    rejected = false;
    try
    {
        static_cast<void>(Display_DecodeMeshRenderData(model));
    }
    catch (const MefFormatError &)
    {
        rejected = true;
    }
    require(rejected, "truncated draw index stream rejected");
    std::vector<std::byte> texture(26U);
    texture[0] = std::byte{'L'};
    texture[1] = std::byte{'O'};
    texture[2] = std::byte{'O'};
    texture[3] = std::byte{'P'};
    texture[4] = std::byte{2};
    write_short(texture, 0x10U, 2U);
    write_short(texture, 0x12U, 1U);
    write_short(texture, 0x14U, 1U);
    write_short(texture, 0x16U, 2U);
    write_short(texture, 24U, 0xFC00U);
    const auto pixels = igi::retail::texture::Texture_DecodeModelPixels(texture);
    require(pixels.width == 1U && pixels.height == 1U && pixels.argb_pixels[0] == 0xFFF80000U,
            "retail ARGB1555 conversion preserves five-bit channel expansion");
    texture.resize(25U);
    rejected = false;
    try
    {
        static_cast<void>(igi::retail::texture::Texture_DecodeModelPixels(texture));
    }
    catch (const igi::retail::texture::LoopTextureFormatError &)
    {
        rejected = true;
    }
    require(rejected, "truncated model texture scanline rejected");
    igi::retail::script::QvmCompiledProgram program;
    const std::vector<std::byte> bad_code{std::byte{18U}, std::byte{0U}};
    program.code = bad_code;
    rejected = false;
    try
    {
        static_cast<void>(igi::retail::script::Qvm_ReadObjectCalls(program));
    }
    catch (const igi::retail::script::QvmFormatError &)
    {
        rejected = true;
    }
    require(rejected, "QVM stack underflow rejected");
}
} // namespace
int main(int argument_count, char **arguments)
{
    try
    {
        unit_tests();
        if (argument_count > 1)
        {
            igi::retail::resource::ResourceCache cache;
            const auto script = igi::retail::script::Qvm_Load(cache, arguments[1]);
            const auto calls = igi::retail::script::Qvm_ReadObjectCalls(script.program());
            require(!calls.empty(), "retail OBJECTS.QVM contains object calls");
            std::cout << "QVM object calls=" << calls.size() << '\n';
        }
        std::cout << "Render-data tests passed.\n";
        return 0;
    }
    catch (const std::exception &exception)
    {
        std::cerr << "Render-data tests failed: " << exception.what() << '\n';
        return 1;
    }
}
