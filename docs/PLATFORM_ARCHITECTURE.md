# Platform architecture

## Product target

The current MVP targets macOS. Windows and Linux support are later deliverables,
after a playable macOS gameplay slice exists. The application must reproduce
the original game's behavior and appearance without using its Windows or DirectX
implementation.

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

## Current state and acceptance

The current repository builds OS-neutral C++20 gameplay and format libraries,
a shared fixed-step game loop, and a C++ SDL2 application shell with a native macOS window. The shell does not yet render a level or run player gameplay. Windows and Linux builds are not yet verified.

A future platform is supported only after its backend builds, launches the same
mission data, and passes platform-specific runtime checks. A successful build of
the portable core alone does not establish full platform support.
