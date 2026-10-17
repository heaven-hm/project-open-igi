#include "igi/retail/audio/sound_adpcm_decoder.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace igi::retail::audio {
namespace {

// Retail IGI.EXE lookup table at VA 0x00543920 (16 signed bytes).
constexpr std::array<std::int8_t, 16U> retail_adpcm_step_index_adjustments = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
};

// Retail IGI.EXE lookup table at VA 0x00543930 (89 little-endian signed words).
constexpr std::array<std::int16_t, 89U> retail_adpcm_step_sizes = {
    7, 8, 9, 10, 11, 12, 13, 14,
    16, 17, 19, 21, 23, 25, 28, 31,
    34, 37, 41, 45, 50, 55, 60, 66,
    73, 80, 88, 97, 107, 118, 130, 143,
    157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658,
    724, 796, 876, 963, 1060, 1166, 1282, 1411,
    1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
    3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
    7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
    32767,
};

constexpr std::int16_t retail_adpcm_maximum_step_index = 88;
constexpr std::int32_t retail_adpcm_minimum_sample = -32768;
constexpr std::int32_t retail_adpcm_maximum_sample = 32767;
constexpr std::int16_t retail_adpcm_maximum_channel_count = 2;

std::int16_t& predictor_for_channel(SoundAdpcmState& state, std::size_t channel_index) noexcept
{
    return channel_index == 0U
        ? state.predictor_channel_zero
        : state.predictor_channel_one;
}

std::int16_t& step_index_for_channel(SoundAdpcmState& state, std::size_t channel_index) noexcept
{
    return channel_index == 0U
        ? state.step_index_channel_zero
        : state.step_index_channel_one;
}

bool state_has_valid_step_indices(const SoundAdpcmState& state) noexcept
{
    return state.step_index_channel_zero >= 0
        && state.step_index_channel_zero <= retail_adpcm_maximum_step_index
        && state.step_index_channel_one >= 0
        && state.step_index_channel_one <= retail_adpcm_maximum_step_index;
}

std::size_t encoded_bytes_required(std::size_t sample_count, bool has_pending_low_nibble) noexcept
{
    if (sample_count == 0U) {
        return 0U;
    }

    if (has_pending_low_nibble) {
        const std::size_t remaining_samples = sample_count - 1U;
        const std::size_t bytes_for_remaining_samples =
            remaining_samples / 2U + remaining_samples % 2U;
        return 1U + bytes_for_remaining_samples;
    }

    return sample_count / 2U + sample_count % 2U;
}

std::int32_t decode_adpcm_nibble(
    std::uint8_t nibble,
    std::int16_t step_size,
    std::int16_t& predictor) noexcept
{
    std::int32_t sample_delta = static_cast<std::int32_t>(step_size) >> 3;
    if ((nibble & 0x04U) != 0U) {
        sample_delta += static_cast<std::int32_t>(step_size);
    }

    if ((nibble & 0x02U) != 0U) {
        sample_delta += static_cast<std::int32_t>(step_size) >> 1;
    }

    if ((nibble & 0x01U) != 0U) {
        sample_delta += static_cast<std::int32_t>(step_size) >> 2;
    }

    const std::int32_t previous_sample = static_cast<std::int32_t>(predictor);
    const std::int32_t next_sample = (nibble & 0x08U) != 0U
        ? previous_sample - sample_delta
        : previous_sample + sample_delta;
    const std::int32_t clipped_sample = std::clamp(
        next_sample,
        retail_adpcm_minimum_sample,
        retail_adpcm_maximum_sample);

    predictor = static_cast<std::int16_t>(clipped_sample);
    return clipped_sample;
}

} // namespace

void Sound_InitializeAdpcmState_00496420(
    SoundAdpcmState& state,
    std::int16_t channel_count) noexcept
{
    // Retail initializer writes offsets 0x00 through 0x0A, leaving byte 0x0B untouched.
    state.channel_count = channel_count;
    state.predictor_channel_zero = 0;
    state.predictor_channel_one = 0;
    state.step_index_channel_zero = 0;
    state.step_index_channel_one = 0;
    state.pending_low_nibble = 0U;
}

