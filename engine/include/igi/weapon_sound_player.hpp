#pragma once

#include "igi/audio/sound_system.hpp"
#include "igi/mission_scene.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace igi::gameplay
{

// Port helper for the original IGI.EXE weapon sound trigger; this is not a retail native entrypoint.
class WeaponSoundPlayer final
{
  public:
    explicit WeaponSoundPlayer(audio::SoundSystem& sound_system) noexcept;

    void WeaponSoundPlayer_Tick(
        bool fired_this_tick,
        const SceneWeapon* selected_weapon,
        audio::RetailVoiceParameters spatial_parameters = {});
    void WeaponSoundPlayer_Reset() noexcept;

  private:
    [[nodiscard]] std::uint32_t WeaponSoundPlayer_Play(
        std::string_view sound_name,
        bool looping,
        audio::RetailVoiceParameters spatial_parameters);
    void WeaponSoundPlayer_EndLoop();

    audio::SoundSystem& sound_system_;
    const SceneWeapon* looping_weapon_{};
    std::uint32_t looping_voice_handle_{};
    std::uint32_t quiet_tick_count_{};
    audio::RetailVoiceParameters spatial_parameters_{};
};

} // namespace igi::gameplay
