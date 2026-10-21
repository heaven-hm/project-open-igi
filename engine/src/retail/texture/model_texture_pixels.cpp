#include "igi/retail/texture/model_texture_pixels.hpp"

namespace igi::retail::texture
{
ModelTexturePixels Texture_DecodeModelPixels(std::span<const std::byte> file_bytes)
{
    const auto header = LoopModelTextureFile::DynObj_ParseModelTextureFile(file_bytes);
    const std::uint32_t stride = header.operation_arguments[0];
    const std::uint32_t width = header.operation_arguments[1];
    const std::uint32_t height = header.operation_arguments[2];
    const std::uint32_t format = header.operation_arguments[3];
    const std::size_t bytes_per_pixel = format == 2U || format == 5U ? 2U : 4U;
    if (format != 2U && format != 3U && format != 4U && format != 5U && format != 67U)
    {
        throw LoopTextureFormatError("Model texture pixel format is not ported.");
    }
    if (width == 0U || height == 0U || width > 16384U || height > 16384U ||
        stride < width * bytes_per_pixel || height > (file_bytes.size() - header.pixel_data_offset) / stride)
    {
        throw LoopTextureFormatError("Model texture dimensions or scanline range are invalid.");
    }
    ModelTexturePixels result;
    result.width = width;
    result.height = height;
    result.argb_pixels.reserve(static_cast<std::size_t>(width) * height);
    for (std::size_t row = 0; row < height; ++row)
    {
        for (std::size_t column = 0; column < width; ++column)
        {
            const std::size_t offset = header.pixel_data_offset + row * stride + column * bytes_per_pixel;
            std::uint32_t pixel = std::to_integer<std::uint32_t>(file_bytes[offset]) |
                                  (std::to_integer<std::uint32_t>(file_bytes[offset + 1U]) << 8U);
            if (bytes_per_pixel == 4U)
            {
                pixel |= (std::to_integer<std::uint32_t>(file_bytes[offset + 2U]) << 16U) |
                         (std::to_integer<std::uint32_t>(file_bytes[offset + 3U]) << 24U);
            }
            else if (format == 2U)
            {
                pixel = ((pixel & 0x8000U) != 0U ? 0xFF000000U : 0U) | ((pixel & 0x7C00U) << 9U) |
                        ((pixel & 0x03E0U) << 6U) | ((pixel & 0x001FU) << 3U);
            }
            else
            {
                pixel = 0xFF000000U | ((pixel & 0xF800U) << 8U) | ((pixel & 0x07E0U) << 5U) |
                        ((pixel & 0x001FU) << 3U);
            }
            result.argb_pixels.push_back(pixel);
        }
    }
    return result;
}
} // namespace igi::retail::texture
