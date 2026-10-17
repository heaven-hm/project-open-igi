#pragma once

#include "igi/formats/ilsf_sound.hpp"

#include <cstdint>
#include <ios>
#include <istream>
#include <optional>
#include <span>
#include <vector>

namespace igi::audio
{

// Retail ILSF loader 0x00495F70 keeps streamed sounds on disk; this reader pulls bounded frame ranges.
class IlsfStreamReader
{
  public:
    // The caller owns the stream and must keep it alive for the reader's lifetime.
    [[nodiscard]] static IlsfStreamReader Open(std::istream& source);

    [[nodiscard]] const formats::sound::IlsfSoundData& Header() const noexcept;
    [[nodiscard]] std::uint64_t FramesRead() const noexcept;
    [[nodiscard]] bool EndOfSound() const noexcept;
    [[nodiscard]] bool CanRestart() const noexcept;

    // Writes channel-interleaved signed 16-bit PCM and returns the number of complete frames written.
    [[nodiscard]] std::size_t ReadFrames(std::span<std::int16_t> destination);

    // Restarts at the payload boundary and resets all retail ADPCM predictor state.
    void Restart();

  private:
    IlsfStreamReader(
        std::istream& source,
        formats::sound::IlsfSoundData header,
        std::optional<std::streampos> payload_start);

    std::istream& source_;
    formats::sound::IlsfSoundData header_;
    std::optional<std::streampos> payload_start_;
    formats::sound::IlsfAdpcmDecoderState adpcm_state_;
    std::vector<std::uint8_t> encoded_scratch_;
    std::uint64_t frames_read_ = 0;
};

} // namespace igi::audio
