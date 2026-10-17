#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace igi::retail::audio {

struct SoundAdpcmState {
    std::int16_t channel_count = 0;
    std::int16_t predictor_channel_zero = 0;
    std::int16_t predictor_channel_one = 0;
    std::int16_t step_index_channel_zero = 0;
    std::int16_t step_index_channel_one = 0;
    std::uint8_t pending_low_nibble = 0U;
    std::uint8_t reserved_state_byte = 0U;

    friend bool operator==(const SoundAdpcmState&, const SoundAdpcmState&) = default;
};

static_assert(offsetof(SoundAdpcmState, channel_count) == 0x00U);
static_assert(offsetof(SoundAdpcmState, predictor_channel_zero) == 0x02U);
static_assert(offsetof(SoundAdpcmState, predictor_channel_one) == 0x04U);
static_assert(offsetof(SoundAdpcmState, step_index_channel_zero) == 0x06U);
static_assert(offsetof(SoundAdpcmState, step_index_channel_one) == 0x08U);
static_assert(offsetof(SoundAdpcmState, pending_low_nibble) == 0x0AU);
static_assert(sizeof(SoundAdpcmState) == 0x0CU);

enum class SoundAdpcmDecodeError : std::uint8_t {
    none,
    invalid_channel_count,
    invalid_step_index,
    sample_count_overflow,
    encoded_input_too_short,
    decoded_output_too_short,
};

struct SoundAdpcmDecodeResult {
    SoundAdpcmDecodeError error = SoundAdpcmDecodeError::none;
    std::size_t encoded_bytes_consumed = 0U;
    std::size_t decoded_samples_written = 0U;

    [[nodiscard]] bool succeeded() const noexcept
    {
        return error == SoundAdpcmDecodeError::none;
    }
};

// Port identifier for the unresolved retail decoder at VA 0x00496280.
// The generated native catalog calls this address Display_SwapBuffers_10, but its
// verified body and sound-loader call sites perform ADPCM decoding.
void Sound_InitializeAdpcmState_00496420(
    SoundAdpcmState& state,
    std::int16_t channel_count) noexcept;

// Bounded helper for retail IGI.EXE decoder VA 0x00496280. A pending low nibble
// requires the caller to re-present the partially consumed byte at input[0].
[[nodiscard]] SoundAdpcmDecodeResult Sound_DecodeAdpcmFrames_00496280(
    std::span<const std::uint8_t> encoded_input,
    std::span<std::int16_t> decoded_output,
    std::size_t frame_count,
    SoundAdpcmState& state) noexcept;

} // namespace igi::retail::audio
