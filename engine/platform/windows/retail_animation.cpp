#include "retail_animation.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>

namespace igi::host
{
namespace
{

constexpr std::uint32_t kBothTag = 0x48544F42U; // "BOTH" little-endian
constexpr std::uint32_t kBotdTag = 0x44544F42U; // "BOTD"
constexpr std::uint32_t kBoahTag = 0x48414F42U; // "BOAH"
constexpr std::size_t kBotdKeyBytes = 40U;

std::uint32_t read_be32(const std::vector<std::uint8_t>& bytes, std::size_t offset)
{
    return (static_cast<std::uint32_t>(bytes[offset]) << 24U) |
           (static_cast<std::uint32_t>(bytes[offset + 1U]) << 16U) |
           (static_cast<std::uint32_t>(bytes[offset + 2U]) << 8U) |
           static_cast<std::uint32_t>(bytes[offset + 3U]);
}

std::uint32_t read_le32(const std::vector<std::uint8_t>& bytes, std::size_t offset)
{
    return static_cast<std::uint32_t>(bytes[offset]) |
           (static_cast<std::uint32_t>(bytes[offset + 1U]) << 8U) |
           (static_cast<std::uint32_t>(bytes[offset + 2U]) << 16U) |
           (static_cast<std::uint32_t>(bytes[offset + 3U]) << 24U);
}

float read_le_float(const std::vector<std::uint8_t>& bytes, std::size_t offset)
{
    const std::uint32_t raw = read_le32(bytes, offset);
    float value = 0.0F;
    std::memcpy(&value, &raw, sizeof(value));
    return value;
}

bool has_tag(const std::vector<std::uint8_t>& bytes, std::size_t offset, std::uint32_t tag)
{
    return offset + 4U <= bytes.size() && read_le32(bytes, offset) == tag;
}

std::size_t find_tag(const std::vector<std::uint8_t>& bytes, std::size_t begin, std::size_t end,
                     std::uint32_t tag)
{
    if (end > bytes.size())
    {
        end = bytes.size();
    }
    for (std::size_t offset = begin; offset + 4U <= end; ++offset)
    {
        if (read_le32(bytes, offset) == tag)
        {
            return offset;
        }
    }
    return std::string::npos;
}

std::array<double, 3> read_triplet(const std::vector<std::uint8_t>& bytes, std::size_t offset)
{
    return {static_cast<double>(read_le_float(bytes, offset)),
            static_cast<double>(read_le_float(bytes, offset + 4U)),
            static_cast<double>(read_le_float(bytes, offset + 8U))};
}

} // namespace

std::array<double, 3> AnimationClip::sample_root(double time_ms) const
{
    if (root_keys.empty())
    {
        return {};
    }
    if (time_ms <= root_keys.front().time_ms)
    {
        return root_keys.front().position;
    }
    if (time_ms >= root_keys.back().time_ms)
    {
        return root_keys.back().position;
    }

    std::size_t index = 0;
    for (std::size_t candidate = 0; candidate + 1U < root_keys.size(); ++candidate)
    {
        if (root_keys[candidate].time_ms <= time_ms)
        {
            index = candidate;
        }
        else
        {
            break;
        }
    }

    const AnimationRootKey& current = root_keys[index];
    const AnimationRootKey& next = root_keys[index + 1U];
    const double interval = next.time_ms - current.time_ms;
    if (interval <= 0.0)
    {
        return next.position;
    }
    // The retail root track interpolation uses the per-key spline tangents; their
    // sign/order convention is not yet established from the binary, so the host
    // samples the key positions linearly (the editor reference sampler does the
    // same).  The integral over a loop, and therefore the ground speed, is the
    // same for any interpolation through these keys; only the sub-key profile
    // differs.
    const double u = std::clamp((time_ms - current.time_ms) / interval, 0.0, 1.0);
    std::array<double, 3> position{};
    for (std::size_t axis = 0; axis < 3U; ++axis)
    {
        position[axis] = current.position[axis] + ((next.position[axis] - current.position[axis]) * u);
    }
    return position;
}

std::vector<AnimationClip> read_animation_clips(const std::filesystem::path& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input)
    {
        throw std::runtime_error("Cannot open animation IFF: " + path.string());
    }
    std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)),
                                    std::istreambuf_iterator<char>());
    if (bytes.size() < 16U)
    {
        throw std::runtime_error("Animation IFF is too small: " + path.string());
    }

    std::vector<std::size_t> boah_offsets;
    for (std::size_t offset = 0; offset + 20U <= bytes.size(); ++offset)
    {
        if (has_tag(bytes, offset, kBoahTag) && read_be32(bytes, offset + 4U) == 12U)
        {
            boah_offsets.push_back(offset);
        }
    }
    if (boah_offsets.empty())
    {
        throw std::runtime_error("Animation IFF contains no BOAH clips: " + path.string());
    }

    std::vector<AnimationClip> clips;
    clips.reserve(boah_offsets.size());
    for (std::size_t clip_index = 0; clip_index < boah_offsets.size(); ++clip_index)
    {
        const std::size_t boah = boah_offsets[clip_index];
        const std::size_t clip_end = clip_index + 1U < boah_offsets.size()
                                         ? boah_offsets[clip_index + 1U]
                                         : bytes.size();
        const double duration = static_cast<double>(read_le_float(bytes, boah + 8U));
        const std::uint32_t id = read_le32(bytes, boah + 16U);
        if (!std::isfinite(duration) || duration <= 0.0 || id == 0U)
        {
            continue;
        }

        AnimationClip clip{};
        clip.id = id;
        clip.duration_ms = duration;

        const std::size_t both = find_tag(bytes, boah + 20U, clip_end, kBothTag);
        if (both != std::string::npos && both + 12U <= clip_end)
        {
            const std::uint32_t key_count = read_le32(bytes, both + 8U);
            const std::size_t botd = find_tag(bytes, both + 12U, clip_end, kBotdTag);
            const std::size_t available =
                botd == std::string::npos || botd + 8U > clip_end ? 0U : (clip_end - (botd + 8U));
            if (botd != std::string::npos && key_count <= available / kBotdKeyBytes)
            {
                clip.root_keys.reserve(key_count);
                for (std::uint32_t key = 0; key < key_count; ++key)
                {
                    const std::size_t offset = botd + 8U + (static_cast<std::size_t>(key) * kBotdKeyBytes);
                    AnimationRootKey root_key{};
                    root_key.position = read_triplet(bytes, offset);
                    root_key.time_ms = static_cast<double>(read_le_float(bytes, offset + 12U));
                    root_key.tangent_in = read_triplet(bytes, offset + 16U);
                    root_key.tangent_out = read_triplet(bytes, offset + 28U);
                    if (!std::isfinite(root_key.time_ms) ||
                        !std::isfinite(root_key.position[0]) ||
                        !std::isfinite(root_key.position[1]) ||
                        !std::isfinite(root_key.position[2]))
                    {
                        clip.root_keys.clear();
                        break;
                    }
                    clip.root_keys.push_back(root_key);
                }
                std::sort(clip.root_keys.begin(), clip.root_keys.end(),
                          [](const AnimationRootKey& left, const AnimationRootKey& right) {
                              return left.time_ms < right.time_ms;
                          });
            }
        }
        clips.push_back(std::move(clip));
    }
    return clips;
}

} // namespace igi::host
