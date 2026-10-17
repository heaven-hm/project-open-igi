#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include "igi/retail/model/mef_model_loader.hpp"
#include "igi/retail/diagnostics/log_record.hpp"
#include "igi/retail/resource/resource_archive.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <chrono>
#include <cstdio>
#include <ddraw.h>
#include <filesystem>
#include <fstream>
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

namespace
{

constexpr wchar_t window_class_name[] = L"IGI.DirectDraw7.Window";
constexpr wchar_t window_title[] = L"IGI";

IDirectDraw7* direct_draw_device = nullptr;
IDirectDrawSurface7* primary_surface = nullptr;
std::vector<std::wstring> demo_lines;

void log_message(
    std::string_view message,
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

void load_retail_resource_demo(const std::filesystem::path& resource_path,
                               std::string_view requested_resource_name)
{
    const igi::retail::resource::ResourceArchive archive =
        igi::retail::resource::Resource_Load(resource_path);

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
        throw std::runtime_error("Failed to load resource: 'LOCAL:models/<name>.mef'");
    }

    const igi::retail::model::MefModel model =
        igi::retail::model::MefModel::DynObj_LocalModelsSMef(selected_entry->body);
    const igi::retail::model::MefRenderChunks render_chunks = model.render_chunks();

    std::ostringstream resource_log;
    resource_log << "Resource_Load completed; entries=" << archive.entries.size()
                 << "; selected=" << selected_entry->name
                 << "; bytes=" << selected_entry->body.size();
    log_message(resource_log.str(), igi::retail::diagnostics::LogLevel::info, "resource");

    std::ostringstream model_log;
    model_log << "DynObj_LocalModelsSMef completed; version=" << model.version
              << "; header_bytes=" << model.mesh_header.size()
              << "; chunks=" << model.container.chunks.size()
              << "; D3DR_bytes="
              << (render_chunks.d3d_render_header == nullptr
                      ? 0U
                      : render_chunks.d3d_render_header->payload.size())
              << "; DNER_bytes="
              << (render_chunks.draw_records == nullptr ? 0U : render_chunks.draw_records->payload.size())
              << "; XTRV_bytes="
              << (render_chunks.vertex_stream == nullptr ? 0U : render_chunks.vertex_stream->payload.size());
    log_message(model_log.str(), igi::retail::diagnostics::LogLevel::info, "model");

    std::wostringstream version_line;
    version_line << L"MESH version: " << std::fixed << std::setprecision(1) << model.version;

    demo_lines = {
        L"RETAIL NATIVE RESOURCE DEMO",
        L"DirectDraw 7 window initialized.",
        L"Resource_Load - original VA 0x004B5F00.",
        L"Sys_Resource - original VA 0x004B5F80.",
        L"DynObj_LocalModelsSMef - original VA 0x004CF390.",
        L"Container: ILFF / IRES.",
        L"Resource entries loaded: " + std::to_wstring(archive.entries.size()),
        L"Selected resource: " + widen_ascii(selected_entry->name),
        L"Embedded resource bytes: " + std::to_wstring(selected_entry->body.size()),
        version_line.str(),
        L"MESH header bytes: " + std::to_wstring(model.mesh_header.size()),
        L"MEF chunks: " + std::to_wstring(model.container.chunks.size()),
        L"D3DR / DNER / XTRV bytes: "
            + std::to_wstring(render_chunks.d3d_render_header == nullptr
                                  ? 0U
                                  : render_chunks.d3d_render_header->payload.size())
            + L" / "
            + std::to_wstring(render_chunks.draw_records == nullptr
                                  ? 0U
                                  : render_chunks.draw_records->payload.size())
            + L" / "
            + std::to_wstring(render_chunks.vertex_stream == nullptr
                                  ? 0U
                                  : render_chunks.vertex_stream->payload.size()),
        L"",
        L"RES entries:",
    };

    const std::size_t displayed_entry_count = std::min<std::size_t>(archive.entries.size(), 5U);
    for (std::size_t entry_index = 0U; entry_index < displayed_entry_count; ++entry_index)
    {
        const igi::retail::resource::ResourceEntry& entry = archive.entries[entry_index];
        demo_lines.push_back(L"  " + widen_ascii(entry.name) + L"   " + std::to_wstring(entry.body.size()) +
                             L" bytes");
    }

    demo_lines.push_back(L"");
    demo_lines.push_back(
        L"RES archive and embedded MEF loading are implemented; 3D rendering remains pending.");
}

void initialize_demo_lines(int argument_count, wchar_t** arguments)
{
    demo_lines = {
        L"RETAIL NATIVE RESOURCE DEMO",
        L"Load a Level 1 resource archive:",
        L"open-igi.exe --res <path-to-level.RES>",
        L"Optional: --asset LOCAL:models/name.mef",
    };

    std::filesystem::path resource_path;
    std::string requested_resource_name;

    for (int argument_index = 1; argument_index + 1 < argument_count; ++argument_index)
    {
        const std::wstring_view argument(arguments[argument_index]);
        if (argument == L"--res")
        {
            resource_path = std::filesystem::path(arguments[argument_index + 1]);
            ++argument_index;
        }
        else if (argument == L"--asset")
        {
            requested_resource_name = narrow_ascii(arguments[argument_index + 1]);
            ++argument_index;
        }
    }

    if (resource_path.empty())
    {
        return;
    }

    try
    {
        load_retail_resource_demo(resource_path, requested_resource_name);
    }
    catch (const std::exception& exception)
    {
        demo_lines = {
            L"RETAIL NATIVE RESOURCE DEMO",
            L"Error in resource file.",
            widen_ascii(exception.what()),
        };
        log_message(exception.what(), igi::retail::diagnostics::LogLevel::error, "resource");
    }
}

void log_message(
    std::string_view message,
    igi::retail::diagnostics::LogLevel log_level,
    std::string_view subsystem)
{
    const std::string formatted_record = igi::retail::diagnostics::Log_FormatTimestampedRecord(
        log_level,
        std::chrono::system_clock::now(),
        subsystem,
        message);
    OutputDebugStringA(formatted_record.c_str());

    HANDLE log_file = CreateFileW(
        L"open-igi.log",
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
        const BOOL write_succeeded = WriteFile(
            log_file,
            formatted_record.data(),
            record_size,
            &bytes_written,
            nullptr);
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
        log_message("DirectDrawCreateEx failed.");
        return false;
    }

    const HRESULT cooperative_level_result =
        direct_draw_device->SetCooperativeLevel(window_handle, DDSCL_NORMAL);
    if (FAILED(cooperative_level_result))
    {
        log_message("IDirectDraw7::SetCooperativeLevel failed.");
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
        log_message("IDirectDraw7::CreateSurface failed.");
        return false;
    }

    log_message("DirectDraw 7 initialized with a primary surface.");
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
        SetBkMode(device_context, TRANSPARENT);
        SetTextColor(device_context, RGB(24, 32, 40));

        RECT client_rectangle{};
        GetClientRect(window_handle, &client_rectangle);
        FillRect(device_context, &client_rectangle, reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

        int text_y = 24;
        for (const std::wstring& line : demo_lines)
        {
            TextOutW(device_context, 24, text_y, line.c_str(), static_cast<int>(line.size()));
            text_y += 24;
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
        log_message("CommandLineToArgvW failed.");
        return 1;
    }
    initialize_demo_lines(argument_count, arguments);
    LocalFree(arguments);

    WNDCLASSEXW window_class{};
    window_class.cbSize = sizeof(window_class);
    window_class.lpfnWndProc = window_procedure;
    window_class.hInstance = instance_handle;
    window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    window_class.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    window_class.lpszClassName = window_class_name;

    if (RegisterClassExW(&window_class) == 0)
    {
        log_message("RegisterClassExW failed.");
        return 1;
    }

    RECT client_rectangle{0, 0, 640, 480};
    if (AdjustWindowRect(&client_rectangle, WS_OVERLAPPEDWINDOW, FALSE) == 0)
    {
        log_message("AdjustWindowRect failed.");
        return 1;
    }

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
        log_message("CreateWindowExW failed.");
        return 1;
    }

    if (!initialize_direct_draw(window_handle))
    {
        release_graphics();
        MessageBoxW(window_handle,
                    L"DirectDraw 7 initialization failed. See the CrossOver log.",
                    window_title,
                    MB_OK | MB_ICONERROR);
        DestroyWindow(window_handle);
        return 1;
    }

    ShowWindow(window_handle, show_command);
    UpdateWindow(window_handle);
    log_message("Retail RES native demo window is running.");

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    release_graphics();
    return static_cast<int>(message.wParam);
}
