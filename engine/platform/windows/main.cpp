#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include "igi/retail/core/app_context.hpp"
#include "igi/retail/core/debug_command_line.hpp"
#include "igi/retail/diagnostics/log_record.hpp"
#include "igi/retail/model/mef_model_loader.hpp"
#include "igi/retail/resource/resource_archive.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ddraw.h>
#include <filesystem>
#include <iomanip>
#include <iterator>
#include <limits>
#include <shellapi.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <windows.h>

#include "retail_model_scene.hpp"

namespace
{

// Retail IGI.EXE window class and window name are the same string: the "Innerloop"
// literal at VA 0x0053B534, reached through the zClassName pointer at 0x00541D68.
// The retail WinMain passes it as both lpClassName and lpWindowName to CreateWindowEx.
constexpr wchar_t window_class_name[] = L"Innerloop";
constexpr wchar_t window_title[] = L"Innerloop";

// Retail IGI.EXE startup strings.
constexpr char register_winproc_failed[] = "Failed to register WinProc callback."; // VA 0x00541D70
constexpr char create_device_failed[] = "Failed to create device.";                // VA 0x0054E97C
constexpr char obtain_interface_failed[] = "Failed to obtain interface.";          // VA 0x0054E998
constexpr char load_resource_failed_format[] = "Failed to load resource: '%s'";    // VA 0x005470D0

// On-screen diagnostic text. This overlay is the port's own tooling and is shown only when the
// retail debug-text switch is active (App_ContextSetDebugtextState / global 0x005C8BF4); the
// retail game itself draws no such text.
std::vector<std::wstring> debug_overlay_lines;

IDirectDraw7* direct_draw_device = nullptr;
IDirectDrawSurface7* primary_surface = nullptr;

void log_message(std::string_view message,
                 igi::retail::diagnostics::LogLevel log_level = igi::retail::diagnostics::LogLevel::info,
                 std::string_view subsystem = "platform");

std::wstring widen_ascii(const std::string& text)
{
    return std::wstring(text.begin(), text.end());
}

[[nodiscard]] std::string narrow_ascii(std::wstring_view text)
{
    std::string result;
    result.reserve(text.size());
    for (const wchar_t character : text)
    {
        if (static_cast<std::uint32_t>(character) > 0x7fU)
        {
            throw std::runtime_error("Resource names must contain ASCII characters.");
        }
        result.push_back(static_cast<char>(character));
    }
    return result;
}

[[nodiscard]] std::string format_load_resource_failed(std::string_view resource_name)
{
    // Reproduces the retail "Failed to load resource: '%s'" message at VA 0x005470D0.
    std::string message(load_resource_failed_format);
    const std::size_t placeholder = message.find("%s");
    if (placeholder != std::string::npos)
    {
        message.replace(placeholder, 2U, resource_name);
    }
    return message;
}

void load_retail_resource(const std::filesystem::path& resource_path,
                          std::string_view requested_resource_name)
{
    const igi::retail::resource::ResourceArchive archive =
        igi::retail::resource::Host_LoadResourceArchive(resource_path);

    const igi::retail::resource::ResourceEntry* selected_entry = nullptr;
    if (!requested_resource_name.empty())
    {
        selected_entry = archive.find(requested_resource_name);
    }
    else
    {
        for (const igi::retail::resource::ResourceEntry& entry : archive.entries)
        {
            if (entry.name.ends_with(".mef"))
            {
                selected_entry = &entry;
                break;
            }
        }
    }

    if (selected_entry == nullptr)
    {
        throw std::runtime_error(format_load_resource_failed(requested_resource_name));
    }

    const igi::retail::model::MefModel model =
        igi::retail::model::MefModel::DynObj_LoadModelMesh(selected_entry->body);
    const igi::retail::model::MefRenderChunks render_chunks = model.render_chunks();

    std::ostringstream resource_log;
    resource_log << "Host_LoadResourceArchive completed; entries=" << archive.entries.size()
                 << "; selected=" << selected_entry->name << "; bytes=" << selected_entry->body.size();
    log_message(resource_log.str(), igi::retail::diagnostics::LogLevel::info, "resource");

    std::ostringstream model_log;
    model_log << "DynObj_LoadModelMesh completed; version=" << model.version
              << "; header_bytes=" << model.mesh_header.size() << "; chunks=" << model.container.chunks.size()
              << "; D3DR_bytes="
              << (render_chunks.d3d_render_header == nullptr
                      ? 0U
                      : render_chunks.d3d_render_header->payload.size())
              << "; DNER_bytes="
              << (render_chunks.draw_records == nullptr ? 0U : render_chunks.draw_records->payload.size())
              << "; XTRV_bytes="
              << (render_chunks.vertex_stream == nullptr ? 0U : render_chunks.vertex_stream->payload.size());
    log_message(model_log.str(), igi::retail::diagnostics::LogLevel::info, "model");

    debug_overlay_lines.clear();
    if (igi::retail::core::AppContext_Get().debug_text_state != 0)
    {
        std::wostringstream version_line;
        version_line << L"MESH version: " << std::fixed << std::setprecision(1) << model.version;

        debug_overlay_lines = {
            L"Resource_Load - VA 0x004B5F00.",
            L"Sys_Resource - VA 0x004B5F80.",
            L"DynObj_LoadModelMesh - VA 0x004CF390.",
            L"Resource entries loaded: " + std::to_wstring(archive.entries.size()),
            L"Selected resource: " + widen_ascii(selected_entry->name),
            L"Embedded resource bytes: " + std::to_wstring(selected_entry->body.size()),
            version_line.str(),
            L"MESH header bytes: " + std::to_wstring(model.mesh_header.size()),
            L"MEF chunks: " + std::to_wstring(model.container.chunks.size()),
        };

        const std::size_t displayed_entry_count = std::min<std::size_t>(archive.entries.size(), 5U);
        for (std::size_t entry_index = 0U; entry_index < displayed_entry_count; ++entry_index)
        {
            const igi::retail::resource::ResourceEntry& entry = archive.entries[entry_index];
            debug_overlay_lines.push_back(L"  " + widen_ascii(entry.name) + L"   " +
                                          std::to_wstring(entry.body.size()) + L" bytes");
        }
    }
}

// Applies the host arguments the retail executable does not define (--res, --asset) plus the
// IGIPatch-compatible debug switches. Returns false when a fatal host argument error occurred.
bool apply_command_line(int argument_count, wchar_t** arguments)
{
    // Owned storage first: the string_views below must not outlive their strings.
    std::vector<std::string> debug_switch_storage;
    std::filesystem::path resource_path;
    std::string requested_resource_name;

    for (int argument_index = 1; argument_index < argument_count; ++argument_index)
    {
        const std::wstring_view argument(arguments[argument_index]);
        if (argument == L"--res" && argument_index + 1 < argument_count)
        {
            resource_path = std::filesystem::path(arguments[argument_index + 1]);
            ++argument_index;
        }
        else if (argument == L"--asset" && argument_index + 1 < argument_count)
        {
            requested_resource_name = narrow_ascii(arguments[argument_index + 1]);
            ++argument_index;
        }
        else
        {
            debug_switch_storage.push_back(narrow_ascii(argument));
        }
    }

    std::vector<std::string_view> debug_switches;
    debug_switches.reserve(debug_switch_storage.size());
    for (const std::string& stored : debug_switch_storage)
    {
        debug_switches.push_back(stored);
    }

    static_cast<void>(igi::retail::core::debug_command_line::ApplyDebugFeatureCommandLine(debug_switches));

    if (!resource_path.empty())
    {
        load_retail_resource(resource_path, requested_resource_name);
    }
    return true;
}

void log_message(std::string_view message,
                 igi::retail::diagnostics::LogLevel log_level,
                 std::string_view subsystem)
{
    const std::string formatted_record = igi::retail::diagnostics::Log_FormatTimestampedRecord(
        log_level, std::chrono::system_clock::now(), subsystem, message);
    OutputDebugStringA(formatted_record.c_str());

    HANDLE log_file = CreateFileW(L"open-igi.log",
                                  FILE_APPEND_DATA,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  nullptr,
                                  OPEN_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  nullptr);
    if (log_file != INVALID_HANDLE_VALUE)
    {
        DWORD bytes_written = 0U;
        const DWORD record_size = static_cast<DWORD>(formatted_record.size());
        const BOOL write_succeeded =
            WriteFile(log_file, formatted_record.data(), record_size, &bytes_written, nullptr);
        CloseHandle(log_file);
        if (write_succeeded == FALSE || bytes_written != record_size)
        {
            OutputDebugStringA("[open-igi] Failed to append the diagnostic record to open-igi.log.\r\n");
        }
    }
    else
    {
        OutputDebugStringA("[open-igi] Failed to open open-igi.log for appending.\r\n");
    }
}

void release_graphics()
{
    if (primary_surface != nullptr)
    {
        primary_surface->Release();
        primary_surface = nullptr;
    }

    if (direct_draw_device != nullptr)
    {
        direct_draw_device->Release();
        direct_draw_device = nullptr;
    }
}

bool initialize_direct_draw(HWND window_handle)
{
    const HRESULT create_result =
        DirectDrawCreateEx(nullptr, reinterpret_cast<void**>(&direct_draw_device), IID_IDirectDraw7, nullptr);
    if (FAILED(create_result))
    {
        log_message(obtain_interface_failed);
        return false;
    }

    const HRESULT cooperative_level_result =
        direct_draw_device->SetCooperativeLevel(window_handle, DDSCL_NORMAL);
    if (FAILED(cooperative_level_result))
    {
        log_message(obtain_interface_failed);
        return false;
    }

    DDSURFACEDESC2 surface_description{};
    surface_description.dwSize = sizeof(surface_description);
    surface_description.dwFlags = DDSD_CAPS;
    surface_description.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    const HRESULT surface_result =
        direct_draw_device->CreateSurface(&surface_description, &primary_surface, nullptr);
    if (FAILED(surface_result))
    {
        log_message(create_device_failed);
        return false;
    }

    log_message("DirectDraw 7 primary surface created.");
    return true;
}

LRESULT CALLBACK window_procedure(HWND window_handle,
                                  UINT message,
                                  WPARAM word_parameter,
                                  LPARAM long_parameter)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT paint_state{};
        HDC device_context = BeginPaint(window_handle, &paint_state);