SoundAdpcmDecodeResult Sound_DecodeAdpcmFrames_00496280(
    std::span<const std::uint8_t> encoded_input,
    std::span<std::int16_t> decoded_output,
    std::size_t frame_count,
    SoundAdpcmState& state) noexcept
{
    SoundAdpcmDecodeResult result{};

    if (state.channel_count < 1
        || state.channel_count > retail_adpcm_maximum_channel_count) {
        result.error = SoundAdpcmDecodeError::invalid_channel_count;
        return result;
    }

    if (!state_has_valid_step_indices(state)) {
        result.error = SoundAdpcmDecodeError::invalid_step_index;
        return result;
    }

    const std::size_t channel_count = static_cast<std::size_t>(state.channel_count);
    if (frame_count > std::numeric_limits<std::size_t>::max() / channel_count) {
        result.error = SoundAdpcmDecodeError::sample_count_overflow;
        return result;
    }

    const std::size_t sample_count = frame_count * channel_count;
    if (decoded_output.size() < sample_count) {
        result.error = SoundAdpcmDecodeError::decoded_output_too_short;
        return result;
    }

    const bool has_pending_low_nibble = state.pending_low_nibble != 0U;
    const std::size_t required_encoded_bytes =
        encoded_bytes_required(sample_count, has_pending_low_nibble);
    if (encoded_input.size() < required_encoded_bytes) {
        result.error = SoundAdpcmDecodeError::encoded_input_too_short;
        return result;
    }

    if (sample_count == 0U) {
        return result;
    }

    std::array<std::int16_t, 2U> cached_step_sizes = {
        retail_adpcm_step_sizes[static_cast<std::size_t>(state.step_index_channel_zero)],
        retail_adpcm_step_sizes[static_cast<std::size_t>(state.step_index_channel_one)],
    };

    std::size_t input_byte_index = 0U;
    std::uint8_t current_encoded_byte = 0U;
    if (has_pending_low_nibble) {
        current_encoded_byte = encoded_input[0U];
        input_byte_index = 1U;
    }

    for (std::size_t sample_index = 0U; sample_index < sample_count; ++sample_index) {
        std::uint8_t current_nibble = 0U;
        if (state.pending_low_nibble == 0U) {
            current_encoded_byte = encoded_input[input_byte_index];
            ++input_byte_index;
            current_nibble = static_cast<std::uint8_t>(current_encoded_byte >> 4U);
        } else {
            current_nibble = static_cast<std::uint8_t>(current_encoded_byte & 0x0FU);
        }

        state.pending_low_nibble = state.pending_low_nibble == 0U ? 1U : 0U;
        const std::size_t channel_index = sample_index % channel_count;
        std::int16_t& channel_step_index = step_index_for_channel(state, channel_index);
        std::int16_t& channel_predictor = predictor_for_channel(state, channel_index);

        const std::int32_t adjusted_step_index =
            static_cast<std::int32_t>(channel_step_index)
            + retail_adpcm_step_index_adjustments[current_nibble];
        const std::int32_t clipped_step_index = std::clamp(
            adjusted_step_index,
            0,
            static_cast<std::int32_t>(retail_adpcm_maximum_step_index));
        channel_step_index = static_cast<std::int16_t>(clipped_step_index);

        const std::int32_t decoded_sample = decode_adpcm_nibble(
            current_nibble,
            cached_step_sizes[channel_index],
            channel_predictor);
        decoded_output[sample_index] = static_cast<std::int16_t>(decoded_sample);

        cached_step_sizes[channel_index] =
            retail_adpcm_step_sizes[static_cast<std::size_t>(channel_step_index)];
        ++result.decoded_samples_written;
    }

    result.encoded_bytes_consumed = input_byte_index;
    return result;
}

} // namespace igi::retail::audio
