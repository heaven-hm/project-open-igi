#include "igi/asset_io.hpp"
#include "igi/audio/audio_mixer.hpp"
#include "igi/audio/animation_event_dispatcher.hpp"
#include "igi/audio/animation_trigger_registry.hpp"
#include "igi/audio/sdl_audio_output.hpp"
#include "igi/audio/sound_bank.hpp"
#include "igi/audio/sound_definition_registry.hpp"
#include "igi/audio/sound_system.hpp"
#include "igi/audio/sound_volume_curve.hpp"
#include "igi/diagnostics.hpp"
#include "igi/exceptions.hpp"
#include "igi/game_simulation.hpp"
#include "igi/weapon_sound_player.hpp"
#include "igi/input_controller.hpp"
#include "igi/log_macros.hpp"
#include "igi/logger.hpp"
#include "igi/scene_renderer.hpp"
#include "igi/vector_math.hpp"

#include <SDL.h>
#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace
{

using igi::diagnostics::log;
using igi::diagnostics::Severity;
using igi::gameplay::FreeCamera;
using igi::gameplay::length;
using igi::gameplay::PlayerInput;
using igi::gameplay::Vector3;
using Clock = std::chrono::steady_clock;

struct RuntimeOptions final
{
    bool headless{};
    bool demonstration{};
    bool verbose{};
    bool benchmark{};
    bool fullscreen{};
    int width = 1280;
    int height = 960;
    int mission = 1;
    int difficulty = 1;
    int target_fps = 60;
    igi::gameplay::TerrainDetail terrain_detail = igi::gameplay::TerrainDetail::enhanced;
    igi::gameplay::TextureFilterMode texture_filter_mode =
        igi::gameplay::TextureFilterMode::trilinear_mipmap;
    std::optional<std::uint32_t> texture_anisotropy;
    std::optional<std::uint64_t> frames;
    std::filesystem::path game_directory;
    std::filesystem::path cache_directory;
    std::filesystem::path screenshot;
    std::filesystem::path replay;
    std::filesystem::path trace;
    std::string profile;
};

template <class Integer> Integer parse_integer(std::string_view text, Integer minimum, Integer maximum)
{
    Integer value{};
    const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
    if (result.ec != std::errc{} || result.ptr != text.data() + text.size() || value < minimum ||
        value > maximum)
    {
        throw std::runtime_error("Invalid integer argument: " + std::string(text));
    }
    return value;
}

RuntimeOptions parse_arguments(int argument_count, char** arguments)
{
    RuntimeOptions options;
    for (int argument_index = 1; argument_index < argument_count; ++argument_index)
    {
        const std::string_view argument(arguments[argument_index]);
        const auto next_argument = [&]() -> std::string_view
        {
            if (++argument_index >= argument_count)
            {
                throw std::runtime_error("Missing value for " + std::string(argument));
            }
            return arguments[argument_index];
        };
        if (argument == "window" || argument == "--window")
        {
            options.fullscreen = false;
        }
        else if (argument == "fullscreen" || argument == "--fullscreen")
        {
            options.fullscreen = true;
        }
        else if (argument == "easy" || argument == "--easy")
        {
            options.difficulty = 0;
        }
        else if (argument == "medium" || argument == "--medium")
        {
            options.difficulty = 1;
        }
        else if (argument == "hard" || argument == "--hard")
        {
            options.difficulty = 2;
        }
        else if (argument == "--difficulty")
        {
            const auto diff = next_argument();
            if (diff == "easy" || diff == "1" || diff == "GD_1")
            {
                options.difficulty = 0;
            }
            else if (diff == "medium" || diff == "2" || diff == "GD_2")
            {
                options.difficulty = 1;
            }
            else if (diff == "hard" || diff == "3" || diff == "GD_3")
            {
                options.difficulty = 2;
            }
            else
            {
                throw std::runtime_error("Difficulty must be easy, medium, or hard");
            }
        }
        else if (argument == "--fps")
        {
            options.target_fps = parse_integer(next_argument(), 0, 1000);
        }
        else if (argument == "640x480")
        {
            options.width = 640;
            options.height = 480;
        }
        else if (argument == "800x600")
        {
            options.width = 800;
            options.height = 600;
        }
        else if (argument == "1024x768")
        {
            options.width = 1024;
            options.height = 768;
        }
        else if (argument == "1280x960")
        {
            options.width = 1280;
            options.height = 960;
        }
        else if (argument == "1920x1080")
        {
            options.width = 1920;
            options.height = 1080;
        }
        else if (argument == "--res")
        {
            const auto res = next_argument();
            const auto sep = res.find('x');
            if (sep == std::string_view::npos)
            {
                throw std::runtime_error("Resolution format must be <width>x<height>");
            }
            options.width = parse_integer(res.substr(0, sep), 160, 3840);
            options.height = parse_integer(res.substr(sep + 1), 120, 2160);
        }
        else if (argument.starts_with("level"))
        {
            options.mission = parse_integer(argument.substr(5), 1, 14);
        }
        else if (argument == "--headless")
        {
            options.headless = true;
        }
        else if (argument == "--demo")
        {
            options.demonstration = true;
        }
        else if (argument == "--verbose")
        {
            options.verbose = true;
        }
        else if (argument == "--benchmark")
        {
            options.benchmark = true;
        }
        else if (argument == "--game")
        {
            options.game_directory = next_argument();
        }
        else if (argument == "--cache")
        {
            options.cache_directory = next_argument();
        }
        else if (argument == "--screenshot")
        {
            options.screenshot = next_argument();
        }
        else if (argument == "--replay")
        {
            options.replay = next_argument();
        }
        else if (argument == "--trace")
        {
            options.trace = next_argument();
        }
        else if (argument == "--profile")
        {
            options.profile = next_argument();
        }
        else if (argument == "--mission")
        {
            options.mission = parse_integer(next_argument(), 1, 14);
        }
        else if (argument == "--graphics" || argument == "--gfx")
        {
            const auto setting = next_argument();
            if (setting == "vanilla")
            {
                options.terrain_detail = igi::gameplay::TerrainDetail::vanilla;
            }
            else if (setting == "ultra")
            {
                options.terrain_detail = igi::gameplay::TerrainDetail::ultra;
            }
            else if (setting == "enhanced")
            {
                options.terrain_detail = igi::gameplay::TerrainDetail::enhanced;
            }
            else if (setting == "high")
            {
                options.terrain_detail = igi::gameplay::TerrainDetail::high;
            }
            else if (setting == "medium")
            {
                options.terrain_detail = igi::gameplay::TerrainDetail::medium;
            }
            else if (setting == "low")
            {
                options.terrain_detail = igi::gameplay::TerrainDetail::low;
            }
            else
            {
                throw std::runtime_error(
                    "Graphics setting must be vanilla, low, medium, enhanced, high, or ultra");
            }
        }
        else if (argument == "--texture-filter")
        {
            const auto setting = next_argument();
            if (setting == "nearest")
            {
                options.texture_filter_mode = igi::gameplay::TextureFilterMode::nearest_mipmap;
            }
            else if (setting == "trilinear")
            {
                options.texture_filter_mode = igi::gameplay::TextureFilterMode::trilinear_mipmap;
            }
            else
            {
                throw std::runtime_error("Texture filter must be nearest or trilinear");
            }
        }
        else if (argument == "--texture-anisotropy")
        {
            const auto anisotropy = parse_integer<std::uint32_t>(next_argument(), 1, 16);
            if ((anisotropy & (anisotropy - 1)) != 0)
            {
                throw std::runtime_error("Texture anisotropy must be 1, 2, 4, 8, or 16");
            }
            options.texture_anisotropy = anisotropy;
        }
        else if (argument == "--fps")
        {
            options.target_fps = parse_integer(next_argument(), 15, 360);
        }
        else if (argument == "--width")
        {
            options.width = parse_integer(next_argument(), 160, 3840);
        }
        else if (argument == "--height")
        {
            options.height = parse_integer(next_argument(), 120, 2160);
        }
        else if (argument == "--frames")
        {
            options.frames = parse_integer<std::uint64_t>(next_argument(), 1, 10000000);
        }
        else if (argument == "--help")
        {
            std::cout
                << "igi [window|fullscreen] [level1..level14] [--game directory]\n"
                   "    [--mission 1] [--headless --frames count]\n"
                   "    [--demo] [--replay inputs.csv] [--trace state.csv] "
                   "[--screenshot frame.bmp]\n"
                   "    [--cache directory] [--width 1280 --height 960] [--verbose] "
                   "[--benchmark]\n"
                   "    [--graphics vanilla|low|medium|enhanced|high|ultra]\n"
                   "    [--texture-filter nearest|trilinear]\n"
                   "    [--texture-anisotropy 1|2|4|8|16]\n"
                   "    [--profile name] (controls come from original config.qvm)\n"
                   "Movement, jump, crouch, fire, reload and weapon bindings use "
                   "config.qvm.\n"
                   "Number-key categories use humanplayer.qvm; mouse wheel cycles "
                   "weapons.\n"
                   "Enter restarts; F10 toggles fullscreen; F9 or G toggles developer mode; Escape quits.\n";
            std::exit(0);
        }
        else
        {
            throw std::runtime_error("Unknown argument: " + std::string(argument));
        }
    }
    if (options.headless && !options.frames)
    {
        options.frames = 120;
    }
    return options;
}

void resolve_runtime_paths(RuntimeOptions& options, const char* executable)
{
    if (options.game_directory.empty())
    {
        if (const auto* configured_game = std::getenv("IGI_GAME_PATH"))
        {
            options.game_directory = configured_game;
        }
    }
    if (options.game_directory.empty())
    {
        const auto binary_directory = std::filesystem::absolute(executable).parent_path();
        std::vector<std::filesystem::path> candidates{
            std::filesystem::current_path(),
            binary_directory,
            binary_directory.parent_path().parent_path().parent_path()};
        const char* home_directory = std::getenv("HOME");
#if defined(_WIN32)
        if (home_directory == nullptr)
        {
            home_directory = std::getenv("USERPROFILE");
        }
#endif
        if (home_directory != nullptr)
        {
            candidates.emplace_back(std::filesystem::path(home_directory) / "Downloads" / "IGI");
            candidates.emplace_back(std::filesystem::path(home_directory) / "IGI");
            candidates.emplace_back(std::filesystem::path(home_directory) / "Games" / "IGI");
        }
#if defined(_WIN32)
        candidates.emplace_back("C:/IGI");
        candidates.emplace_back("C:/Games/IGI");
        candidates.emplace_back("C:/Program Files (x86)/Project IGI");
        candidates.emplace_back("C:/Program Files/Project IGI");
#elif defined(__linux__)
        candidates.emplace_back("/opt/igi");
        candidates.emplace_back("/usr/local/share/igi");
        candidates.emplace_back("/usr/share/igi");
#endif
        for (const auto& candidate : candidates)
        {
            if (std::filesystem::is_directory(candidate / "MISSIONS") ||
                std::filesystem::is_directory(candidate / "missions"))
            {
                options.game_directory = candidate;
                break;
            }
        }
    }
    if (options.game_directory.empty())
    {
        throw std::runtime_error("Original game assets were not found; use --game or IGI_GAME_PATH");
    }
    if (options.cache_directory.empty())
    {
        char* preference_path = SDL_GetPrefPath("HeavenHM", "ProjectIGI");
        if (preference_path == nullptr)
        {
            throw std::runtime_error("Cannot determine application cache directory");
        }
        options.cache_directory = preference_path;
        SDL_free(preference_path);
    }
    std::filesystem::create_directories(options.cache_directory);
    igi::diagnostics::configure(options.verbose, options.cache_directory / "igi.log");
}

std::vector<PlayerInput> load_replay(const std::filesystem::path& path)
{
    if (path.empty())
    {
        return {};
    }
    std::ifstream file(path);
    if (!file)
    {
        throw std::runtime_error("Cannot open input replay: " + path.string());
    }
    std::vector<PlayerInput> inputs;
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line.starts_with('#'))
        {
            continue;
        }
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream row(line);
        PlayerInput input;
        if (!(row >> input.forward >> input.sideways >> input.heading_change >> input.pitch_change >>
              input.walking >> input.crouching >> input.jumping >> input.firing >> input.reloading >>
              input.aiming))
        {
            throw std::runtime_error("Invalid replay row " + std::to_string(inputs.size() + 1));
        }
        std::string trailing;
        row >> std::ws;
        if (!row.eof())
        {
            if (!(row >> input.weapon_slot >> input.weapon_cycle >> input.interacting))
            {
                throw std::runtime_error("Invalid optional inventory columns in replay");
            }
        }
        if (row >> trailing || inputs.size() >= 1000000)
        {
            throw std::runtime_error("Replay row or length exceeds the supported format");
        }
        inputs.push_back(input);
    }
    return inputs;
}

