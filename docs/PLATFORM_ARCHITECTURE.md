# Platform architecture and DirectX 7 toolchain

## Product target

The target executable is a **32-bit x86 Windows program**. The retail game uses Win32 and DirectX APIs; the engine port should retain those platform APIs where they define the original boundary. On this Mac, CrossOver is the compatibility host used to run the Windows executable and original game assets.

The game core and platform boundary remain separate in code organization, but this MVP targets Windows x86 first. Do not add SDL, OpenGL, Vulkan, or another runtime abstraction to the Windows vanilla path. The codebase uses C++20. DirectX headers and import libraries are provided by the Windows toolchain; DirectX APIs are operating-system interfaces, not third-party game-engine dependencies.

## Retail import evidence

The retail `/Users/haseeb-mir/Downloads/IGI/igi.exe` is PE32 x86. Its import table includes `DDRAW.dll`, `DINPUT.dll`, `DSOUND.dll`, and `WINMM.dll`, and imports `DirectDrawCreateEx`, `DirectInputCreateA`, and `DirectInputCreateEx`. This establishes the relevant platform API families; it does not by itself prove which interface versions or call semantics every subsystem uses.

## Required Windows API surface

| Game subsystem | Header/API | Link library | Runtime module |
| --- | --- | --- | --- |
| DirectDraw and Direct3D 7 | `ddraw.h`, `d3d.h`; `IDirectDraw7`, `IDirect3D7` | `ddraw`, `dxguid` | `ddraw.dll` |
| DirectInput 7 | `dinput.h`; set `DIRECTINPUT_VERSION` to `0x0700` before including it | `dinput`, `dxguid` | `dinput.dll` |
| DirectSound 7 | `dsound.h`; set `DIRECTSOUND_VERSION` to `0x0700` before including it | `dsound`, `dxguid` | `dsound.dll` |
| Multimedia timers | `mmsystem.h` | `winmm` | `winmm.dll` |
| COM support | `objbase.h` | `ole32` | `ole32.dll` |

`IDirect3D7` is obtained from the DirectDraw 7 object through COM; there is no separate `d3d7.dll` link dependency. Standard Win32 libraries such as `kernel32`, `user32`, and `gdi32` are part of the Windows platform boundary.

## Toolchain verification on macOS

The installed x86 cross compiler is `/opt/homebrew/bin/i686-w64-mingw32-g++` (MinGW-w64, GCC 16.2.0; package `mingw-w64` 14.0.0). Its i686 sysroot contains `ddraw.h`, `d3d.h`, `dinput.h`, and `dsound.h`, plus `libddraw.a`, `libdxguid.a`, `libdinput.a`, and `libdsound.a` import libraries.

A temporary compile/link probe successfully built as PE32 x86 when using:

```sh
/opt/homebrew/bin/i686-w64-mingw32-g++ \
  -std=c++20 -Wall -Wextra -Wpedantic \
  directx7_probe.cpp -o directx7_probe.exe \
  -lddraw -ldinput -ldsound -ldxguid
```

`DIRECTINPUT_VERSION` and `DIRECTSOUND_VERSION` must be defined as `0x0700` before their headers. Without the DirectInput definition, MinGW defaults to DirectInput 8 and hides the DirectInput 7 factory declarations.

CrossOver 26.3.0 includes 32-bit `ddraw.dll`, `dinput.dll`, and `dsound.dll` implementations under its `lib/wine/i386-windows` directory. That verifies the DLL files are installed, not that each DirectX feature or the current executable works at runtime. A prior bounded REA process-capture attempt could not verify cleanup of its Wine process group, so the current target has **no successful CrossOver runtime verification** yet.

## Evidence gates

- Import names and API entry points are observations from the retail PE import table.
- Header and import-library availability is verified by the local i686 toolchain and the compile/link probe.
- CrossOver DLL presence is an installation check, not proof of rendering, input, or audio parity.
- Verify each ported retail routine against `igi.exe` before treating a decompiler label or C# implementation as ground truth.
- Keep unknown interface versions, object layouts, and runtime behavior explicitly unresolved until static or runtime evidence resolves them.
