#include "igi/weapon_sound_player.hpp"

#include "igi/diagnostics.hpp"
#include "igi/weapon_accuracy.hpp"

#include <algorithm>
#include <cmath>
#include <exception>
#include <limits>
#include <string>

namespace igi::gameplay
{
namespace
{

constexpr std::string_view audio_log_category = "audio";

} // namespace

WeaponSoundPlayer::WeaponSoundPlayer(audio::SoundSystem& sound_system) noexcept
    : sound_system_(sound_system)
{
}

void WeaponSoundPlayer::WeaponSoundPlayer_Tick(
    bool fired_this_tick,
    const SceneWeapon* selected_weapon,
    audio::RetailVoiceParameters spatial_parameters)
{
    spatial_parameters_ = spatial_parameters;
    if (looping_weapon_ != nullptr && looping_weapon_ != selected_weapon)
    {
        WeaponSoundPlayer_EndLoop();
    }

    if (selected_weapon == nullptr)
    {
        return;
    }

    if (selected_weapon->parameters.maximum_rounds_per_burst <= 1)
    {
        if (fired_this_tick)
        {
            static_cast<void>(WeaponSoundPlayer_Play(selected_weapon->fire_sound_name, false, spatial_parameters_));
        }
        return;
    }

    if (fired_this_tick)
    {
        quiet_tick_count_ = 0;
        if (looping_weapon_ == nullptr)
        {
            looping_voice_handle_ = WeaponSoundPlayer_Play(selected_weapon->fire_sound_name, true, spatial_parameters_);
            if (looping_voice_handle_ != 0)
            {
                looping_weapon_ = selected_weapon;
            }
        }
        else if (looping_voice_handle_ != 0)
        {
            spatial_parameters_.looping = true;
            sound_system_.SoundSystem_SetDefinitionVoiceParameters(
                selected_weapon->fire_sound_name,
                looping_voice_handle_,
                spatial_parameters_);
        }
        return;
    }

    if (looping_weapon_ == nullptr)
    {
        return;
    }

    std::uint32_t end_after_quiet_ticks = 2;
    try
    {
        const float shots_per_minute = selected_weapon->parameters.shots_per_minute;
        if (!std::isfinite(shots_per_minute) || shots_per_minute < 1.0F ||
            shots_per_minute > static_cast<float>(std::numeric_limits<std::int32_t>::max()))
        {
            throw std::invalid_argument("Weapon shots-per-minute value is outside the supported range");
        }
        const std::int32_t ticks_per_round = WeaponAccuracy::Weapon_ComputeCooldownTicks(
            static_cast<std::int32_t>(shots_per_minute));
        end_after_quiet_ticks = static_cast<std::uint32_t>(std::max(2, ticks_per_round + 1));
    }
    catch (const std::exception& exception)
    {
        diagnostics::log(
            diagnostics::Severity::warning,
            audio_log_category,
            "Invalid weapon fire cadence while closing its sound loop: " + std::string(exception.what()));
    }

    ++quiet_tick_count_;
    if (quiet_tick_count_ >= end_after_quiet_ticks)
    {
        WeaponSoundPlayer_EndLoop();
    }
}

void WeaponSoundPlayer::WeaponSoundPlayer_Reset() noexcept
{
    if (looping_voice_handle_ != 0)
    {
        sound_system_.SoundSystem_Stop(looping_voice_handle_);
    }
    looping_voice_handle_ = 0;
    looping_weapon_ = nullptr;
    quiet_tick_count_ = 0;
}

std::uint32_t WeaponSoundPlayer::WeaponSoundPlayer_Play(
    std::string_view sound_name,
    bool looping,
    audio::RetailVoiceParameters spatial_parameters)
{
    if (sound_name.empty())
    {
        return 0;
    }

    try
    {
        spatial_parameters.looping = looping;
        const std::uint32_t definition_voice = sound_system_.SoundSystem_PlayDefinition(
            sound_name,
            spatial_parameters,
            looping ? std::optional<bool>(true) : std::nullopt);
        if (definition_voice != 0)
        {
            return definition_voice;
        }

        return sound_system_.SoundSystem_PlayBankSound(sound_name, spatial_parameters);
    }
    catch (const std::exception& exception)
    {
        diagnostics::log(
            diagnostics::Severity::warning,
            audio_log_category,
            "Weapon sound '" + std::string(sound_name) + "' could not play: " + exception.what());
        return 0;
    }
}

void WeaponSoundPlayer::WeaponSoundPlayer_EndLoop()
{
    const SceneWeapon* ended_weapon = looping_weapon_;
    if (looping_voice_handle_ != 0)
    {
        sound_system_.SoundSystem_Stop(looping_voice_handle_);
    }

    looping_weapon_ = nullptr;
    looping_voice_handle_ = 0;
    quiet_tick_count_ = 0;

    if (ended_weapon != nullptr)
    {
        spatial_parameters_.looping = false;
        static_cast<void>(WeaponSoundPlayer_Play(
            ended_weapon->fire_loop_end_sound_name,
            false,
            spatial_parameters_));
    }
}

} // namespace igi::gameplay