PlayerInput demonstration_input(std::uint64_t tick)
{
    PlayerInput input;
    input.forward = tick < 50 ? 1.0f : 0.0f;
    input.walking = tick < 25;
    input.jumping = tick == 35;
    input.crouching = tick >= 70 && tick < 95;
    input.firing = tick >= 110 && tick < 200;
    input.reloading = tick == 205;
    input.heading_change = tick >= 260 && tick < 320 ? -0.015f : 0.0f;
    return input;
}

void save_frame(const igi::gameplay::SceneRenderer& renderer, const std::filesystem::path& path)
{
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> surface(
        SDL_CreateRGBSurfaceWithFormatFrom(const_cast<std::uint32_t*>(renderer.pixels().data()),
                                           renderer.width(),
                                           renderer.height(),
                                           32,
                                           renderer.width() * 4,
                                           SDL_PIXELFORMAT_ARGB8888),
        SDL_FreeSurface);
    if (!surface || SDL_SaveBMP(surface.get(), path.string().c_str()) != 0)
    {
        throw std::runtime_error("Cannot save frame " + path.string() + ": " + SDL_GetError());
    }
    log(Severity::information, "capture", "Saved rendered frame: " + path.string());
}

class SdlRuntime final
{
  public:
    explicit SdlRuntime(bool headless)
    {
        SDL_SetMainReady();
        if (SDL_Init(headless ? SDL_INIT_TIMER : SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
        {
            throw std::runtime_error("SDL initialization failed: " + std::string(SDL_GetError()));
        }
    }

    ~SdlRuntime()
    {
        SDL_Quit();
    }

    SdlRuntime(const SdlRuntime&) = delete;
    SdlRuntime& operator=(const SdlRuntime&) = delete;
};

int run(RuntimeOptions options, const char* executable)
{
    resolve_runtime_paths(options, executable);
    const SdlRuntime sdl(options.headless);
    log(Severity::information,
        "application",
        "Loading original mission " + std::to_string(options.mission) + " from " +
            options.game_directory.string());
    const auto scene = igi::gameplay::load_mission(
        options.game_directory, options.cache_directory / "assets", options.mission, options.terrain_detail);
    log(Severity::information,
        "graphics",
        std::string("Terrain detail=") +
            (options.terrain_detail == igi::gameplay::TerrainDetail::ultra      ? "ultra"
             : options.terrain_detail == igi::gameplay::TerrainDetail::high     ? "high"
             : options.terrain_detail == igi::gameplay::TerrainDetail::enhanced ? "enhanced"
             : options.terrain_detail == igi::gameplay::TerrainDetail::vanilla  ? "vanilla"
             : options.terrain_detail == igi::gameplay::TerrainDetail::medium   ? "medium"
                                                                                : "low") +
            "; original terrain triangles=" + std::to_string(scene.terrain_triangle_count) +
            "; viewport=" + std::to_string(options.width) + "x" + std::to_string(options.height));
    std::size_t authored_mipmap_count = 0;
    std::size_t generated_mipmap_count = 0;
    for (const auto& scene_texture : scene.textures)
    {
        for (const auto& mipmap : scene_texture.mipmaps)
        {
            if (mipmap.authored)
            {
                ++authored_mipmap_count;
            }
            else
            {
                ++generated_mipmap_count;
            }
        }
    }
    const std::uint32_t preset_anisotropy =
        options.terrain_detail == igi::gameplay::TerrainDetail::ultra ? 16u
        : options.terrain_detail == igi::gameplay::TerrainDetail::high ? 8u
        : options.terrain_detail == igi::gameplay::TerrainDetail::enhanced ? 4u
                                                                         : 1u;
    const std::uint32_t selected_anisotropy =
        options.texture_filter_mode == igi::gameplay::TextureFilterMode::nearest_mipmap
            ? 1u
            : options.texture_anisotropy.value_or(preset_anisotropy);
    log(Severity::information,
        "graphics",
        std::string("Texture filter=") +
            (options.texture_filter_mode == igi::gameplay::TextureFilterMode::nearest_mipmap
                 ? "nearest mip level"
                 : "trilinear mip filtering") +
            "; textures=" + std::to_string(scene.textures.size()) +
            "; authored mip levels=" + std::to_string(authored_mipmap_count) +
            "; generated mip levels=" + std::to_string(generated_mipmap_count) +
            "; anisotropy=" + std::to_string(selected_anisotropy) + "x");
    igi::audio::AudioMixer audio_mixer;
    igi::audio::SoundBank sound_bank(audio_mixer);
    igi::audio::SoundDefinitionRegistry sound_definitions;
    igi::audio::AnimationTriggerRegistry animation_trigger_registry;
    igi::audio::SoundSystem sound_system(audio_mixer, sound_bank, sound_definitions);
    igi::gameplay::WeaponSoundPlayer weapon_sound_player(sound_system);
    std::unique_ptr<igi::audio::SdlAudioOutput> audio_output;
    if (!options.headless)
    {
        try
        {
            const std::size_t definition_count =
                sound_definitions.SoundSystem_LoadForMission(options.game_directory, scene.mission_number);
            try
            {
                static_cast<void>(animation_trigger_registry.AnimationTriggerRegistry_LoadForGame(
                    options.game_directory));
            }
            catch (const std::exception& exception)
            {
                log(Severity::warning,
                    "animation-audio",
                    "Retail animation sound triggers could not be loaded: " + std::string(exception.what()));
            }
            const std::filesystem::path common_sound_directory =
                options.game_directory / "common" / "sounds";
            const std::filesystem::path mission_sound_directory =
                options.game_directory / "missions" / "location0" /
                ("level" + std::to_string(scene.mission_number)) / "sounds";
            const std::size_t common_sound_count = sound_bank.SoundSystem_MountDirectory(common_sound_directory);
            const std::size_t mission_sound_count =
                sound_bank.SoundSystem_MountDirectory(mission_sound_directory);
            log(Severity::information,
                "audio",
                "Loaded " + std::to_string(definition_count) + " sound definitions and mounted " +
                    std::to_string(common_sound_count) + " common plus " +
                    std::to_string(mission_sound_count) + " mission sound assets");

            audio_output = std::make_unique<igi::audio::SdlAudioOutput>(audio_mixer);
            if (!audio_output->IsAvailable())
            {
                log(Severity::warning, "audio", std::string(audio_output->FailureReason()));
            }
            else
            {
                // The port starts directly in gameplay without the retail intro cutscene transition.
                // The C# vanilla MusicPlayer plays this mission's game_music bank track as a loop.
                igi::audio::RetailVoiceParameters mission_music_parameters;
                mission_music_parameters.looping = true;
                const std::uint32_t music_voice =
                    sound_system.SoundSystem_PlayBankSound("game_music", mission_music_parameters);
                if (music_voice == 0)
                {
                    log(Severity::warning, "audio", "Level mission music could not start");
                }
                else
                {
                    log(Severity::information,
                        "audio",
                        "Started looping Level " + std::to_string(scene.mission_number) + " music; voice=" +
                            std::to_string(music_voice));
                }
            }
        }
        catch (const std::exception& exception)
        {
            log(Severity::warning, "audio", "Audio startup failed; gameplay continues silently: " +
                                                   std::string(exception.what()));
        }
    }
    else
    {
        log(Severity::information, "audio", "Audio output is disabled for this headless run");
    }

    igi::gameplay::GameSimulation simulation(scene);
    std::vector<const igi::gameplay::SceneWeapon*> actor_sound_weapons;
    std::vector<std::unique_ptr<igi::gameplay::WeaponSoundPlayer>> actor_weapon_sound_players;
    std::vector<std::uint64_t> actor_shot_counts_before_tick(simulation.actors().size());
    actor_sound_weapons.reserve(scene.actors.size());
    actor_weapon_sound_players.reserve(scene.actors.size());
    for (const auto& actor_definition : scene.actors)
    {
        const auto weapon_definition = std::find_if(
            scene.weapons.begin(),
            scene.weapons.end(),
            [&actor_definition](const auto& candidate)
            { return candidate.identifier == actor_definition.weapon_identifier; });
        actor_sound_weapons.push_back(
            weapon_definition == scene.weapons.end() ? nullptr : &*weapon_definition);
        actor_weapon_sound_players.push_back(
            std::make_unique<igi::gameplay::WeaponSoundPlayer>(sound_system));
    }
    if (simulation.actors().size() != scene.actors.size())
    {
        throw std::runtime_error("Mission actor audio mapping does not match the simulation actor count");
    }
    simulation.set_difficulty(options.difficulty);
    log(Severity::information,
        "gameplay",
        std::string("Difficulty=") +
            (options.difficulty == 0 ? "easy" : options.difficulty == 2 ? "hard" : "medium") +
            "; target_fps=" + (options.target_fps > 0 ? std::to_string(options.target_fps) : "uncapped"));
    igi::gameplay::InputController controls(
        igi::gameplay::load_input_configuration(options.game_directory, options.profile));
    igi::gameplay::SceneRenderer frame(
        options.width, options.height, 0, options.texture_filter_mode, selected_anisotropy);
    frame.set_detail(options.terrain_detail);
    const auto replay = load_replay(options.replay);
    std::ofstream trace;
    if (!options.trace.empty())
    {
        trace.open(options.trace);
        if (!trace)
        {
            throw std::runtime_error("Cannot open state trace: " + options.trace.string());
        }
        trace << "tick,x,y,z,grounded,locomotion,health,magazine,reserve,shots,"
                 "hits,kills,projectiles,recoil_pitch,recoil_yaw,accumulated_recoil_pitch,"
                 "accumulated_recoil_yaw,view_heading,view_pitch,random_accumulator,"
                 "random_feedback,random_shift\n";
        trace << std::setprecision(9);
    }
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window(nullptr, SDL_DestroyWindow);
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> display(nullptr, SDL_DestroyRenderer);
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture(nullptr, SDL_DestroyTexture);
    if (!options.headless)
    {
        const auto title = "IGI - Level " + std::to_string(options.mission);
        window.reset(SDL_CreateWindow(title.c_str(),
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      options.width,
                                      options.height,
                                      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
        if (!window)
        {
            throw std::runtime_error("Cannot create game window: " + std::string(SDL_GetError()));
        }
        if (options.fullscreen && SDL_SetWindowFullscreen(window.get(), SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
        {
            throw std::runtime_error("Cannot enter fullscreen: " + std::string(SDL_GetError()));
        }
        SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        display.reset(
            SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
        if (!display)
        {
            display.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_SOFTWARE));
        }
        if (!display)
        {
            throw std::runtime_error("Cannot create display renderer: " + std::string(SDL_GetError()));
        }
        texture.reset(SDL_CreateTexture(display.get(),
                                        SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        options.width,
                                        options.height));
        if (!texture || SDL_RenderSetLogicalSize(display.get(), options.width, options.height) != 0)
        {
            throw std::runtime_error("Cannot configure display texture: " + std::string(SDL_GetError()));
        }
        if (SDL_SetRelativeMouseMode(SDL_TRUE) != 0)
        {
            log(Severity::warning, "input", SDL_GetError());
        }
    }
    log(Severity::information,
        "application",
        "Mission ready. Controls loaded from original config.qvm; Enter restarts, "
        "F10 toggles fullscreen, F9 or G toggles developer mode (god/weapons/ammo), F12 toggles free camera, "
        "Escape quits");
    bool running = true;
    std::uint64_t frame_count = 0;
    auto previous_time = Clock::now();
    const auto start_time = previous_time;
    double accumulator = 0;
    float heading_change = 0;
    float pitch_change = 0;
    int requested_weapon_slot = -1;
    int requested_weapon_cycle = 0;
    bool paused = false;
    igi::gameplay::FreeCamera free_camera{};
    while (running && (!options.frames || frame_count < *options.frames))
    {
        SDL_Event event{};
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
            {
                paused = true;
                heading_change = 0;
                pitch_change = 0;
                requested_weapon_slot = -1;
                requested_weapon_cycle = 0;
                controls.reset();
                if (SDL_SetRelativeMouseMode(SDL_FALSE) != 0)
                {
                    log(Severity::warning, "input", SDL_GetError());
                }
                log(Severity::information, "application", "Paused while game window is inactive");
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
            {
                paused = false;
                previous_time = Clock::now();
                if (SDL_SetRelativeMouseMode(SDL_TRUE) != 0)
                {
                    log(Severity::warning, "input", SDL_GetError());
                }
            }
            if (event.type == SDL_MOUSEWHEEL)
            {
                const int wheel =
                    event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -event.wheel.y : event.wheel.y;
                if (simulation.is_scoped() && !free_camera.active)
                {
                    simulation.adjust_zoom(wheel);
                }
                else
                {
                    requested_weapon_cycle = wheel > 0 ? -1 : wheel < 0 ? 1 : 0;
                }
            }
            if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                }
                if (event.key.keysym.sym == SDLK_RETURN)
                {
                    weapon_sound_player.WeaponSoundPlayer_Reset();
                    simulation.restart();
                    if (free_camera.active)
                    {
                        free_camera.position = simulation.camera_position();
                        free_camera.heading = simulation.view_heading();
                        free_camera.pitch = simulation.view_pitch();
                    }
                }
                if (event.key.keysym.sym == SDLK_F10 && window)
                {
                    const bool fullscreen =
                        (SDL_GetWindowFlags(window.get()) & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
                    if (SDL_SetWindowFullscreen(window.get(),
                                                fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
                    {
                        log(Severity::warning, "display", SDL_GetError());
                    }
                }
                if (igi::gameplay::is_developer_mode_toggle_event(event))
                {
                    simulation.toggle_developer_mode();
                    log(Severity::information,
                        "developer",
                        std::string("Developer mode ") + (simulation.developer_mode()
                                                              ? "ENABLED (God mode / All weapons / Inf ammo)"
                                                              : "DISABLED"));
                }
                if (event.key.keysym.sym == SDLK_F12)
                {
                    free_camera.active = !free_camera.active;
                    if (free_camera.active)
                    {
                        free_camera.position = simulation.camera_position();
                        free_camera.heading = simulation.view_heading();
                        free_camera.pitch = simulation.view_pitch();
                        log(Severity::information,
                            "camera",
                            "Free camera enabled (WASD: roam, Space: up, Shift: down, F12/T: resume gameplay)");
                    }
                    else
                    {
                        simulation.teleport_player(
                            free_camera.position,
                            free_camera.heading - simulation.weapon().accumulated_recoil_yaw,
                            free_camera.pitch - simulation.weapon().accumulated_recoil_pitch);
                        log(Severity::information, "camera", "Free camera disabled; player teleported to camera position");
                    }
                }
                if (event.key.keysym.sym == SDLK_t && free_camera.active)
                {
                    free_camera.active = false;
                        simulation.teleport_player(
                            free_camera.position,
                            free_camera.heading - simulation.weapon().accumulated_recoil_yaw,
                            free_camera.pitch - simulation.weapon().accumulated_recoil_pitch);
                    log(Severity::information, "camera", "Player teleported to camera position and resumed gameplay");
                }
                if ((event.key.keysym.sym == SDLK_EQUALS || event.key.keysym.sym == SDLK_PLUS ||
                     event.key.keysym.sym == SDLK_RIGHTBRACKET) && simulation.is_scoped())
                {
                    simulation.adjust_zoom(1);
                }
                if ((event.key.keysym.sym == SDLK_MINUS || event.key.keysym.sym == SDLK_UNDERSCORE ||
                     event.key.keysym.sym == SDLK_LEFTBRACKET) && simulation.is_scoped())
                {
                    simulation.adjust_zoom(-1);
                }
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                float sensitivity = controls.mouse_sensitivity() * 0.00625f;
                if (free_camera.active)
                {
                    free_camera.heading -= static_cast<float>(event.motion.xrel) * sensitivity;
                    free_camera.pitch += static_cast<float>(event.motion.yrel) * sensitivity *
                                         (controls.invert_mouse() ? 1 : -1);
                    free_camera.pitch = std::clamp(free_camera.pitch, -1.50f, 1.50f);
                }
                else
                {
                    if (simulation.is_scoped())
                    {
                        sensitivity *= std::clamp(1.0f / simulation.zoom_factor(), 0.08f, 1.0f);
                    }
                    else if (simulation.aiming())
                    {
                        sensitivity *= 0.65f;
                    }
                    heading_change -= static_cast<float>(event.motion.xrel) * sensitivity;
                    pitch_change +=
                        static_cast<float>(event.motion.yrel) * sensitivity * (controls.invert_mouse() ? 1 : -1);
                }
            }
        }
        const auto now = Clock::now();
        const double delta_seconds =
            std::min(std::chrono::duration<double>(now - previous_time).count(), 0.1);
        accumulator += options.headless
                           ? 1.0 / 30.0
                           : std::min(std::chrono::duration<double>(now - previous_time).count(), 0.25);
        previous_time = now;
        if (paused && !options.headless)
        {
            accumulator = 0;
            SDL_Delay(25);
            continue;
        }
        if (free_camera.active && !options.headless)
        {
            const auto* keys = SDL_GetKeyboardState(nullptr);
            if (keys)
            {
                const float dt = static_cast<float>(delta_seconds > 0.0 ? delta_seconds : (1.0 / 60.0));
                constexpr float speed = 30.0f;

                const float heading = free_camera.heading;
                const float pitch = free_camera.pitch;
                const Vector3 forward{-std::sin(heading) * std::cos(pitch),
                                      std::cos(heading) * std::cos(pitch),
                                      std::sin(pitch)};
                const Vector3 right{std::cos(heading), std::sin(heading), 0.0f};

                Vector3 move_dir{};
                if (keys[SDL_SCANCODE_W])
                {
                    move_dir = move_dir + forward;
                }
                if (keys[SDL_SCANCODE_S])
                {
                    move_dir = move_dir - forward;
                }
                if (keys[SDL_SCANCODE_D])
                {
                    move_dir = move_dir + right;
                }
                if (keys[SDL_SCANCODE_A])
                {
                    move_dir = move_dir - right;
                }

                const float len = length(move_dir);
                if (len > 0.001f)
                {
                    free_camera.position = free_camera.position + (move_dir * (speed * dt / len));
                }
                if (keys[SDL_SCANCODE_SPACE])
                {
                    free_camera.position.z += speed * dt;
                }
                if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT])
                {
                    free_camera.position.z -= speed * dt;
                }
            }
        }
        while (accumulator + 0.00000001 >= 1.0 / 30.0)
        {
            PlayerInput input;
            if (!replay.empty())
            {
                if (simulation.tick_count() < replay.size())
                {
                    input = replay[simulation.tick_count()];
                }
            }
            else if (options.demonstration)
            {
                input = demonstration_input(simulation.tick_count());
            }
            else if (!options.headless)
            {
                if (free_camera.active)
                {
                    input = {};
                }
                else
                {
                    int keyboard_size = 0;
                    const auto* keys = SDL_GetKeyboardState(&keyboard_size);
                    const auto buttons = SDL_GetMouseState(nullptr, nullptr);
                    input = controls.sample({keys, static_cast<std::size_t>(keyboard_size)}, buttons);
                    if (keys && (keys[SDL_SCANCODE_Q] || keys[SDL_SCANCODE_E] || keys[SDL_SCANCODE_F]))
                    {
                        input.interacting = true;
                    }
                    input.weapon_slot = requested_weapon_slot;
                    if (requested_weapon_cycle != 0)
                    {
                        input.weapon_cycle = requested_weapon_cycle;
                    }
                    input.heading_change = heading_change;
                    input.pitch_change = pitch_change;
                }
            }
            heading_change = 0;
            pitch_change = 0;
            requested_weapon_slot = -1;
            requested_weapon_cycle = 0;
            const int player_animation_identifier_before_tick = simulation.weapon().animation_identifier;
            const float player_animation_time_before_tick = simulation.weapon().animation_time;
            const std::uint64_t shots_before_tick = simulation.weapon().shots_fired;
            if (audio_output && audio_output->IsAvailable())
            {
                for (std::size_t actor_index = 0; actor_index < simulation.actors().size(); ++actor_index)
                {
                    actor_shot_counts_before_tick[actor_index] = simulation.actors()[actor_index].shots_fired;
                }
            }
            simulation.tick(input);
            // REA/Ghidra confirms the retail DefineAnimTrigger registration at 0x004ebe70 and
            // task-handler dispatch in its callback at 0x004ebeb0. The open-old/closed-new
            // interval sweep remains C#-derived until a paired retail runtime trace confirms it.
            if (audio_output && audio_output->IsAvailable() &&
                animation_trigger_registry.AnimationTriggerRegistry_Count() > 0 &&
                scene.first_person_animations)
            {
                const auto& weapon_state = simulation.weapon();
                if (weapon_state.animation_identifier == player_animation_identifier_before_tick)
                {
                    const auto animation_clip_iterator =
                        scene.first_person_animations->clips.find(weapon_state.animation_identifier);
                    if (animation_clip_iterator != scene.first_person_animations->clips.end())
                    {
                        const auto& animation_clip = animation_clip_iterator->second;
                        const bool player_fired_this_tick =
                            weapon_state.shots_fired > shots_before_tick;
                        const bool animation_time_wrapped =
                            !player_fired_this_tick &&
                            animation_clip.duration > 0.0F &&
                            weapon_state.animation_time < player_animation_time_before_tick &&
                            player_animation_time_before_tick + 160.0F >= animation_clip.duration;
                        const auto crossed_animation_sounds =
                            igi::audio::AnimationEventDispatcher_CollectCrossedSounds(
                                animation_clip,
                                player_animation_time_before_tick,
                                weapon_state.animation_time,
                                animation_time_wrapped,
                                animation_trigger_registry);
                        for (const auto& animation_sound_event : crossed_animation_sounds)
                        {
                            const std::string& sound_definition_name =
                                animation_sound_event.trigger_definition->sound_definition_name;
                            const std::uint32_t sound_voice =
                                sound_system.SoundSystem_PlayDefinition(sound_definition_name);
                            if (sound_voice == 0)
                            {
                                log(Severity::warning,
                                    "animation-audio",
                                    "Could not play animation event " +
                                        std::to_string(animation_sound_event.animation_event->event_identifier) +
                                        " sound '" + sound_definition_name + "'");
                            }
                            else
                            {
                                log(Severity::debug,
                                    "animation-audio",
                                    "Animation event " +
                                        std::to_string(animation_sound_event.animation_event->event_identifier) +
                                        " in clip " + std::to_string(weapon_state.animation_identifier) +
                                        " triggered sound '" + sound_definition_name + "' at time " +
                                        std::to_string(animation_sound_event.animation_event->time) +
                                        "; voice=" + std::to_string(sound_voice));
                            }
                        }
                    }
                }
            }
            const std::string_view player_fall_sound = simulation.take_player_fall_sound();
            const std::string_view player_jump_sound = simulation.take_player_jump_sound();
            for (const std::string_view player_sound_name : {player_fall_sound, player_jump_sound})
            {
                if (player_sound_name.empty() || !audio_output || !audio_output->IsAvailable())
                {
                    continue;
                }
                const std::uint32_t player_sound_voice = sound_system.SoundSystem_PlayDefinition(player_sound_name);
                if (player_sound_voice == 0)
                {
                    log(Severity::warning,
                        "audio",
                        "Could not play player sound definition: " + std::string(player_sound_name));
                }
                else
                {
                    log(Severity::debug,
                        "audio",
                        "Played retail player sound '" + std::string(player_sound_name) + "'; voice=" +
                            std::to_string(player_sound_voice));
                }
            }
            if (audio_output && audio_output->IsAvailable())
            {
                const bool fired_this_tick = simulation.weapon().shots_fired > shots_before_tick;
                weapon_sound_player.WeaponSoundPlayer_Tick(fired_this_tick, simulation.weapon_definition());

                const Vector3 listener_position = simulation.camera_position();
                for (std::size_t actor_index = 0; actor_index < simulation.actors().size(); ++actor_index)
                {
                    const auto& actor_state = simulation.actors()[actor_index];
                    const auto* actor_weapon = actor_sound_weapons[actor_index];
                    igi::audio::RetailVoiceParameters actor_sound_parameters;
                    if (actor_weapon != nullptr && !actor_weapon->fire_sound_name.empty())
                    {
                        Vector3 emitter_position = actor_state.position + Vector3{0.0F, 0.0F, 1.5F};
                        float minimum_distance_metres = 1.0F;
                        float maximum_distance_metres = 50.0F;
                        const igi::audio::SoundDefinition* sound_definition = nullptr;
                        if (sound_definitions.SoundSystem_TryResolve(
                                actor_weapon->fire_sound_name, sound_definition) &&
                            sound_definition != nullptr && sound_definition->SoundSystem_IsPlayable())
                        {
                            constexpr double engine_units_per_metre = 4096.0;
                            emitter_position.x +=
                                static_cast<float>(sound_definition->offset_x / engine_units_per_metre);
                            emitter_position.y +=
                                static_cast<float>(sound_definition->offset_y / engine_units_per_metre);
                            emitter_position.z +=
                                static_cast<float>(sound_definition->offset_z / engine_units_per_metre);
                            minimum_distance_metres = sound_definition->minimum_distance_metres;
                            maximum_distance_metres = sound_definition->maximum_distance_metres;
                        }

                        const auto spatial_gain_and_pan = igi::audio::SoundSystem_SpatialiseRetailVoice(
                            1.0F,
                            {emitter_position.x, emitter_position.y, emitter_position.z},
                            {listener_position.x, listener_position.y, listener_position.z},
                            static_cast<double>(simulation.view_heading()),
                            minimum_distance_metres,
                            maximum_distance_metres);
                        actor_sound_parameters.linear_gain = spatial_gain_and_pan.linear_gain;
                        actor_sound_parameters.pan = spatial_gain_and_pan.pan;
                    }

                    const bool actor_fired_this_tick =
                        actor_state.shots_fired > actor_shot_counts_before_tick[actor_index];
                    actor_weapon_sound_players[actor_index]->WeaponSoundPlayer_Tick(
                        actor_fired_this_tick,
                        actor_weapon,
                        actor_sound_parameters);
                }
            }
            if (trace.is_open())
            {
                const auto position = simulation.player_position();
                const auto& weapon = simulation.weapon();
                const auto random_state = simulation.random_state();
                trace << simulation.tick_count() << ',' << position.x << ',' << position.y << ','
                      << position.z << ',' << simulation.grounded() << ',' << simulation.locomotion_state()
                      << ',' << simulation.health() << ',' << weapon.magazine << ',' << weapon.reserve << ','
                      << weapon.shots_fired << ',' << weapon.hits << ',' << weapon.kills << ','
                      << simulation.projectiles().size() << ',' << weapon.sampled_recoil_pitch << ','
                      << weapon.sampled_recoil_yaw << ',' << weapon.accumulated_recoil_pitch << ','
                      << weapon.accumulated_recoil_yaw << ',' << simulation.view_heading() << ','
                      << simulation.view_pitch() << ',' << random_state[0] << ',' << random_state[1] << ','
                      << random_state[2] << '\n';
                if (!trace)
                {
                    throw std::runtime_error("Cannot write gameplay trace");
                }
            }
            accumulator -= 1.0 / 30.0;
        }
        ++frame_count;
        const bool final_frame = options.frames && frame_count == *options.frames;
        if (!options.headless || options.benchmark || (final_frame && !options.screenshot.empty()))
        {
            frame.render(scene, simulation, free_camera.active ? &free_camera : nullptr);
            if (display)
            {
                if (SDL_UpdateTexture(texture.get(), nullptr, frame.pixels().data(), options.width * 4) !=
                        0 ||
                    SDL_RenderClear(display.get()) != 0 ||
                    SDL_RenderCopy(display.get(), texture.get(), nullptr, nullptr) != 0)
                {
                    throw std::runtime_error("Cannot present game frame: " + std::string(SDL_GetError()));
                }
                SDL_RenderPresent(display.get());
            }
            if (options.target_fps > 0 && !options.headless)
            {
                const double target_frame_time = 1.0 / static_cast<double>(options.target_fps);
                const auto frame_now = Clock::now();
                const double frame_time = std::chrono::duration<double>(frame_now - now).count();
                if (frame_time < target_frame_time)
                {
                    const auto delay_ms = static_cast<Uint32>((target_frame_time - frame_time) * 1000.0);
                    if (delay_ms > 0)
                    {
                        SDL_Delay(delay_ms);
                    }
                }
            }
        }
    }
    if (!options.screenshot.empty())
    {
        frame.render(scene, simulation);
        save_frame(frame, options.screenshot);
    }
    if (trace.is_open())
    {
        trace.flush();
        if (!trace)
        {
            throw std::runtime_error("Cannot finish writing gameplay trace");
        }
    }
    const double elapsed = std::chrono::duration<double>(Clock::now() - start_time).count();
    if (options.benchmark && frame_count > 0 && elapsed > 0.0)
    {
        log(Severity::information,
            "benchmark",
            "Average FPS=" + std::to_string(static_cast<double>(frame_count) / elapsed) +
                ", average frame time=" +
                std::to_string(elapsed * 1000.0 / static_cast<double>(frame_count)) +
                " ms, terrain triangles submitted=" +
                std::to_string(frame.terrain_triangles_submitted()) + "/" +
                std::to_string(scene.terrain_triangle_count) + ", terrain chunks culled=" +
                std::to_string(frame.terrain_chunks_culled()) + "/" +
                std::to_string(scene.terrain_render_chunks.size()));
    }
    if (audio_output)
    {
        weapon_sound_player.WeaponSoundPlayer_Reset();
        if (audio_output->ConsumeCallbackFailure())
        {
            log(Severity::error, "audio", "SDL audio callback reported a mixer failure");
        }
        log(Severity::information,
            "audio",
            "SDL audio callback count=" + std::to_string(audio_output->CallbackCount()));
    }
    log(Severity::information,
        "application",
        "Stopped cleanly; ticks=" + std::to_string(simulation.tick_count()) +
            ", frames=" + std::to_string(frame_count) + ", elapsed=" + std::to_string(elapsed) +
            "s, shots=" + std::to_string(simulation.weapon().shots_fired) +
            ", hits=" + std::to_string(simulation.weapon().hits));
    return 0;
}

} // namespace

int main(int argument_count, char** arguments)
{
    bool interactive = false;
    try
    {
        const auto options = parse_arguments(argument_count, arguments);
        interactive = !options.headless;
        return run(options, arguments[0]);
    }
    catch (const igi::logging::EngineException& engine_error)
    {
        IGI_LOG_ERROR("application",
                      "Engine failure [" << engine_error.subsystem() << "]: " << engine_error.raw_message());
        if (interactive)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Project I.G.I.", engine_error.what(), nullptr);
        }
        return 1;
    }
    catch (const std::exception& exception)
    {
        IGI_LOG_ERROR("application", "Standard failure: " << exception.what());
        if (interactive)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Project I.G.I.", exception.what(), nullptr);
        }
        return 1;
    }
    catch (...)
    {
        IGI_LOG_ERROR("application", "Unexpected non-standard exception at application boundary");
        if (interactive)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                     "Project I.G.I.",
                                     "Unexpected non-standard exception at application boundary",
                                     nullptr);
        }
        return 1;
    }
}
