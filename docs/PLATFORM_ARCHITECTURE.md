# Platform architecture

## Product target

The current MVP targets macOS. Windows and Linux support are later deliverables,
after a playable macOS gameplay slice exists. The application must reproduce
the original game's behavior and appearance without using its Windows or DirectX
implementation.

## Code boundary

The game core is OS-independent from the start. Gameplay, physics, movement, AI,
weapons, QVM execution, asset parsing, and renderer-independent frame data must
not include or call operating-system APIs. They must compile without platform
SDK headers.

Window creation, graphics API calls, input devices, filesystem access, timing,
and audio devices belong in platform backends behind small C++ interfaces. The
macOS backend will be implemented first. Future Windows and Linux backends implement
the same interfaces without changing game rules or asset parsers.

## Current state and acceptance

The current repository builds OS-neutral C++20 gameplay and format libraries on
macOS. It does not yet contain a game window, renderer backend, full game loop,
or app bundle. No Windows or Linux backend is implemented or verified.

A future platform is supported only after its backend builds, launches the same
mission data, and passes platform-specific runtime checks. A successful build of
the portable core alone does not establish full platform support.
