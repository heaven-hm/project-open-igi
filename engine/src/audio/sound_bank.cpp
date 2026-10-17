#include "igi/audio/sound_bank.hpp"

#include "igi/asset_io.hpp"
#include "igi/diagnostics.hpp"
#include "igi/exceptions.hpp"
#include "igi/formats/ilsf_sound.hpp"
#include "res_parser.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <limits>
#include <system_error>

namespace igi::audio
{
namespace
{

constexpr std::size_t maximum_sound_key_length = 31;
constexpr std::size_t ilsf_header_byte_count = 20;
constexpr std::string_view audio_log_category = "audio";

std::string SoundSystem_NormalizeExtension(const std::filesystem::path& path)
{
    std::string extension = path.extension().string();
    std::transform(
        extension.begin(),
        extension.end(),
        extension.begin(),
        [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
    return extension;
}

bool SoundSystem_IsIlsfAsset(std::span<const std::uint8_t> bytes) noexcept
{
    return formats::sound::SoundSystem_IsIlsfFile(bytes);
}

} // namespace

std::string SoundSystem_ExtractSoundKey(std::string_view sound_path)
{
    const std::size_t extension_position = sound_path.find('.');
    if (extension_position == std::string_view::npos || extension_position == 0)
    {
        return {};
    }

    std::size_t key_start = std::string_view::npos;
    for (std::size_t character_index = extension_position; character_index > 0; --character_index)
    {
        const char character = sound_path[character_index - 1];
        if (character == '/' || character == '\\' || character == ':')
        {
            key_start = character_index;
            break;
        }
    }
    if (key_start == std::string_view::npos || key_start >= extension_position)
    {
        return {};
    }

    const std::size_t key_length = std::min(extension_position - key_start, maximum_sound_key_length);
    std::string key(sound_path.substr(key_start, key_length));
    std::transform(
        key.begin(),
        key.end(),
        key.begin(),
        [](unsigned char character) { return static_cast<char>(std::toupper(character)); });
    return key;
}

SoundBank::SoundBank(AudioMixer& mixer) noexcept : mixer_(mixer)
{
}

SoundBank::~SoundBank()
{
    SoundSystem_Clear();
}

std::size_t SoundBank::SoundSystem_MountDirectory(const std::filesystem::path& directory)
{
    if (directory.empty())
    {
        throw logging::AudioException("Sound directory path cannot be empty");
    }

    std::error_code filesystem_error;
    if (!std::filesystem::is_directory(directory, filesystem_error))
    {
        if (filesystem_error)
        {
            diagnostics::log(
                diagnostics::Severity::warning,
                audio_log_category,
                "Sound directory '" + directory.string() + "' could not be inspected: " +
                    filesystem_error.message());
        }
        else
        {
            diagnostics::log(
                diagnostics::Severity::debug,
                audio_log_category,
                "Sound directory '" + directory.string() + "' is absent; no files were mounted");
        }
        return 0;
    }

    std::vector<std::filesystem::path> wave_files;
    std::vector<std::filesystem::path> archive_files;
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }
            const std::string extension = SoundSystem_NormalizeExtension(entry.path());
            if (extension == ".wav")
            {
                wave_files.push_back(entry.path());
            }
            else if (extension == ".res")
            {
                archive_files.push_back(entry.path());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception)
    {
        diagnostics::log(
            diagnostics::Severity::warning,
            audio_log_category,
            "Sound directory '" + directory.string() + "' could not be enumerated: " + exception.what());
        return 0;
    }

    const auto sort_paths = [](std::vector<std::filesystem::path>& paths)
    {
        std::sort(paths.begin(), paths.end(), [](const auto& left_path, const auto& right_path)
        {
            return assets::normalize_name(left_path.filename().string()) <
                   assets::normalize_name(right_path.filename().string());
        });
    };
    sort_paths(wave_files);
    sort_paths(archive_files);

    const std::size_t before = entries_.size();
    for (const auto& file_path : wave_files)
    {
        static_cast<void>(SoundSystem_AddFile(file_path));
    }
    for (const auto& archive_path : archive_files)
    {
        static_cast<void>(SoundSystem_MountArchive(archive_path));
    }

    const std::size_t added_count = entries_.size() - before;
    const std::string mounted_path = directory.string();
    if (std::find(mounted_paths_.begin(), mounted_paths_.end(), mounted_path) == mounted_paths_.end())
    {
        mounted_paths_.push_back(mounted_path);
    }
    diagnostics::log(
        diagnostics::Severity::information,
        audio_log_category,
        "Mounted '" + mounted_path + "': " + std::to_string(added_count) + " sound(s)");
    return added_count;
}

std::size_t SoundBank::SoundSystem_MountArchive(const std::filesystem::path& archive_path)
{
    if (archive_path.empty())
    {
        throw logging::AudioException("Sound archive path cannot be empty");
    }

    std::error_code filesystem_error;
    if (!std::filesystem::is_regular_file(archive_path, filesystem_error))
    {
        if (filesystem_error)
        {
            diagnostics::log(
                diagnostics::Severity::warning,
                audio_log_category,
                "Sound archive '" + archive_path.string() + "' could not be inspected: " +
                    filesystem_error.message());
        }
        return 0;
    }

    const std::size_t before = entries_.size();
    std::string parse_error;
    try
    {
        // Original IGI.EXE sound-directory native 0x004E68D0 delegates file walking to 0x004B1420.
        // REA/Ghidra evidence ev_11824e316ce3e5127f136ede5036f3863e632198a7ad14fdc07e50bc2a7681ba
        // confirms the wrapper and callback binding; RES parsing is the portable asset-format layer.
        const bool archive_valid = RES_ForEachEntry(
            archive_path.string(),
            [&](const std::string& entry_name, const std::uint8_t* entry_data, std::size_t entry_size)
            {
                if (entry_data == nullptr && entry_size != 0)
                {
                    diagnostics::log(
                        diagnostics::Severity::warning,
                        audio_log_category,
                        "Sound archive '" + archive_path.string() + "' returned an invalid entry buffer");
                    return;
                }
                const auto encoded_bytes = std::span<const std::uint8_t>(entry_data, entry_size);
                if (!SoundSystem_IsIlsfAsset(encoded_bytes))
                {
                    return;
                }
                try
                {
                    static_cast<void>(SoundSystem_AddAsset(entry_name, encoded_bytes, true));
                }
                catch (const std::exception& exception)
                {
                    diagnostics::log(
                        diagnostics::Severity::warning,
                        audio_log_category,
                        "Sound archive entry '" + entry_name + "' could not be loaded: " + exception.what());
                }
            },
            parse_error);
        if (!archive_valid)
        {
            diagnostics::log(
                diagnostics::Severity::warning,
                audio_log_category,
                "Sound archive '" + archive_path.string() + "' would not open: " + parse_error);
            return 0;
        }
    }
    catch (const std::exception& exception)
    {
        diagnostics::log(
            diagnostics::Severity::warning,
            audio_log_category,
            "Sound archive '" + archive_path.string() + "' stopped while loading: " + exception.what());
        return entries_.size() - before;
    }

    const std::string mounted_path = archive_path.string();
    if (std::find(mounted_paths_.begin(), mounted_paths_.end(), mounted_path) == mounted_paths_.end())
    {
        mounted_paths_.push_back(mounted_path);
    }
    return entries_.size() - before;
}

void SoundBank::SoundSystem_Clear() noexcept
{
    for (const auto& [name, entry] : entries_)
    {
        static_cast<void>(name);
        if (entry.audio_buffer_handle != 0)
        {
            mixer_.DestroyBuffer(entry.audio_buffer_handle);
        }
    }
    entries_.clear();
    mounted_paths_.clear();
}

bool SoundBank::SoundSystem_TryGet(std::string_view name, const SoundEntry*& entry) const
{
    entry = nullptr;
    if (name.empty())
    {
        return false;
    }
    std::string key(name);
    std::transform(
        key.begin(),
        key.end(),
        key.begin(),
        [](unsigned char character) { return static_cast<char>(std::toupper(character)); });

    const auto iterator = entries_.find(key);
    if (iterator == entries_.end())
    {
        return false;
    }
    entry = &iterator->second;
    return true;
}

std::uint32_t SoundBank::SoundSystem_GetOrCreateAudioBuffer(std::string_view name)
{
    const SoundEntry* found_entry = nullptr;
    if (!SoundSystem_TryGet(name, found_entry) || found_entry == nullptr)
    {
        return 0;
    }

    auto iterator = entries_.find(found_entry->name);
    if (iterator == entries_.end())
    {
        throw logging::AudioException("Sound bank lookup returned an entry that is no longer registered");
    }
    return SoundSystem_CreateAudioBuffer(iterator->second);
}

std::size_t SoundBank::SoundSystem_Count() const noexcept
{
    return entries_.size();
}

const std::vector<std::string>& SoundBank::SoundSystem_MountedPaths() const noexcept
{
    return mounted_paths_;
}

bool SoundBank::SoundSystem_AddFile(const std::filesystem::path& file_path)
{
    try
    {
        const assets::FileBytes encoded_file = assets::read_file(file_path);
        return SoundSystem_AddAsset(file_path.string(), encoded_file, false);
    }
    catch (const std::exception& exception)
    {
        diagnostics::log(
            diagnostics::Severity::warning,
            audio_log_category,
            "Sound '" + file_path.string() + "' would not load: " + exception.what());
        return false;
    }
}

bool SoundBank::SoundSystem_AddAsset(
    std::string origin,
    std::span<const std::uint8_t> encoded_file,
    bool stored_in_archive)
{
    const std::string key = SoundSystem_ExtractSoundKey(origin);
    if (key.empty())
    {
        diagnostics::log(
            diagnostics::Severity::warning,
            audio_log_category,
            "Invalid sound filename '" + origin + "'");
        return false;
    }

    if (entries_.contains(key))
    {
        diagnostics::log(
            diagnostics::Severity::debug,
            audio_log_category,
            "Sound '" + key + "' already loaded; keeping the first entry");
        return false;
    }
    if (!SoundSystem_IsIlsfAsset(encoded_file))
    {
        return false;
    }

    SoundEntry entry;
    entry.name = key;
    entry.origin = std::move(origin);
    entry.header = formats::sound::SoundSystem_ParseIlsfFile(encoded_file);
    entry.stored_in_archive = stored_in_archive;
    // The C# reference keeps only a path for streamed directory sounds. Archive entries have no
    // independently openable path in this C++ VFS yet, so retain their encoded bytes for playback.
    if (!entry.header.IsStreamed() || stored_in_archive)
    {
        entry.encoded_file.assign(encoded_file.begin(), encoded_file.end());
    }
    entries_.emplace(key, std::move(entry));
    return true;
}

std::uint32_t SoundBank::SoundSystem_CreateAudioBuffer(SoundEntry& entry)
{
    if (entry.audio_buffer_handle != 0)
    {
        return entry.audio_buffer_handle;
    }

    try
    {
        assets::FileBytes encoded_file;
        std::span<const std::uint8_t> bytes = entry.encoded_file;
        if (bytes.empty())
        {
            if (entry.origin.empty())
            {
                throw logging::AudioException("Streamed sound has no readable source path");
            }
            encoded_file = assets::read_file(entry.origin);
            bytes = encoded_file;
        }

        const auto decoded_sound = formats::sound::SoundSystem_ParseIlsfFile(bytes);
        const std::vector<std::int16_t> pcm_samples =
            formats::sound::SoundSystem_DecodeIlsfFileToPcm16(decoded_sound);
        entry.audio_buffer_handle = mixer_.CreateBuffer(
            {decoded_sound.sample_rate, decoded_sound.channel_count},
            pcm_samples);
        diagnostics::log(
            diagnostics::Severity::debug,
            audio_log_category,
            "Decoded sound '" + entry.name + "' to mixer buffer " +
                std::to_string(entry.audio_buffer_handle));
        return entry.audio_buffer_handle;
    }
    catch (const std::exception& exception)
    {
        diagnostics::log(
            diagnostics::Severity::error,
            audio_log_category,
            "Sound '" + entry.name + "' could not be decoded: " + exception.what());
        return 0;
    }
}

} // namespace igi::audio
