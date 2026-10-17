#include "igi/retail/audio/sound_adpcm_decoder.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <span>
#include <string_view>

namespace {

using igi::retail::audio::SoundAdpcmDecodeError;
using igi::retail::audio::SoundAdpcmDecodeResult;
using igi::retail::audio::SoundAdpcmState;

void require(bool condition, std::string_view description)
{
    if (!condition) {
        std::cerr << "FAIL: " << description << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void test_initializer_matches_retail_state_offsets()
{
    SoundAdpcmState state{
        .channel_count = 7,
        .predictor_channel_zero = 100,
        .predictor_channel_one = -100,
        .step_index_channel_zero = 12,
        .step_index_channel_one = 18,
        .pending_low_nibble = 1U,
        .reserved_state_byte = 0xA5U,
    };

    igi::retail::audio::Sound_InitializeAdpcmState(state, 2);

    require(state.channel_count == 2, "initializer stores the channel count at offset 0x00");
    require(state.predictor_channel_zero == 0, "initializer clears predictor offset 0x02");
    require(state.predictor_channel_one == 0, "initializer clears predictor offset 0x04");
    require(state.step_index_channel_zero == 0, "initializer clears step index offset 0x06");
    require(state.step_index_channel_one == 0, "initializer clears step index offset 0x08");
    require(state.pending_low_nibble == 0U, "initializer clears pending-nibble byte offset 0x0A");
    require(state.reserved_state_byte == 0xA5U, "initializer leaves byte offset 0x0B unchanged");
}

void test_mono_decoder_reads_high_nibble_before_low_nibble()
{
    SoundAdpcmState state{};
    igi::retail::audio::Sound_InitializeAdpcmState(state, 1);
    constexpr std::array<std::uint8_t, 1U> encoded_input = { 0x71U };
    std::array<std::int16_t, 2U> decoded_output{};

    const SoundAdpcmDecodeResult result =
        igi::retail::audio::Sound_DecodeAdpcmFrames(
            encoded_input, decoded_output, 2U, state);

    require(result.succeeded(), "two mono samples decode from one byte");
    require(result.encoded_bytes_consumed == 1U, "one byte supplies two mono nibbles");
    require(result.decoded_samples_written == 2U, "both mono samples are written");
    require(decoded_output[0U] == 11, "high nibble 7 decodes first");
    require(decoded_output[1U] == 17, "low nibble 1 decodes second");
    require(state.pending_low_nibble == 0U, "two decoded nibbles leave no pending low nibble");
    require(state.predictor_channel_zero == 17, "channel predictor retains the second sample");
    require(state.step_index_channel_zero == 7, "step index follows the retail adjustment table");
}

void test_stereo_decoder_interleaves_channels_per_sample()
{
    SoundAdpcmState state{};
    igi::retail::audio::Sound_InitializeAdpcmState(state, 2);
    constexpr std::array<std::uint8_t, 1U> encoded_input = { 0x79U };
    std::array<std::int16_t, 2U> decoded_output{};

    const SoundAdpcmDecodeResult result =
        igi::retail::audio::Sound_DecodeAdpcmFrames(
            encoded_input, decoded_output, 1U, state);

    require(result.succeeded(), "one stereo frame decodes from one byte");
    require(decoded_output[0U] == 11, "high nibble is channel zero");
    require(decoded_output[1U] == -1, "low nibble is channel one");
    require(state.step_index_channel_zero == 8, "channel zero retains its own step index");
    require(state.step_index_channel_one == 0, "channel one clamps its own step index at zero");
}

void test_streaming_replays_the_partially_consumed_byte()
{
    SoundAdpcmState streamed_state{};
    igi::retail::audio::Sound_InitializeAdpcmState(streamed_state, 1);
    constexpr std::array<std::uint8_t, 1U> encoded_input = { 0x71U };
    std::array<std::int16_t, 1U> first_output{};

    const SoundAdpcmDecodeResult first_result =
        igi::retail::audio::Sound_DecodeAdpcmFrames(
            encoded_input, first_output, 1U, streamed_state);

    require(first_result.succeeded(), "first streaming chunk decodes");
    require(first_output[0U] == 11, "first chunk consumes the high nibble");
    require(streamed_state.pending_low_nibble == 1U, "first chunk leaves the low nibble pending");

    std::array<std::int16_t, 1U> second_output{};
    const SoundAdpcmDecodeResult second_result =
        igi::retail::audio::Sound_DecodeAdpcmFrames(
            encoded_input, second_output, 1U, streamed_state);

    require(second_result.succeeded(), "second streaming chunk decodes");
    require(second_result.encoded_bytes_consumed == 1U, "pending path skips the replayed byte");
    require(second_output[0U] == 17, "second chunk consumes the replayed byte's low nibble");
    require(streamed_state.pending_low_nibble == 0U, "second chunk clears the pending-nibble flag");

    SoundAdpcmState contiguous_state{};
    igi::retail::audio::Sound_InitializeAdpcmState(contiguous_state, 1);
    std::array<std::int16_t, 2U> contiguous_output{};
    const SoundAdpcmDecodeResult contiguous_result =
        igi::retail::audio::Sound_DecodeAdpcmFrames(
            encoded_input, contiguous_output, 2U, contiguous_state);

    require(contiguous_result.succeeded(), "contiguous decode succeeds");
    require(contiguous_output[0U] == first_output[0U], "first chunk matches contiguous decoding");
    require(contiguous_output[1U] == second_output[0U], "second chunk matches contiguous decoding");
    require(contiguous_state == streamed_state, "streamed state matches contiguous state");
}

void test_predictor_and_step_index_clamp_to_retail_limits()
{
    SoundAdpcmState positive_state{};
    igi::retail::audio::Sound_InitializeAdpcmState(positive_state, 1);
    positive_state.predictor_channel_zero = 32760;
    positive_state.step_index_channel_zero = 88;
    constexpr std::array<std::uint8_t, 1U> positive_input = { 0x70U };
    std::array<std::int16_t, 1U> positive_output{};

    const SoundAdpcmDecodeResult positive_result =
        igi::retail::audio::Sound_DecodeAdpcmFrames(
            positive_input, positive_output, 1U, positive_state);

    require(positive_result.succeeded(), "positive saturation case decodes");
    require(positive_output[0U] == 32767, "positive predictor saturates at 0x7FFF");
    require(positive_state.step_index_channel_zero == 88, "step index saturates at 88");

    SoundAdpcmState negative_state{};
    igi::retail::audio::Sound_InitializeAdpcmState(negative_state, 1);
    negative_state.predictor_channel_zero = -32760;
    negative_state.step_index_channel_zero = 88;
    constexpr std::array<std::uint8_t, 1U> negative_input = { 0xF0U };
    std::array<std::int16_t, 1U> negative_output{};

    const SoundAdpcmDecodeResult negative_result =
        igi::retail::audio::Sound_DecodeAdpcmFrames(
            negative_input, negative_output, 1U, negative_state);

    require(negative_result.succeeded(), "negative saturation case decodes");
    require(negative_output[0U] == -32768, "negative predictor saturates at 0x8000");
}

void test_invalid_bounds_return_before_state_or_output_changes()
{
    SoundAdpcmState state{};
    igi::retail::audio::Sound_InitializeAdpcmState(state, 1);
    const SoundAdpcmState original_state = state;
    constexpr std::array<std::uint8_t, 1U> short_input = { 0x71U };
    std::array<std::int16_t, 3U> output_samples = { 123, 123, 123 };

    const SoundAdpcmDecodeResult short_input_result =
        igi::retail::audio::Sound_DecodeAdpcmFrames(
            short_input, output_samples, 3U, state);

    require(short_input_result.error == SoundAdpcmDecodeError::encoded_input_too_short,
        "short input returns a specific error");
    require(state == original_state, "short input does not mutate decoder state");
    require(output_samples[0U] == 123, "short input does not partially write output");

    const SoundAdpcmDecodeResult short_output_result =
        igi::retail::audio::Sound_DecodeAdpcmFrames(
            short_input, std::span<std::int16_t>(output_samples.data(), 2U), 3U, state);

    require(short_output_result.error == SoundAdpcmDecodeError::decoded_output_too_short,
        "short output returns a specific error");
    require(state == original_state, "short output does not mutate decoder state");

    SoundAdpcmState invalid_channel_state = original_state;
    invalid_channel_state.channel_count = 3;
    const SoundAdpcmDecodeResult invalid_channel_result =
        igi::retail::audio::Sound_DecodeAdpcmFrames(
            short_input, output_samples, 1U, invalid_channel_state);
    require(invalid_channel_result.error == SoundAdpcmDecodeError::invalid_channel_count,
        "channel count above the native state capacity is rejected");

    SoundAdpcmState invalid_step_state = original_state;
    invalid_step_state.step_index_channel_zero = 89;
    const SoundAdpcmDecodeResult invalid_step_result =
        igi::retail::audio::Sound_DecodeAdpcmFrames(
            short_input, output_samples, 1U, invalid_step_state);
    require(invalid_step_result.error == SoundAdpcmDecodeError::invalid_step_index,
        "step index outside the observed table is rejected");

    SoundAdpcmState overflow_state = original_state;
    overflow_state.channel_count = 2;
    const SoundAdpcmDecodeResult overflow_result =
        igi::retail::audio::Sound_DecodeAdpcmFrames(
            short_input,
            output_samples,
            std::numeric_limits<std::size_t>::max(),
            overflow_state);
    require(overflow_result.error == SoundAdpcmDecodeError::sample_count_overflow,
        "sample-count multiplication overflow is rejected");
}

} // namespace

int main()
{
    test_initializer_matches_retail_state_offsets();
    test_mono_decoder_reads_high_nibble_before_low_nibble();
    test_stereo_decoder_interleaves_channels_per_sample();
    test_streaming_replays_the_partially_consumed_byte();
    test_predictor_and_step_index_clamp_to_retail_limits();
    test_invalid_bounds_return_before_state_or_output_changes();

    std::cout << "Retail ADPCM decoder tests passed.\n";
    return EXIT_SUCCESS;
}
