#include "igi/game_loop.hpp"
#include "igi/input_state.hpp"
#include "igi/platform_application.hpp"

#include <SDL.h>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>

namespace {

[[nodiscard]] bool input_code_for_scancode(
    SDL_Scancode scancode, igi::gameplay::InputCode& input_code) noexcept {
    using igi::gameplay::InputCode;

    switch (scancode) {
        case SDL_SCANCODE_A: input_code = InputCode::KeyA; return true;
        case SDL_SCANCODE_B: input_code = InputCode::KeyB; return true;
        case SDL_SCANCODE_C: input_code = InputCode::KeyC; return true;
        case SDL_SCANCODE_D: input_code = InputCode::KeyD; return true;
        case SDL_SCANCODE_E: input_code = InputCode::KeyE; return true;
        case SDL_SCANCODE_F: input_code = InputCode::KeyF; return true;
        case SDL_SCANCODE_G: input_code = InputCode::KeyG; return true;
        case SDL_SCANCODE_H: input_code = InputCode::KeyH; return true;
        case SDL_SCANCODE_I: input_code = InputCode::KeyI; return true;
        case SDL_SCANCODE_J: input_code = InputCode::KeyJ; return true;
        case SDL_SCANCODE_K: input_code = InputCode::KeyK; return true;
        case SDL_SCANCODE_L: input_code = InputCode::KeyL; return true;
        case SDL_SCANCODE_M: input_code = InputCode::KeyM; return true;
        case SDL_SCANCODE_N: input_code = InputCode::KeyN; return true;
        case SDL_SCANCODE_O: input_code = InputCode::KeyO; return true;
        case SDL_SCANCODE_P: input_code = InputCode::KeyP; return true;
        case SDL_SCANCODE_Q: input_code = InputCode::KeyQ; return true;
        case SDL_SCANCODE_R: input_code = InputCode::KeyR; return true;
        case SDL_SCANCODE_S: input_code = InputCode::KeyS; return true;
        case SDL_SCANCODE_T: input_code = InputCode::KeyT; return true;
        case SDL_SCANCODE_U: input_code = InputCode::KeyU; return true;
        case SDL_SCANCODE_V: input_code = InputCode::KeyV; return true;
        case SDL_SCANCODE_W: input_code = InputCode::KeyW; return true;
        case SDL_SCANCODE_X: input_code = InputCode::KeyX; return true;
        case SDL_SCANCODE_Y: input_code = InputCode::KeyY; return true;
        case SDL_SCANCODE_Z: input_code = InputCode::KeyZ; return true;
        case SDL_SCANCODE_0: input_code = InputCode::Digit0; return true;
        case SDL_SCANCODE_1: input_code = InputCode::Digit1; return true;
        case SDL_SCANCODE_2: input_code = InputCode::Digit2; return true;
        case SDL_SCANCODE_3: input_code = InputCode::Digit3; return true;
        case SDL_SCANCODE_4: input_code = InputCode::Digit4; return true;
        case SDL_SCANCODE_5: input_code = InputCode::Digit5; return true;
        case SDL_SCANCODE_6: input_code = InputCode::Digit6; return true;
        case SDL_SCANCODE_7: input_code = InputCode::Digit7; return true;
        case SDL_SCANCODE_8: input_code = InputCode::Digit8; return true;
        case SDL_SCANCODE_9: input_code = InputCode::Digit9; return true;
        case SDL_SCANCODE_RETURN: input_code = InputCode::Return; return true;
        case SDL_SCANCODE_ESCAPE: input_code = InputCode::Escape; return true;
        case SDL_SCANCODE_SPACE: input_code = InputCode::Space; return true;
        case SDL_SCANCODE_TAB: input_code = InputCode::Tab; return true;
        case SDL_SCANCODE_BACKSPACE: input_code = InputCode::Backspace; return true;
        case SDL_SCANCODE_LSHIFT: input_code = InputCode::LeftShift; return true;
        case SDL_SCANCODE_RSHIFT: input_code = InputCode::RightShift; return true;
        case SDL_SCANCODE_LCTRL: input_code = InputCode::LeftControl; return true;
        case SDL_SCANCODE_RCTRL: input_code = InputCode::RightControl; return true;
        case SDL_SCANCODE_LALT: input_code = InputCode::LeftAlt; return true;
        case SDL_SCANCODE_RALT: input_code = InputCode::RightAlt; return true;
        case SDL_SCANCODE_LEFT: input_code = InputCode::ArrowLeft; return true;
        case SDL_SCANCODE_RIGHT: input_code = InputCode::ArrowRight; return true;
        case SDL_SCANCODE_DOWN: input_code = InputCode::ArrowDown; return true;
        case SDL_SCANCODE_UP: input_code = InputCode::ArrowUp; return true;
        default: return false;
    }
}

[[nodiscard]] bool set_mouse_button_state(
    const SDL_MouseButtonEvent& button_event,
    bool is_down,
    igi::gameplay::InputState& input_state) noexcept {
    igi::gameplay::InputCode input_code = igi::gameplay::InputCode::Unknown;
    switch (button_event.button) {
        case SDL_BUTTON_LEFT:
            input_code = igi::gameplay::InputCode::MouseLeft;
            break;
        case SDL_BUTTON_RIGHT:
            input_code = igi::gameplay::InputCode::MouseRight;
            break;
        case SDL_BUTTON_MIDDLE:
            input_code = igi::gameplay::InputCode::MouseMiddle;
            break;
        default:
            return false;
    }
    return input_state.set_control_state(input_code, is_down);
}

void process_event(
    const SDL_Event& event,
    bool& application_is_running,
    igi::gameplay::InputState& input_state) noexcept {
    switch (event.type) {
        case SDL_QUIT:
            application_is_running = false;
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                application_is_running = false;
            } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                input_state.reset();
            }
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            igi::gameplay::InputCode input_code =
                igi::gameplay::InputCode::Unknown;
            if (input_code_for_scancode(event.key.keysym.scancode, input_code)) {
                (void)input_state.set_control_state(
                    input_code, event.type == SDL_KEYDOWN);
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            (void)set_mouse_button_state(
                event.button, event.type == SDL_MOUSEBUTTONDOWN, input_state);
            break;
        default:
            break;
    }
}

} // namespace

