#pragma once

#include "igi/audio/audio_mixer.hpp"
#include "igi/formats/ilsf_sound.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace igi::audio
{

// Original IGI.EXE key extraction routine at VA 0x00497390; confirmed in REA/Ghidra evidence
// ev_b1605f6cb7a9e93953c35422081be664371ff28e94b4a25b4347a85532809832.
[[nodiscard]] std::string SoundSystem_ExtractSoundKey(std::string_view sound_path);

struct SoundEntry final
{
    std::string name;
    std::string origin;
    formats::sound::IlsfSoundData header;
    std::vector<std::uint8_t> encoded_file;
    std::uint32_t audio_buffer_handle{};
    bool stored_in_archive{};

    [[nodiscard]] bool SoundSystem_IsStreamed() const noexcept
    {
        return header.IsStreamed();
    }
};

class SoundBank final
{
  public:
    explicit SoundBank(AudioMixer& mixer) noexcept;
    ~SoundBank();

    SoundBank(const SoundBank&) = delete;
    SoundBank& operator=(const SoundBank&) = delete;
    SoundBank(SoundBank&&) = delete;
    SoundBank& operator=(SoundBank&&) = delete;

    [[nodiscard]] std::size_t SoundSystem_MountDirectory(const std::filesystem::path& directory);
    [[nodiscard]] std::size_t SoundSystem_MountArchive(const std::filesystem::path& archive_path);
    void SoundSystem_Clear() noexcept;
    [[nodiscard]] bool SoundSystem_TryGet(std::string_view name, const SoundEntry*& entry) const;
    [[nodiscard]] std::uint32_t SoundSystem_GetOrCreateAudioBuffer(std::string_view name);
    [[nodiscard]] std::size_t SoundSystem_Count() const noexcept;
    [[nodiscard]] const std::vector<std::string>& SoundSystem_MountedPaths() const noexcept;

  private:
    [[nodiscard]] bool SoundSystem_AddFile(const std::filesystem::path& file_path);
    [[nodiscard]] bool SoundSystem_AddAsset(
        std::string origin,
        std::span<const std::uint8_t> encoded_file,
        bool stored_in_archive);
    [[nodiscard]] std::uint32_t SoundSystem_CreateAudioBuffer(SoundEntry& entry);

    AudioMixer& mixer_;
    std::unordered_map<std::string, SoundEntry> entries_;
    std::vector<std::string> mounted_paths_;
};

} // namespace igi::audio