        RECT client_rectangle{};
        GetClientRect(window_handle, &client_rectangle);
        FillRect(device_context, &client_rectangle, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

        if (igi::retail::core::AppContext_Get().debug_text_state != 0)
        {
            SetBkMode(device_context, TRANSPARENT);
            SetTextColor(device_context, RGB(255, 255, 255));

            int text_y = 24;
            for (const std::wstring& line : debug_overlay_lines)
            {
                TextOutW(device_context, 24, text_y, line.c_str(), static_cast<int>(line.size()));
                text_y += 24;
            }
        }

        EndPaint(window_handle, &paint_state);
        return 0;
    }
    case WM_CLOSE:
        DestroyWindow(window_handle);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(window_handle, message, word_parameter, long_parameter);
    }
}

} // namespace

int WINAPI wWinMain(HINSTANCE instance_handle, HINSTANCE, PWSTR, int show_command)
{
    int argument_count = 0;
    wchar_t** arguments = CommandLineToArgvW(GetCommandLineW(), &argument_count);
    if (arguments == nullptr)
    {
        log_message(register_winproc_failed);
        return 1;
    }
    std::filesystem::path capture_path;
    bool model_scene_requested = false;
    bool dev_mode = false;
    bool input_demo = false;
    // Fullscreen by default, like the retail executable; "window" selects windowed mode.
    bool windowed = false;
    unsigned int level_number = 1U;
    std::wstring ignored_retail_tokens;

    // Retail command-line dispatch, transcribed from igi.exe VA 0x0048F360: the raw command
    // line is split on space/tab, each registered word is matched against every token as a
    // case-insensitive PREFIX (the token length must be at least the word length; the retail
    // loop compares both sides with the 'a'..'z' fold at 0x0048F401), and the handler receives
    // the token remainder. Retail main registers "Window" (0x0048F350 -> SetWindowMode(0),
    // a decorated 640x480 window), "WinInput", "Germany", "USA", and "HardcoreGerman";
    // the level selection travels through the filesys.cfg script probe. Dev-only options used
    // by this reconstruction (--capture-frame, devmode) are dispatched first and are not part
    // of the retail vocabulary.
    auto token_equals = [](const std::wstring& token, const wchar_t* word) {
        const std::size_t word_length = std::wcslen(word);
        if (token.size() < word_length)
        {
            return false;
        }
        for (std::size_t index = 0; index < word_length; ++index)
        {
            const auto fold = [](wchar_t character) {
                return character >= L'a' && character <= L'z' ? static_cast<wchar_t>(character - L'a' + L'A')
                                                              : character;
            };
            if (fold(token[index]) != fold(word[index]))
            {
                return false;
            }
        }
        return true;
    };
    for (int argument_index = 1; argument_index < argument_count; ++argument_index)
    {
        const std::wstring token(arguments[argument_index]);
        if (token == L"--capture-frame" && argument_index + 1 < argument_count)
        {
            // Port development extension: capture one frame and exit.
            capture_path = arguments[++argument_index];
            model_scene_requested = true;
            continue;
        }
        if (token_equals(token, L"window"))
        {
            // Retail 0x0048F350: SetWindowMode(0) -> decorated window, 640x480 client.
            model_scene_requested = true;
            windowed = true;
            continue;
        }
        if (token_equals(token, L"level"))
        {
            // Retail filesys.cfg probes the level word; the port parses the remainder like
            // the retail script does (level1 .. level14).
            const std::wstring remainder = token.substr(5);
            wchar_t* end = nullptr;
            const unsigned long parsed_level = std::wcstoul(remainder.c_str(), &end, 10);
            if (remainder.empty() || end == remainder.c_str() || *end != L'\0' || parsed_level < 1UL || parsed_level > 14UL)
            {
                LocalFree(arguments);
                MessageBoxW(nullptr, L"Expected level1 through level14.", window_title, MB_OK | MB_ICONERROR);
                return 1;
            }
            level_number = static_cast<unsigned int>(parsed_level);
            model_scene_requested = true;
            continue;
        }
        if (token_equals(token, L"devmode"))
        {
            // Port development extension (free camera, developer keys).
            dev_mode = true;
            model_scene_requested = true;
            continue;
        }
        if (token_equals(token, L"inputdemo"))
        {
            // Port development extension: post the retail key sequence to the
            // scene window so the input port, the human controller and the
            // camera can be exercised without a physical keyboard.
            input_demo = true;
            model_scene_requested = true;
            continue;
        }
        // Recognised retail vocabulary this reconstruction does not model yet; keep them
        // observable instead of silently unknown (retail: WinInput 0x0048F920, Germany
        // 0x0048F930, USA 0x0048F950, HardcoreGerman 0x0048F7B4).
        if (token_equals(token, L"wininput") || token_equals(token, L"germany") ||
            token_equals(token, L"usa") || token_equals(token, L"hardcoregerman"))
        {
            ignored_retail_tokens += token + L" ";
            continue;
        }
    }
    if (!ignored_retail_tokens.empty())
    {
        std::wstring message = L"Retail-only command-line options ignored by this build: " + ignored_retail_tokens;
        std::string narrow(message.begin(), message.end());
        log_message(narrow);
    }
    // The debug switches must be applied before the scene branch: "level1 DebugKeys" has to take
    // effect for the scene run, not only for the resource-window mode.
    apply_command_line(argument_count, arguments);

    if (model_scene_requested)
    {
        LocalFree(arguments);
        wchar_t executable_path[32768]{};
        const DWORD path_length = GetModuleFileNameW(nullptr, executable_path, 32768U);
        if (path_length == 0U || path_length >= 32768U)
        {
            return 1;
        }
        const auto game_directory = std::filesystem::path(executable_path).parent_path();
        const int scene_result = RunRetailModelScene(
            instance_handle, show_command, game_directory, level_number, capture_path, dev_mode, windowed,
            input_demo);
        return scene_result;
    }

    LocalFree(arguments);

    WNDCLASSEXW window_class{};
    window_class.cbSize = sizeof(window_class);
    window_class.lpfnWndProc = window_procedure;
    window_class.hInstance = instance_handle;
    window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    window_class.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    window_class.lpszClassName = window_class_name;

    if (RegisterClassExW(&window_class) == 0)
    {
        log_message(register_winproc_failed);
        return 1;
    }

    RECT client_rectangle{0, 0, 640, 480};
    AdjustWindowRect(&client_rectangle, WS_OVERLAPPEDWINDOW, FALSE);

    HWND window_handle = CreateWindowExW(0,
                                         window_class_name,
                                         window_title,
                                         WS_OVERLAPPEDWINDOW,
                                         CW_USEDEFAULT,
                                         CW_USEDEFAULT,
                                         client_rectangle.right - client_rectangle.left,
                                         client_rectangle.bottom - client_rectangle.top,
                                         nullptr,
                                         nullptr,
                                         instance_handle,
                                         nullptr);
    if (window_handle == nullptr)
    {
        log_message(register_winproc_failed);
        return 1;
    }

    if (!initialize_direct_draw(window_handle))
    {
        release_graphics();
        DestroyWindow(window_handle);
        return 1;
    }

    ShowWindow(window_handle, show_command);
    UpdateWindow(window_handle);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    release_graphics();
    return static_cast<int>(message.wParam);
}
