#pragma once

#include "igi/retail/audio/sound_adpcm_decoder.hpp"
#include "igi/retail/audio/sound_ilsf_header.hpp"

#include <cstddef>
#include <cstdint>
#include <istream>
#include <span>
#include <stdexcept>

namespace igi::retail::audio {

class SoundIlsfStreamError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Bounded frame reader for the four storage branches in retail sound loader FUN_00495F70.
// Static sources: Ghidra/REA ev_ab711dc698b93f8ca5d3b05777759dacdc4dee8a2d085b49203a6751e188e7c9
// and pseudocode ev_b83d3e432ec4f45b736a09f13db010e527f632434d5f932180b5454612bd86ee.
// ADPCM calls the retail decoder at 0x00496280, initialized by 0x00496420. Runtime playback parity
// has not been verified. The catalog-derived Sound_Load label is provisional; REA names the loader FUN_00495f70.
class SoundIlsfStreamReader final {
public:
    explicit SoundIlsfStreamReader(std::istream& input_stream);

    [[nodiscard]] const SoundIlsfHeader& header() const noexcept;
    [[nodiscard]] std::uint64_t frames_read() const noexcept;
    [[nodiscard]] bool end_of_sound() const noexcept;
    [[nodiscard]] bool can_restart() const noexcept;

    // Port helper for FUN_00495f70; returns decoded 16-bit frames, interleaved by channel.
    // Streamed input is read in bounded chunks. ADPCM re-presents the partially consumed byte exactly
    // as FUN_00496280 requires; the C# candidate's forward-only nibble carry is not copied.
    [[nodiscard]] std::size_t Sound_ReadIlsfFrames(std::span<std::int16_t> output_samples);

    // Restarts at the payload only when the underlying stream supports seeking.
    void Sound_RestartIlsfStream();

private:
    void read_exact(std::span<std::byte> destination);
    void read_exact(std::span<std::uint8_t> destination);

    std::istream* input_stream_ = nullptr;
    SoundIlsfHeader header_{};
    SoundAdpcmState adpcm_state_{};
    std::streampos payload_start_ = std::streampos(-1);
    std::uint64_t frames_read_ = 0U;
    std::uint8_t replayed_adpcm_byte_ = 0U;
    bool can_restart_ = false;
};

} // namespace igi::retail::audio
