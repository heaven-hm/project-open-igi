# Platform architecture

## Product target

The current MVP targets macOS. Windows and Linux support are later deliverables,
after a playable macOS gameplay slice exists. The application must reproduce
the original game's behavior on every platform. The renderer may use the
original Direct3D 7 pipeline on Windows for vanilla visual parity; that choice
must stay inside the Windows rendering backend.

## Code boundary

Use one shared, OS-independent game core for every platform. Do not maintain
separate OS branches, forks, or duplicated gameplay implementations. Gameplay,
physics, movement, AI, weapons, QVM execution, asset parsing, and
renderer-independent frame data must not include or call operating-system APIs.
They must compile without platform SDK headers.

Window creation, graphics API calls, input devices, filesystem access, timing,
and audio devices belong in platform backends behind small C++ interfaces. Select
the backend in the application/build configuration and keep platform-specific
conditionals inside the backend or build setup. SDL2 provides the initial C++ window and input backend across target operating
systems. Platform graphics and device implementations still require explicit
verification on each operating system.

## Renderer policy

Keep scene data, material properties, render ordering, and gameplay independent
of graphics APIs. A future Windows vanilla backend may use Direct3D 7 and
DirectDraw to reproduce the retail fixed-function pipeline. macOS and Linux
must use their own supported graphics backends behind the same renderer
interface, with parity measured against the retail output. Do not require
DirectX headers or types in shared engine code. Enhanced graphics remain an
explicit option and must not change vanilla behavior. No Direct3D 7 backend is
implemented or verified yet.

## Current state and acceptance

The current repository builds OS-neutral C++20 gameplay and format libraries,
a shared fixed-step game loop, and an SDL2 application shell. The current
macOS application loads original Level 1 data and renders it through a CPU
software rasterizer. It can select nearest authored mip levels or trilinear
filtering with generated mip levels. This is an interim renderer, not a
Direct3D 7 equivalent; Windows and Linux builds are not yet verified.

A future platform is supported only after its backend builds, launches the same
mission data, and passes platform-specific runtime checks. A successful build of
the portable core alone does not establish full platform support.