namespace igi::platform {

int run_application(int argument_count, char** arguments) {
    (void)argument_count;
    (void)arguments;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        std::cerr << "[ERROR] [Application] SDL initialization failed: "
                  << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Window* application_window = SDL_CreateWindow(
        "Project Open I.G.I. — Gameplay Loop",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1024,
        640,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (application_window == nullptr) {
        std::cerr << "[ERROR] [Application] Window creation failed: "
                  << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    igi::gameplay::GameLoop game_loop;
    igi::gameplay::InputState input_state;
    igi::gameplay::InputSnapshot last_input_snapshot;
    bool application_is_running = true;
    auto previous_frame_time = std::chrono::steady_clock::now();
    auto previous_title_update_time = previous_frame_time;
    std::uint64_t rendered_frame_count = 0;

    std::cout << "[INFO] [Application] C++ SDL application loop started.\n";

    while (application_is_running) {
        SDL_Event event{};
        while (SDL_PollEvent(&event) != 0) {
            process_event(event, application_is_running, input_state);
        }

        const auto current_frame_time = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed_time =
            current_frame_time - previous_frame_time;
        previous_frame_time = current_frame_time;

        const igi::gameplay::FrameAdvance frame_advance = game_loop.advance(
            elapsed_time.count(), [&](double fixed_delta_seconds) {
                (void)fixed_delta_seconds;
                last_input_snapshot = input_state.consume_tick();
                // The central gameplay-session update will be called here next.
            });
        ++rendered_frame_count;

        SDL_Surface* window_surface = SDL_GetWindowSurface(application_window);
        if (window_surface == nullptr) {
            std::cerr << "[ERROR] [Application] Window surface acquisition failed: "
                      << SDL_GetError() << '\n';
            application_is_running = false;
            break;
        }

        const Uint32 background_color = SDL_MapRGB(
            window_surface->format, 9, 14, 11);
        if (SDL_FillRect(window_surface, nullptr, background_color) != 0 ||
            SDL_UpdateWindowSurface(application_window) != 0) {
            std::cerr << "[ERROR] [Application] Window presentation failed: "
                      << SDL_GetError() << '\n';
            application_is_running = false;
            break;
        }

        const auto title_update_elapsed =
            current_frame_time - previous_title_update_time;
        if (title_update_elapsed >= std::chrono::milliseconds(250)) {
            previous_title_update_time = current_frame_time;
            const std::string window_title =
                "Project Open I.G.I. | frame " +
                std::to_string(rendered_frame_count) + " | tick " +
                std::to_string(frame_advance.total_simulation_steps) +
                " | held " + std::to_string(
                    input_state.current_snapshot().down_control_count()) +
                " | pressed " + std::to_string(
                    last_input_snapshot.pressed_control_count());
            SDL_SetWindowTitle(application_window, window_title.c_str());
        }

        if (frame_advance.discarded_time_seconds >= 0.1) {
            std::cerr << "[WARNING] [Application] Frame backlog discarded "
                      << frame_advance.discarded_time_seconds << " seconds.\n";
        }

        SDL_Delay(1);
    }

    SDL_DestroyWindow(application_window);
    SDL_Quit();
    return 0;
}

} // namespace igi::platform
